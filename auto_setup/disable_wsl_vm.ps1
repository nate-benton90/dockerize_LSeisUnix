# Step 1: Disable WSL and related features
Write-Host "Disabling WSL and related features..."

# Disable WSL
dism.exe /online /disable-feature /featurename:Microsoft-Windows-Subsystem-Linux /norestart

# Disable Virtual Machine Platform
dism.exe /online /disable-feature /featurename:VirtualMachinePlatform /norestart

# Disable Hypervisor Platform
dism.exe /online /disable-feature /featurename:HypervisorPlatform /norestart

Write-Host "WSL and related features have been disabled. Please restart your computer to apply changes."

# Step 2: (Required) Restart the machine
Restart-Computer