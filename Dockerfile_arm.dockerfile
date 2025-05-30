FROM arm64v8/ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive \
    PERL_MM_USE_DEFAULT=1

# Basic setup and install tools
RUN apt-get update && apt-get install --fix-missing -y \
    aptitude \
    build-essential \
    curl \
    dpkg-dev \
    gfortran \
    gcc \
    git \
    libjpeg-dev \
    libpng-dev \
    libtirpc-dev \
    libtk-img \
    libx11-dev \
    libxext-dev \
    libxft-dev \
    libxt-dev \
    libxpm-dev \
    libgl1-mesa-dev \
    libxtst-dev \
    libxrender-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxcomposite-dev \
    libxdamage-dev \
    libxi-dev \
    libexpat1-dev \
    libmotif-dev \
    libgd-perl \
    libnetpbm10-dev \
    libjpeg62-turbo-dev \
    libpdl-dev \
    lsb-release \
    make \
    ncftp \
    perl \
    perl-tk \
    software-properties-common \
    sudo \
    tk \
    vim \
    wget \
    xorg \
    imagemagick \
    expect \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

# Clean any old CPAN state
RUN rm -rf /root/.cpan /root/.cpanm

# Install cpanminus from source (safer than CPAN for ARM)
RUN curl -L https://cpanmin.us | perl - App::cpanminus

# Install Perl modules one by one to identify issues
RUN for mod in \
    Module::Refresh \
    Moose \
    Clone \
    File::ShareDir \
    File::Slurp \
    Shell \
    Test::Compile::Internal \
    Time::HiRes \
    Tk \
    aliased \
    namespace::autoclean \
    MIME::Base64 \
    YAML \
    CPAN::DistnameInfo; do \
    echo "Installing $mod"; \
    cpanm -n --verbose "$mod" || (cat /root/.cpanm/build.log && exit 1); \
    done

# Copy SeisUnix and pgplot content into image
COPY SeisUnix-master/* /usr/local/cwp_su_all_44R22
COPY pgplot /usr/local/pgplot

# Set up environment variables
ENV LD_LIBRARY_PATH=/usr/local/pgplot
ENV LOCAL=/usr/local
ENV PL=$LOCAL/pl 
ENV APP_LIB=$PL/SeismicUnixGui/lib 
ENV CWPROOT=/usr/local/cwp_su_all_44R22 
ENV SeismicUnixGui=/usr/local/pl/SeismicUnixGui/lib/App/SeismicUnixGui 
ENV SeismicUnixGui_script=$SeismicUnixGui/script 
ENV PGPLOT_DIR=/usr/local/pgplot 
ENV PGPLOT_DEV=/XWINDOW 
ENV SIOSEIS=/usr/local/sioseis/sioseis-2024.1.1 
ENV PERL5LIB=$APP_LIB 
ENV DISPLAY=host.docker.internal:0.0
ENV PATH=$PATH:/usr/local/pgplot:/usr/local/sioseis/sioseis-2024.1.1:$CWPROOT/bin:$CWPROOT/src/Sfio/bin:$SeismicUnixGui_script:$SeismicUnixGui/fortran/bin:$SeismicUnixGui/c/bin

WORKDIR /usr/local/cwp_su_all_44R22

# Prepare install_cwp.exp
COPY install_cwp.exp /usr/local/cwp_su_all_44R22/src/install_cwp.exp
RUN dos2unix /usr/local/cwp_su_all_44R22/src/install_cwp.exp
RUN chmod +x /usr/local/cwp_su_all_44R22/src/install_cwp.exp
RUN /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Build CWP components, allow failure for optional parts
RUN cd /usr/local/cwp_su_all_44R22/src \
    && make install || true \
    && make xtinstall || true \
    && make xminstall || true \
    && make mglinstall || true \
    && make finstall || true \
    && make sfinstall || true

# Setup SIOSEIS
RUN mkdir -p /usr/local/sioseis
COPY sioseis-2024.1.1 /usr/local/sioseis/sioseis-2024.1.1/
RUN cd /usr/local/sioseis/sioseis-2024.1.1 && make all

# Setup data dir and extract dataset
RUN mkdir -p /usr/local/data /home/sug_user
COPY data/Servilleta.tz /usr/local/data/Servilleta.tz
RUN tar -xzf /usr/local/data/Servilleta.tz -C /home/sug_user

# Create user and assign permissions
RUN groupadd -r sug_ug || true \
    && useradd -r -g sug_ug -m -s /bin/bash sug_user \
    && chown -R sug_user:sug_ug /home/sug_user \
    && ln -s /home/sug_user /home/username

# Setup GUI clone script
COPY seismic_unix_gui.sh /usr/local/pl/clone.sh
RUN chmod +x /usr/local/pl/clone.sh
RUN /usr/local/pl/clone.sh

# Set default workdir and user
WORKDIR /home/sug_user
RUN mkdir -p /home/sug_user/sug_data
USER sug_user
