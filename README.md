# SeismicUnix GUI (SUG) "Dockerization" Project


Welcome - before reading anything else below, note that the main functionality
of this project is based on Dr. Juan Lorenzo's work at LSU (more info
here: https://www.lsu.edu/science/geology/people/faculty/lorenzo.php). 
The Dockerizing portion, graphics rending (*NOT GUI*), versioning,
docs herein and some other presumably "nice stuff" is added by Nathan
Benton (info here: https://subsurfacesee.org/)

![Main intro image](images\lsu_thing.png)

## Section 1: To get started or continue with this project...
1) Install Docker Desktop (or equivalent) on your machine (if this is your first 
   time using Docker).
2) Clone this repo to your local machine (if this is your first time using this project).
3) Acquire or update the Docker image by running this command: `docker pull nathanbenton/sug`.
4) Start the graphics rendering software with (for Windows users): `& 'C:\Program Files\VcXsrv\vcxsrv.exe' :0 -ac`. If you haven't installed that yet (via choco - see further note), refer to the Appendix below by clicking here: [Appendix](#appendix-dev-notes-if-you-are-a-basic-user-ignore-this-section-below).
5) Run Docker container with basic, non-file sharing command: `docker run -it nathanbenton/sug`.
6) If you want to enable file sharing, run this command: `docker run --mount type=bind,source=C:\Users\nbent\dev_testing\k8s\youtube\videos,target=/usr/local/cwp_su_all_44R22/data -it  nathanbenton/sug` (see email to Dr. Lorenzo for more details in regards to changing the source file path example here).

### Appendix: DEV Notes (if you are a basic user, ignore this section below)...
* All preliminary, ongoing, and future work based strictly on
initial dev provided by Dr. Juan Lorenzo (at LSU), which his
work is here: https://github.com/gllore. Right now, these notes
are not listed in order, and are here mostly for reference. *Read
everything here* in this README file **before** doing anything else in
this project.

1) `& 'C:\Program Files\VcXsrv\vcxsrv.exe' :0 -ac` = use this command to start Xserver 
   graphics UI for Windos OS (it's advised to use the latest Windows Terminal with Powershell, so NOT CMD or something else like it).
2) Run this command to start SUG: `SeismicUnixGui` (inside the running container, which you can provision with #8 below).
3) Samples [here](https://www.geol.lsu.edu/jlorenzo/ExplorationAndEnvironmentalGeophysicsGeol4062/labs/SeismicUnixGui%20Tutorial_0.80.1.pdf).
4) Use Choco (for Windows users) to install stuff for #1 above: `choco install vcxsrv`.
5) For macOS users, use XQuartz instead of VcXsrv and install that via Homebrew: `brew cask install xquartz` (and in case one does not have brew, install it via: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`.
6) For Linux users, use X11 and install it via: `sudo apt-get install x11-apps`.
7) To acquire the Docker image, run this: `docker pull nathanbenton/sug`.
8) To run the container with file sharing enabled and interacting with it directly, run this command (which is similar to the initial one, which is docker `run -it nathanbenton/sug`): `docker run --mount type=bind,source=C:\Users\nbent\dev_testing\k8s\youtube\videos,target=/usr/local/cwp_su_all_44R22/data -it  nathanbenton/sug` (see email to Dr. Lorenzo for more details).
9) Note that this file is to automatically handle all the initial prompts thrown from Seismic Unix during the install/config: `./install_cwp.exp`.
10) In order to see any graphics from the container, make sure you have Xserver (or equivalent software for other non-Windos OS) running.
11) This is the unzipped SU download (i.e. **SeisUnix-master**) from here: https://github.com/JohnWStockwellJr/SeisUnix.
12) Also, this is the unzipped folder download (i.e. **sioseis-2024.1.1**) from here: https://sioseis.com/index.html.

