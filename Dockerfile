# Use a build stage for cloning the repo with SSH access
FROM ubuntu:22.04

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
RUN apt-get update && apt-get install --fix-missing -y \
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

# # Install cpanminus for easier module installation
# RUN cpan App::cpanminus \
#         && cpanm Tk Tk::JFileDialog Tk::Pod

# # Install last 2 packages from DL's docs for CPAN setup
# RUN cpan Module::Build \
#         && cpan TAP::Harness \
#         && cpan Moose

# Install cpanminus and the required Perl modules
RUN cpan App::cpanminus \
    && cpanm \
        Module::Refresh \
        Moose \
        Clone \
        File::ShareDir \
        File::Slurp \
        Shell \
        Test::Compile::Internal \
        Time::HiRes \
        Tk \
        Tk::JFileDialog \
        Tk::Pod \
        aliased \
        namespace::autoclean \
    && cpanm MIME::Base64 \
    && cpanm PDL::Core

# Set LD_LIBRARY_PATH including PGPLOT directory early in the file
ENV LD_LIBRARY_PATH=/usr/local/pgplot:$LD_LIBRARY_PATH
ENV LOCAL=/usr/local
ENV PL=$LOCAL/pl 
ENV APP_LIB=$PL/SeismicUnixGui/lib 
ENV CWPROOT=/usr/local/cwp_su_all_44R22 
ENV SeismicUnixGui=/usr/local/pl/SeismicUnixGui/lib/App/SeismicUnixGui 
ENV SeismicUnixGui_script=$SeismicUnixGui/script 
ENV PGPLOT_DIR=/usr/local/pgplot 
ENV PGPLOT_DEV=/XWINDOW 
ENV SIOSEIS=/usr/local/sioseis/sioseis-2024.1.1 
ENV APP_LIB=$PL/SeismicUnixGui/lib 
ENV PERL5LIB=$PERL5LIB:$APP_LIB 
ENV DISPLAY=host.docker.internal:0.0

# Extend PATH to include all required directories
ENV PATH=$PATH:/usr/local/pgplot:/usr/local/sioseis/sioseis-2024.1.1:$CWPROOT/bin:$CWPROOT/src/Sfio/bin:$SeismicUnixGui_script:$SeismicUnixGui/fortran/bin:$SeismicUnixGui/c/bin

# Add your expect script and other necessary files
COPY install_cwp.exp /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Setup this to avoid interactive prompts
RUN apt-get update && apt-get install -y expect

# Fix the line endings for the install_cwp.exp script
RUN apt-get install dos2unix && dos2unix /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Adjust permissions and execute the script as needed
RUN chmod +x /usr/local/cwp_su_all_44R22/src/install_cwp.exp
RUN /usr/local/cwp_su_all_44R22/src/install_cwp.exp

# Execute the expect script for installing CWP and the prompt for the user to accept the license
RUN cd /usr/local/cwp_su_all_44R22/src \
        && ./install_cwp.exp

# Execute commands without failing the build if one fails
RUN cd /usr/local/cwp_su_all_44R22/src \
        && make install || true \
        && make xtinstall || true \
        && make xminstall || true \
        && make mglinstall || true \
        && make finstall || true \
        && make sfinstall || true

# Testing seoseis package
RUN mkdir -p /usr/local/sioseis
COPY sioseis-2024.1.1 /usr/local/sioseis/sioseis-2024.1.1/

# Run MAKE on the sioseis package
RUN cd /usr/local/sioseis/sioseis-2024.1.1 \
        && make all

# Optional: Extract the tar file inside the image (if needed)
# Ensure the data directory exists
RUN mkdir -p /usr/local/data \
        && mkdir -p /home/sug_user

# Copy the large zipped tar file from the build context to the image
COPY data/Servilleta.tz /usr/local/data/Servilleta.tz

# Untar/decrompress the tar file
RUN tar -xzf /usr/local/data/Servilleta.tz -C /home/sug_user

# Create non-admin user
RUN groupadd -r sug_ug || true \
        && useradd -r -g sug_ug -m -s /bin/bash sug_user

# Set the correct home directory
ENV HOME=/home/sug_user

# Ensure sug_user has ownership of their home directory
RUN chown -R sug_user:sug_ug /home/sug_user

# Create a symbolic link from /home/username to /home/sug_user
RUN ln -s /home/sug_user /home/username

# Final lazy setup for image config for PWD at startup
WORKDIR /home/sug_user

# Create the directory inside the container (if needed)
RUN mkdir -p /home/sug_user/sug_data

# Copy the clone.sh script into the container
COPY seismic_unix_gui.sh /usr/local/pl/clone.sh

# Set execution permissions for the script
RUN chmod +x /usr/local/pl/clone.sh

# Run the clone.sh script as the sug_user user
RUN /usr/local/pl/clone.sh

RUN cd /usr/local/pl/SeismicUnixGui \
        && cpan aliased

# Set the WORKDIR back to sug_user's home
WORKDIR /home/sug_user

# Run as non-admin user
USER sug_user
