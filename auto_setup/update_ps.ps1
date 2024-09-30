# Check for existing PowerShell version
$currentVersion = $PSVersionTable.PSVersion
$requiredVersion = [version]"7.0.0"

# Check if the current version is older than the required version
if ($currentVersion -lt $requiredVersion) {
    Write-Output "Current PowerShell version is $currentVersion. Updating to PowerShell 7..."

    # Try to use winget to install or upgrade PowerShell
    try {
        # First, check if winget is available
        if (Get-Command winget -ErrorAction SilentlyContinue) {
            # Install PowerShell 7 with winget
            winget install --id Microsoft.Powershell --source winget --silent
        } else {
            Write-Output "winget is not installed. Please install PowerShell manually."
            exit
        }
    } catch {
        Write-Error "An error occurred during PowerShell 7 installation: $_"
        exit
    }
} else {
    Write-Output "PowerShell is already up-to-date. Current version: $currentVersion"
}
