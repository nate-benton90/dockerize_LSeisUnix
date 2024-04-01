# Use the specific Ubuntu version as the base
FROM ubuntu:22.04

# Avoid prompts from apt and set CPAN to non-interactive mode
ENV DEBIAN_FRONTEND=noninteractive \
    PERL_MM_USE_DEFAULT=1

# Set the environment variables as specified
ENV LOCAL=/usr/local \
    CWPROOT=/usr/local/cwp_su_all_44R22 \
    SeismicUnixGui=/usr/local/share/perl/5.34.0/App/SeismicUnixGui \
    SeismicUnixGui_script=/usr/local/share/perl/5.34.0/App/SeismicUnixGui/script \
    PGPLOT_DIR=/usr/local/pgplot \
    PGPLOT_DEV=/XWINDOW \
    SIOSEIS=/usr/local/sioseis

ENV PATH=$PATH:$CWPROOT/bin:$CWPROOT/src/Sfio/bin:$SeismicUnixGui_script:$SIOSEIS \
    PERL5LIB=$PERL5LIB:$SeismicUnixGui

# Update and install common development tools, Perl, Tk dependencies, and X11/Tcl-Tk development libraries
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    vim \
    curl \
    wget \
    sudo \
    lsb-release \
    software-properties-common \
    perl \
    tk \
    libtk-img \
    libx11-dev \
    libxft-dev \
    tcl-dev \
    tk-dev \
    && rm -rf /var/lib/apt/lists/*

# Install cpanminus for easier module installation
RUN cpan App::cpanminus

# Install Perl modules
RUN cpanm Tk Tk::JFileDialog Tk::Pod
