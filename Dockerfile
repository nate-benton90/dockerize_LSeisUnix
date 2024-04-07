# Use a build stage for cloning the repo with SSH access
FROM ubuntu:22.04

# Avoid prompts from apt and set CPAN to non-interactive mode
ENV DEBIAN_FRONTEND=noninteractive \
    PERL_MM_USE_DEFAULT=1

# Force rebuild for copy layer below
RUN echo "foo"

# Copy the SeisUnix-master directory contents into the image
COPY SeisUnix-master/* /usr/local/cwp_su_all_44R22

# Clone the repository using SSH
RUN cd /usr/local/cwp_su_all_44R22

# Set the environment variables as specified
ENV LOCAL=/usr/local \
    CWPROOT=/usr/local/cwp_su_all_44R22 \
    SeismicUnixGui=/usr/local/share/perl/5.34.0/App/SeismicUnixGui \
    SeismicUnixGui_script=/usr/local/share/perl/5.34.0/App/SeismicUnixGui/script \
    PGPLOT_DIR=/usr/local/pgplot \
    PGPLOT_DEV=/XWINDOW \
    SIOSEIS=/usr/local/sioseis \
    PATH=$PATH:/$CWPROOT/bin:$CWPROOT/src/Sfio/bin:$SeismicUnixGui_script:$SIOSEIS \
    PERL5LIB=$PERL5LIB:$SeismicUnixGui 

# Update and install required packages including development tools, X11/Tcl-Tk libraries, and others as specified
# Also adding the newly required packages
RUN apt-get update && apt-get install -y \
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

# Export the display for UI to work
RUN export DISPLAY=host.docker.internal:0.0

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

# Execute commands without failing the build if one fails
RUN yes | make install || true \
    && make xtinstall || true \
    && make xminstall || true \
    && make mglinstall || true \
    && make finstall || true \
    && make sfinstall || true
