$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$AppName = "my-lvgl-app"
$Version = $env:GITHUB_REF_NAME.TrimStart('v')
$Executable = Join-Path $ProjectRoot "build/release/Release/$AppName.exe"
$DistDirectory = Join-Path $ProjectRoot "dist"
$InstallerScript = Join-Path $ProjectRoot "installer.iss"

Set-Location $ProjectRoot

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message"
}

function Initialize-DistDirectory {
    Write-Step "Preparing Windows packaging directory"
    New-Item -ItemType Directory -Force -Path $DistDirectory | Out-Null
}

function New-InnoSetupScript {
    Write-Step "Generating Inno Setup script"

    $iss = @"
#define MyAppVersion "$Version"
[Setup]
AppId={{B19E6B4B-91AD-4B0C-9A34-0F55A6E0B5D6}
AppName=$AppName
AppVersion={#MyAppVersion}
DefaultDirName={autopf}my-lvgl-app
DisableProgramGroupPage=yes
OutputDir=dist
OutputBaseFilename=my-lvgl-app-$env:GITHUB_REF_NAME-windows-x86_64-installer
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
[Files]
Source: "$Executable"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{autoprograms}my-lvgl-app"; Filename: "{app}my-lvgl-app.exe"
Name: "{autodesktop}my-lvgl-app"; Filename: "{app}my-lvgl-app.exe"
[Run]
Filename: "{app}my-lvgl-app.exe"; Description: "Launch my-lvgl-app"; Flags: nowait postinstall skipifsilent
"@

    Set-Content -Path $InstallerScript -Value $iss -Encoding UTF8
}

function Invoke-InnoSetup {
    Write-Step "Building Windows installer"

    $compiler = $null

    $command = Get-Command ISCC.exe -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        $compiler = $command.Source
    }

    if ($null -eq $compiler) {
        $programFilesX86 = ${env:ProgramFiles(x86)}
        $defaultCompiler = Join-Path $programFilesX86 "Inno Setup 6ISCC.exe"

        if (Test-Path $defaultCompiler) {
            $compiler = $defaultCompiler
        }
    }

    if ($null -eq $compiler) {
        throw "Inno Setup compiler not found. Expected ISCC.exe on PATH or at '${env:ProgramFiles(x86)}Inno Setup 6ISCC.exe'"
    }

    Write-Host "Using Inno Setup compiler: $compiler"
    & $compiler $InstallerScript

    if ($LASTEXITCODE -ne 0) {
        throw "Inno Setup compiler failed with exit code $LASTEXITCODE"
    }
}

function Test-Installer {
    Write-Step "Verifying Windows installer"

    $installer = Get-ChildItem -Path $DistDirectory -Filter "*-installer.exe" -File |
        Select-Object -First 1

    if ($null -eq $installer -or $installer.Length -eq 0) {
        throw "Windows installer was not created in $DistDirectory"
    }

    Write-Host "Installer: $($installer.FullName)"
}

function Main {
    Initialize-DistDirectory
    New-InnoSetupScript
    Invoke-InnoSetup
    Test-Installer
    Write-Step "Created Windows installer in $DistDirectory"
}

Main
