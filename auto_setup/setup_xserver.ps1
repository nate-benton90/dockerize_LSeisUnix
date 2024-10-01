# Define VcXsrv launch arguments
$vcxsrvArgs = ":0 -multiwindow -clipboard -wgl"

# Function to install Chocolatey
function Install-Choco {
    Write-Host "Checking if Chocolatey is installed..."
    $chocoPath = (Get-Command choco.exe -ErrorAction SilentlyContinue).Path
    if (-not $chocoPath) {
        Write-Host "Chocolatey not found. Installing Chocolatey..."
        Set-ExecutionPolicy Bypass -Scope Process -Force;
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072;
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        Write-Host "Chocolatey installed successfully."
    } else {
        Write-Host "Chocolatey is already installed."
    }
}

# Function to install VcXsrv using Chocolatey
function Install-VcXsrv {
    Write-Host "Installing VcXsrv using Chocolatey..."
    choco install vcxsrv --force -y
}

# Function to start VcXsrv with specified arguments
function Start-VcXsrv {
    Write-Host "Starting VcXsrv..."
    $vcxsrvPath = "C:\Program Files\VcXsrv\vcxsrv.exe"
    if (Test-Path $vcxsrvPath) {
        Start-Process -FilePath $vcxsrvPath -ArgumentList $vcxsrvArgs
        Write-Host "VcXsrv started with arguments: $vcxsrvArgs"
    } else {
        Write-Host "VcXsrv executable not found at $vcxsrvPath."
    }
}

# Main script execution
Install-Choco
Install-VcXsrv
Start-VcXsrv
