# Stop Docker Desktop if running
Write-Host "Stopping Docker Desktop..."
$dockerServiceStatus = Get-Process -Name "Docker Desktop" -ErrorAction SilentlyContinue
if ($dockerServiceStatus) {
    Stop-Process -Name "Docker Desktop" -Force
    Start-Sleep -Seconds 5
}

# Uninstall Docker Desktop using the official uninstaller (if installed for all users)
Write-Host "Uninstalling Docker Desktop..."
$uninstallPath = "C:\Program Files\Docker\Docker\Docker Desktop Installer.exe"
if (Test-Path $uninstallPath) {
    Start-Process -FilePath $uninstallPath -ArgumentList "uninstall", "--quiet", "--all-users" -Wait -NoNewWindow
} else {
    Write-Host "Docker Desktop is not installed or uninstaller not found."
}

# Clean up Docker directories
Write-Host "Removing Docker related files and directories..."

# Remove Docker Program Files
$dockerProgramFilesPath = "C:\Program Files\Docker"
if (Test-Path $dockerProgramFilesPath) {
    Remove-Item -Recurse -Force -Path $dockerProgramFilesPath
    Write-Host "Removed Docker files from Program Files."
}

# Remove Docker Desktop data
$dockerDataPath = "$env:APPDATA\Docker"
if (Test-Path $dockerDataPath) {
    Remove-Item -Recurse -Force -Path $dockerDataPath
    Write-Host "Removed Docker data from AppData."
}

# Remove Docker configuration in .docker folder
$dockerConfigPath = "$env:USERPROFILE\.docker"
if (Test-Path $dockerConfigPath) {
    Remove-Item -Recurse -Force -Path $dockerConfigPath
    Write-Host "Removed Docker configuration from .docker folder."
}

# Remove Docker binaries in Windows path (CLI tools)
$dockerCliPath = "C:\ProgramData\DockerDesktop"
if (Test-Path $dockerCliPath) {
    Remove-Item -Recurse -Force -Path $dockerCliPath
    Write-Host "Removed Docker binaries from ProgramData."
}

# Remove Docker Service (if still present)
Write-Host "Removing Docker service (if present)..."
$dockerService = Get-Service -Name "com.docker.service" -ErrorAction SilentlyContinue
if ($dockerService) {
    Stop-Service -Name "com.docker.service" -Force
    Remove-Service -Name "com.docker.service"
    Write-Host "Docker service removed."
}

# Clean environment variables and system paths
Write-Host "Cleaning environment variables..."
[System.Environment]::SetEnvironmentVariable("DOCKER_CERT_PATH", $null, [System.EnvironmentVariableTarget]::Machine)
[System.Environment]::SetEnvironmentVariable("DOCKER_TLS_VERIFY", $null, [System.EnvironmentVariableTarget]::Machine)
[System.Environment]::SetEnvironmentVariable("DOCKER_HOST", $null, [System.EnvironmentVariableTarget]::Machine)
[System.Environment]::SetEnvironmentVariable("DOCKER_TOOLBOX_INSTALL_PATH", $null, [System.EnvironmentVariableTarget]::Machine)

# Remove Docker from PATH (optional, remove if path includes Docker binaries)
Write-Host "Removing Docker from system PATH..."
$envPath = [System.Environment]::GetEnvironmentVariable("Path", [System.EnvironmentVariableTarget]::Machine)
$updatedPath = $envPath -replace [regex]::Escape("C:\Program Files\Docker\Docker\resources\bin"), ""
[System.Environment]::SetEnvironmentVariable("Path", $updatedPath, [System.EnvironmentVariableTarget]::Machine)

Write-Host "Docker completely uninstalled."
