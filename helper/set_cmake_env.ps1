# Get the user's AppData directory
$UserAppData = [System.Environment]::GetFolderPath("LocalApplicationData")
$DefaultInstallPrefix = "$UserAppData\CMakeInstall"

# Function to set environment variables permanently
function Set-PersistentEnvVar {
    param (
        [string]$name,
        [string]$value
    )

    # Set for the current session
    Set-Item -Path "Env:$name" -Value $value

    # Set permanently for the user
    [System.Environment]::SetEnvironmentVariable($name, $value, [System.EnvironmentVariableTarget]::User)

    Write-Output "$name set to: $value (persistent)"
}

# Check and set CMAKE_INSTALL_PREFIX
if (-not $env:CMAKE_INSTALL_PREFIX) {
    Set-PersistentEnvVar "CMAKE_INSTALL_PREFIX" $DefaultInstallPrefix
} else {
    Write-Output "CMAKE_INSTALL_PREFIX is already set to: $env:CMAKE_INSTALL_PREFIX"
}

# Check and set CMAKE_INSTALL_LIBDIR
if (-not $env:CMAKE_INSTALL_LIBDIR) {
    Set-PersistentEnvVar "CMAKE_INSTALL_LIBDIR" "lib"
} else {
    Write-Output "CMAKE_INSTALL_LIBDIR is already set to: $env:CMAKE_INSTALL_LIBDIR"
}
