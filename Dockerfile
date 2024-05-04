# Use a build stage for cloning the repo with SSH access
FROM ubuntu:22.04 as builder

# Avoid prompts from apt and set CPAN to non-interactive mode
# TODO: maybe combine with large ENV layer near end of this file
ENV DEBIAN_FRONTEND=noninteractive \
    PERL_MM_USE_DEFAULT=1

# Copy the SeisUnix-master directory contents into the image
COPY SeisUnix-master/* /usr/local/cwp_su_all_44R22

# Copy the pgplot directory contents into the image (for FORTRAN plotting)
COPY pgplot usr/local/pgplot

# Set WORKDIR
WORKDIR /usr/local/cwp_su_all_44R22

# Update and install required packages including development tools, X11/Tcl-Tk libraries, 
# and others as specified Also adding the newly required packages
RUN apt-get update
RUN apt-get install --fix-missing -y \
    aptitude \
    build-essential \
    curl \
    dpkg-dev \
    evince \
    gfortran \
    gcc \
    git \
    libjpeg-dev \
    libmotif-dev \
    libpng-dev \
    libtirpc-dev \
    libtk-img \
    libx11-dev \
    libxext-dev \
    libxft-dev \
    libxt-dev \
    lsb-release \
    make \
    ncftp \
    perl \
    software-properties-common \
    sudo \
    tk \
    vim \
    wget \
    xorg \
    imagemagick \
    && rm -rf /var/lib/apt/lists/*

# Install cpanminus for easier module installation
RUN cpan App::cpanminus \
        && cpanm Tk Tk::JFileDialog Tk::Pod

# Install last 2 packages from DL's docs for CPAN setup
RUN cpan Module::Build \
        && cpan TAP::Harness \
        && cpan Moose \
        && cpanm --notest App::SeismicUnixGui \
        && apt-get update \
        && apt-get install -y expect

# Add your expect script and other necessary files
COPY install_cwp.exp /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Adjust permissions and execute the script as needed
RUN chmod +x /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Fix the line endings for the install_cwp.exp script
RUN apt-get update && apt-get install -y dos2unix \
        && dos2unix /usr/local/cwp_su_all_44R22/src/install_cwp.exp \
        && rm -rf /var/lib/apt/lists/*

# Placed here to avoid this error >>> "during container run: libpgplot.so: 
# cannot open shared object file: No such file or directory"
ENV LD_LIBRARY_PATH=/usr/local/pgplot:$LD_LIBRARY_PATH \
    CWPROOT=/usr/local/cwp_su_all_44R22 \
    LOCAL=/usr/local \
    SeismicUnixGui=/usr/local/share/perl/5.34.0/App/SeismicUnixGui \
    SeismicUnixGui_script=/usr/local/share/perl/5.34.0/App/SeismicUnixGui/script \
    PGPLOT_DIR=/usr/local/pgplot \
    PGPLOT_DEV=/XWINDOW \
    SIOSEIS=/usr/local/sioseis/sioseis-2024.1.1 \
    DISPLAY=host.docker.internal:0.0

# Extend PATH to include all required directories
ENV PATH=$PATH:/usr/local/pgplot:/usr/local/sioseis/sioseis-2024.1.1:$CWPROOT/bin:$CWPROOT/src/Sfio/bin:$SeismicUnixGui_script

# Set LD_LIBRARY_PATH including PGPLOT directory
# TODO: not sure if this section here is actually needed
RUN echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/usr/local/pgplot/" >> /etc/profile

# Optional final downloads from SeismicUnix download/config docs - not all these download successfully
RUN cd /usr/local/cwp_su_all_44R22/src \
        && make xtinstall || true \
        && make xminstall || true \
        && make mglinstall || true \
        && make finstall || true \
        && make sfinstall || true

# Testing seoseis package...
RUN mkdir -p /usr/local/sioseis
COPY sioseis-2024.1.1 /usr/local/sioseis/sioseis-2024.1.1/

# Run MAKE on the sioseis package
RUN cd /usr/local/sioseis/sioseis-2024.1.1 \
        && make all