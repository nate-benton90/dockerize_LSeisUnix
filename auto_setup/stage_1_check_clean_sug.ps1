# Step 1: Get list of all WSL distributions
$wslDistros = wsl -l -q

# Step 2: Unregister each WSL distribution if any are found
if ($wslDistros) {
    foreach ($distro in $wslDistros) {
        Write-Host "Unregistering distribution: $distro"
        wsl --unregister $distro
    }
} else {
    Write-Host "No WSL distributions found."
}

# Step 3: Disable WSL and related features
Write-Host "Disabling WSL and related features..."

# Disable WSL
dism.exe /online /disable-feature /featurename:Microsoft-Windows-Subsystem-Linux /norestart

# Disable Virtual Machine Platform
dism.exe /online /disable-feature /featurename:VirtualMachinePlatform /norestart

# Disable Hypervisor Platform
dism.exe /online /disable-feature /featurename:HypervisorPlatform /norestart

Write-Host "WSL and related features have been disabled. Please restart your computer to apply changes."

# Step 4: Optionally Restart the machine
Restart-Computer