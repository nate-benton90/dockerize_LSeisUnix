All preliminary, ongoing, and future work based strictly on
initial dev provided by Dr. Juan Lorenzo (at LSU), which his
work is here: https://github.com/gllore

1) `& 'C:\Program Files\VcXsrv\vcxsrv.exe' :0 -ac` = use this command to start Xserver 
   graphics UI for Windos OS (it's advised to use the latest Windows Terminal with Powershell, so NOT CMD or something else like it).
2) Run this command to start SUG: `SeismicUnixGui`.
3) Samples [here](https://www.geol.lsu.edu/jlorenzo/ExplorationAndEnvironmentalGeophysicsGeol4062/labs/SeismicUnixGui%20Tutorial_0.80.1.pdf).
4) Use Choco (for Windows users) to install stuff for #1 above: `choco install vcxsrv`.
5) For macOS users, use XQuartz instead of VcXsrv and install that via Homebrew: `brew cask install xquartz` (and in case one does not have brew, install it via: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`.
6) For Linux users, use X11 and install it via: `sudo apt-get install x11-apps`.
7) To acquire the Docker image, run this: `docker pull nathanbenton/sug`.
8) To run the container with file sharing enabled and interacting with it directly, run this command (which is similar to the initial one, which is docker `run -it nathanbenton/sug`): `docker run --mount type=bind,source=C:\Users\nbent\dev_testing\k8s\youtube\videos,target=/usr/local/cwp_su_all_44R22/data -it  nathanbenton/sug` (see email to Dr. Lorenzo for more details).
9) 
