$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $projectRoot
$version = $env:GITHUB_REF_NAME.TrimStart('v')
$exe = "build/release/Release/my-lvgl-app.exe"
New-Item -ItemType Directory -Force -Path dist | Out-Null
$iss = @"
#define MyAppVersion "$version"
[Setup]
AppId={{B19E6B4B-91AD-4B0C-9A34-0F55A6E0B5D6}
AppName=my-lvgl-app
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\my-lvgl-app
DisableProgramGroupPage=yes
OutputDir=dist
OutputBaseFilename=my-lvgl-app-$env:GITHUB_REF_NAME-windows-x86_64-installer
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
[Files]
Source: "$exe"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{autoprograms}\my-lvgl-app"; Filename: "{app}\my-lvgl-app.exe"
Name: "{autodesktop}\my-lvgl-app"; Filename: "{app}\my-lvgl-app.exe"
[Run]
Filename: "{app}\my-lvgl-app.exe"; Description: "Launch my-lvgl-app"; Flags: nowait postinstall skipifsilent
"@
Set-Content -Path installer.iss -Value $iss -Encoding UTF8
& "$env:ProgramFiles(x86)\Inno Setup 6\ISCC.exe" installer.iss
