#!/bin/bash 
# my name is clone.sh 

# give a name to directory 
installation_directory_for_SeismicUnixGui=/usr/local/pl/ 

# create installation directory (use -p to avoid error if it exists)
mkdir -p $installation_directory_for_SeismicUnixGui 

# change into the installation directory 
cd $installation_directory_for_SeismicUnixGui 

# clone the directory from the remote site on to your computer 
git -c http.lowSpeedLimit=1000 -c http.lowSpeedTime=60 clone https://github.com/gllore/SeismicUnixGui.git 

# change into the cloned directory to run git status
cd SeismicUnixGui

# git status 
git status
