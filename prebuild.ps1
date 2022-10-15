$version = $env:VERSION
Write-Host "version: $version"

$v1 = $version -replace "\.", ","
$v1 += ",0"
$v2 = $version + ".0"
$rc = ".\clipnest\clipnest.rc"

# update version in .rc
(Get-Content $rc) `
    -replace "1,\d+,\d+,\d+", "$v1" |
    Out-File $rc
(Get-Content $rc) `
    -replace "1\.\d+\.\d+\.\d+", "$v2" |
    Out-File $rc