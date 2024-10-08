# Function to check if WSL is installed and upgrade it if necessary
function Install-WSL {
    Write-Host "Checking if WSL is installed..."
    try {
        # Check if WSL is installed
        $wslOutput = wsl --list --verbose 2>&1
        if ($wslOutput -like "*WSL 2 requires an update to its kernel component*") {
            Write-Host "WSL is installed, but the kernel update is required. Updating kernel..."
            wsl --update
            Write-Host "WSL kernel has been updated."
        } elseif ($wslOutput -like "*No installed distributions*") {
            Write-Host "WSL is installed but no distributions are available. No further actions required."
        } else {
            Write-Host "WSL is installed and operational."
        }
    } catch {
        Write-Host "WSL is not installed. Installing WSL..."
        wsl --install
        Write-Host "WSL has been installed. Please restart your machine if required."
    }
}

# Function to set WSL 2 as the default version
function Set-DefaultWSL2 {
    Write-Host "Setting WSL 2 as the default version (if not already set)..."
    try {
        wsl --set-default-version 2
        Write-Host "WSL 2 is now the default version."
    } catch {
        Write-Host "Failed to set WSL 2 as the default version. Make sure your system supports WSL 2."
    }
}

# Main Script Execution
Install-WSL
Set-DefaultWSL2
