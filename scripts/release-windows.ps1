$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$AppName = "my-lvgl-app"
$ReleaseName = if ($env:VERSION) { $env:VERSION } elseif ($env:GITHUB_REF_NAME) { $env:GITHUB_REF_NAME } else { "dev" }
$Version = $ReleaseName.TrimStart('v')
$Executable = Join-Path $ProjectRoot "build/release/Release/$AppName.exe"
$DistDirectory = Join-Path $ProjectRoot "dist"
$InstallerScript = Join-Path $ProjectRoot "installer.iss"
$InstallerName = "$AppName-$ReleaseName-windows-x86_64-installer.exe"
$Installer = Join-Path $DistDirectory $InstallerName

Set-Location $ProjectRoot

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message"
}

function Initialize-DistDirectory {
    Write-Step "Preparing Windows packaging directory"
    New-Item -ItemType Directory -Force -Path $DistDirectory | Out-Null
    Remove-Item -Force -ErrorAction SilentlyContinue $Installer
}

function Test-Prerequisites {
    Write-Step "Checking Windows packaging prerequisites"

    if (-not (Test-Path -Path $Executable -PathType Leaf)) {
        throw "Release executable not found: $Executable"
    }
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
OutputBaseFilename=$([System.IO.Path]::GetFileNameWithoutExtension($InstallerName))
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

    $compiler = $null

    $command = Get-Command ISCC.exe -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        $compiler = $command.Source
    }

    if ($null -eq $compiler) {
        $programFilesX86 = ${env:ProgramFiles(x86)}
        $defaultCompiler = Join-Path $programFilesX86 "Inno Setup 6\ISCC.exe"

        if (Test-Path $defaultCompiler) {
            $compiler = $defaultCompiler
        }
    }

    if ($null -eq $compiler) {
        throw "Inno Setup compiler not found. Expected ISCC.exe on PATH or at '${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe'"
    }

    Write-Host "Using Inno Setup compiler: $compiler"
    & $compiler $InstallerScript

    if ($LASTEXITCODE -ne 0) {
        throw "Inno Setup compiler failed with exit code $LASTEXITCODE"
    }
}

function Test-Installer {
    Write-Step "Verifying Windows installer"

    if (-not (Test-Path -Path $Installer -PathType Leaf) -or (Get-Item $Installer).Length -eq 0) {
        throw "Windows installer was not created: $Installer"
    }

    Write-Host "Installer: $Installer"
}

function Main {
    Initialize-DistDirectory
    Test-Prerequisites
    New-InnoSetupScript
    Invoke-InnoSetup
    Test-Installer
    Write-Step "Created Windows installer in $DistDirectory"
}

Main
