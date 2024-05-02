# Use a build stage for cloning the repo with SSH access
FROM ubuntu:22.04 as builder

# Avoid prompts from apt and set CPAN to non-interactive mode
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
    && rm -rf /var/lib/apt/lists/*

# Set LD_LIBRARY_PATH including PGPLOT directory
RUN echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/usr/local/pgplot/" >> /etc/profile

# Install cpanminus for easier module installation
RUN cpan App::cpanminus

# Install Perl modules
RUN cpanm Tk Tk::JFileDialog Tk::Pod

# Install last 2 packages from DL's docs for CPAN setup
RUN cpan Module::Build
RUN cpan TAP::Harness
RUN cpan Moose

# Force install App::SeismicUnixGui without running tests
RUN cpanm --notest App::SeismicUnixGui

# Setup this ridiculous thing to avoid interactive prompts
RUN apt-get update \
        && apt-get install -y expect

# Add your expect script and other necessary files
COPY install_cwp.exp /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Adjust permissions and execute the script as needed
RUN chmod +x /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Fix the line endings for the install_cwp.exp script
RUN apt-get update && apt-get install -y dos2unix \
        && dos2unix /usr/local/cwp_su_all_44R22/src/install_cwp.exp \
        && rm -rf /var/lib/apt/lists/*

# Placed here to avoid this error during container run: libpgplot.so: 
# cannot open shared object file: No such file or directory
ENV LD_LIBRARY_PATH=/usr/local/pgplot:$LD_LIBRARY_PATH
ENV PATH=/usr/local/pgplot:$PATH
ENV CWPROOT=/usr/local/cwp_su_all_44R22

# Set other environment variables
ENV LOCAL=/usr/local \
    SeismicUnixGui=/usr/local/share/perl/5.34.0/App/SeismicUnixGui \
    SeismicUnixGui_script=/usr/local/share/perl/5.34.0/App/SeismicUnixGui/script \
    PGPLOT_DIR=/usr/local/pgplot \
    PGPLOT_DEV=/XWINDOW \
    SIOSEIS=/usr/local/sioseis

# Required to project graphics to the host machine
ENV DISPLAY=host.docker.internal:0.0

# Now append to PATH
ENV PATH="${PATH}:${CWPROOT}/bin:${CWPROOT}/src/Sfio/bin:${SeismicUnixGui_script}:${SIOSEIS}"

# ---
RUN cd /usr/local/cwp_su_all_44R22/src \
        && make xtinstall || true \
        && make xminstall || true \
        && make mglinstall || true \
        && make finstall || true \
        && make sfinstall || true
