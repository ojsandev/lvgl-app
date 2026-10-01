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
DefaultDirName={autopf}\my-lvgl-app
DisableProgramGroupPage=yes
OutputDir=dist
OutputBaseFilename=my-lvgl-app-$env:GITHUB_REF_NAME-windows-x86_64-installer
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
[Files]
Source: "$Executable"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{autoprograms}\my-lvgl-app"; Filename: "{app}\my-lvgl-app.exe"
Name: "{autodesktop}\my-lvgl-app"; Filename: "{app}\my-lvgl-app.exe"
[Run]
Filename: "{app}\my-lvgl-app.exe"; Description: "Launch my-lvgl-app"; Flags: nowait postinstall skipifsilent
"@

    Set-Content -Path $InstallerScript -Value $iss -Encoding UTF8
}

function Invoke-InnoSetup {
    Write-Step "Building Windows installer"
    $compiler = "$env:ProgramFiles(x86)\Inno Setup 6\ISCC.exe"

    if (-not (Test-Path $compiler)) {
        throw "Inno Setup compiler not found: $compiler"
    }

    & $compiler $InstallerScript
}

function Main {
    Initialize-DistDirectory
    New-InnoSetupScript
    Invoke-InnoSetup
    Write-Step "Created Windows installer in $DistDirectory"
}

Main
