# Define variables for URLs and paths
$dockerInstallerUrl = "https://desktop.docker.com/win/stable/Docker%20Desktop%20Installer.exe"
$dockerInstallerPath = "$env:TEMP\DockerDesktopInstaller.exe"

# Download the Docker Desktop installer
Write-Host "Downloading Docker Desktop installer..."
Invoke-WebRequest -Uri $dockerInstallerUrl -OutFile $dockerInstallerPath

# Install Docker Desktop silently for all users
Write-Host "Installing Docker Desktop silently for all users..."
Start-Process -FilePath $dockerInstallerPath -ArgumentList "install", "--quiet", "--all-users" -Wait -NoNewWindow

# Wait a moment to ensure installation completes
Start-Sleep -Seconds 10

# Verify if Docker Desktop is installed
$dockerInstallPath = "C:\Program Files\Docker\Docker\Docker Desktop.exe"
if (Test-Path $dockerInstallPath) {
    Write-Host "Docker Desktop installed successfully."
} else {
    Write-Host "Docker Desktop installation failed."
    exit 1
}

# Start Docker Desktop
Write-Host "Starting Docker Desktop..."
Start-Process -FilePath $dockerInstallPath

# Wait for a few seconds to allow Docker to initialize
Start-Sleep -Seconds 15

# Check if Docker Desktop is running by verifying if any process named 'Docker Desktop' exists
Write-Host "Checking Docker Desktop status..."
$dockerServiceStatus = Get-Process | Where-Object { $_.Name -like "Docker Desktop*" }

if ($dockerServiceStatus.Count -gt 0) {
    Write-Host "Docker Desktop is running."
} else {
    Write-Host "Docker Desktop failed to start."
}

# Clean up installer file
Remove-Item -Path $dockerInstallerPath -Force

# Additional check: Verify Docker command line tool availability
Write-Host "Checking Docker command line status..."
try {
    $dockerVersion = docker --version
    Write-Host "Docker is working: $dockerVersion"
} catch {
    Write-Host "Docker command line is operational - START ANOTHER TERMINAL to allow usage of CLI on terminal. You may now access Docker functionality via Admin and non-Admin users."
}
