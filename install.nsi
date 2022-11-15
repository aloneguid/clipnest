!define MUI_PRODUCT "Clipnest"
!define MUI_VERSION "1.0.0"
!define MUI_PUBLISHER "Ivan G"

OutFile "clipnest-installer-${MUI_VERSION}.exe"

ManifestDPIAware false

;SetCompressor zlib|bzip2|lzma
SetCompressor lzma

InstallDir "$LOCALAPPDATA\Programs\Clipnest"

VIAddVersionKey "ProductName" "${MUI_PRODUCT}"
VIAddVersionKey "Comments" ""
VIAddVersionKey "CompanyName" "${MUI_PUBLISHER}"
VIAddVersionKey "LegalCopyright" ""
VIAddVersionKey "FileDescription" ""
VIAddVersionKey "FileVersion" "${MUI_VERSION}"
VIAddVersionKey "ProductVersion" "${MUI_VERSION}"
VIProductVersion "${MUI_VERSION}.0"

Icon "clipnest\icon.ico"
UninstallIcon "clipnest\icon.ico"
RequestExecutionLevel user

Section
    SetOutPath $INSTDIR

    ;
    ;Exec '"$INSTDIR\clipnest.exe" shutdown'
    ;Sleep 1000

    File "build\clipnest\release\clipnest.exe"

    CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}.lnk" "$INSTDIR\clipnest.exe"

    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT}"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayIcon" "$INSTDIR\clipnest.exe"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayVersion" "${MUI_VERSION}"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "Publisher" "${MUI_PUBLISHER}"

    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "uninstall"
    ;Exec '"$INSTDIR\clipnest.exe" shutdown'
    ;Sleep 1000
    nsProcess::
    RMDir /r "$INSTDIR\*.*"
    RMDir "$INSTDIR"

    Delete "$SMPROGRAMS\Browser Tamer.lnk"

    DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"  

    Delete "$INSTDIR\uninstall.exe"
SectionEnd
