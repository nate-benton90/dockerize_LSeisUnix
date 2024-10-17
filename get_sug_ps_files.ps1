# Set the base URL for the raw GitHub files
$baseUrl = "https://raw.githubusercontent.com/nate-benton90/dockerize_LSeisUnix/feature/dockerfile_stuff/auto_setup"

# Specify the files you want to download
$files = @(
    "check_wsl.ps1",
    "check_wsl_vm.ps1",
    "docker_setup.ps1",
    "enable_wsl_vm.ps1",
    "remove_docker.ps1",
    "setup_xserver.ps1"
)

# Get the current user's Downloads folder and create 'sug_ps_files' directory
$downloadsFolder = [System.IO.Path]::Combine([Environment]::GetFolderPath("UserProfile"), "Downloads")
$destinationDirectory = [System.IO.Path]::Combine($downloadsFolder, "sug_ps_files")

# Create the destination directory if it doesn't exist
if (-not (Test-Path $destinationDirectory)) {
    New-Item -Path $destinationDirectory -ItemType Directory | Out-Null
}

# Loop through each file and download it
foreach ($file in $files) {
    $url = "$baseUrl/$file"
    $outputFile = Join-Path $destinationDirectory $file

    try {
        Write-Host "Downloading $file..."
        Invoke-WebRequest -Uri $url -OutFile $outputFile
        Write-Host "$file downloaded successfully."
    } catch {
        Write-Host "Failed to download $file. Error: $_"
    }
}

Write-Host "Download process complete."
