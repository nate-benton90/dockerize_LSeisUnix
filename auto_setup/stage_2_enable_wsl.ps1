# Step 1: Enable WSL2 and required features
Write-Host "Enabling WSL2 and required features..."

# Enable WSL
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart

# Enable Virtual Machine Platform
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

# Enable Hypervisor Platform
dism.exe /online /enable-feature /featurename:HypervisorPlatform /all /norestart

# Restart to apply WSL2 installation changes
Write-Host "Restarting to apply changes..."
Restart-Computer
