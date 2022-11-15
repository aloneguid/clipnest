$version = $env:VERSION
Write-Host "version: $version"

$v1 = $version -replace "\.", ","
$v1 += ",0"
$v2 = $version + ".0"
$rc = ".\clipnest\clipnest.rc"
$gh = ".\clipnest\global.h"
$nsi = ".\install.nsi"

# update version in .rc
(Get-Content $rc) `
    -replace "1,\d+,\d+,\d+", "$v1" |
    Out-File $rc
(Get-Content $rc) `
    -replace "1\.\d+\.\d+\.\d+", "$v2" |
    Out-File $rc

 # update version in global.h
(Get-Content $gh) `
    -replace "Version\s*=\s*""(1.*)""", "Version = ""$version""" |
    Out-File $gh

# update version in .nsi
(Get-Content $nsi) `
    -replace "MUI_VERSION\s*""(1.*)""", "MUI_VERSION ""$version""" |
    Out-File $nsi
