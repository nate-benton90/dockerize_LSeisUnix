# Wait for the user to install Ubuntu manually
Read-Host -Prompt "Please install Ubuntu 20.04 from the Microsoft Store and press Enter when done."

# Run the WSL install to complete the setup
wsl --install -d "Ubuntu-20.04"
