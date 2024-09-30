# Unregister the 'Ubuntu-20.04' WSL distribution
wsl --unregister "Ubuntu-20.04"

# Wait for a few seconds to ensure the distribution is fully unregistered
Start-Sleep -Seconds 5

# Uninstall the Ubuntu-20.04 app from the system via PowerShell
Get-AppxPackage *Ubuntu* | Where-Object {$_.Name -like "*Ubuntu*20.04*"} | Remove-AppxPackage

Write-Host "Ubuntu-20.04 has been unregistered and the app has been removed."
