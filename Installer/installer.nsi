!include WinMessages.nsh

Name "Hifito"
OutFile "hifito-setup.exe"
Icon "install.ico"
InstallDir $PROGRAMFILES\Hifito
InstallDirRegKey HKLM "Software\Hifito" "InstallDir"
RequestExecutionLevel admin
XPStyle on
CRCCheck on

;--- Pages ------------------------------------------------------------------

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

LicenseText "This setup will install Hifito. Please read and agree to the license terms below before continuing."
LicenseData "license.rtf"

;--- Functions --------------------------------------------------------------

!macro CloseHifito
	FindWindow $0 "HifitoMessageWindowClass" "HifitoMessageWindow"
		StrCmp $0 0 notRunning
		SendMessage $0 ${WM_CLOSE} 0 0
	notRunning:
!macroend

;--- Sections ---------------------------------------------------------------

Section "Hifito"
	SectionIn RO

	; Close Hifito
	!insertmacro CloseHifito
	
	; Install files
	SetOutPath $INSTDIR
	File "Hifito.exe"
	File "Readme.txt"
	
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "DisplayName" "Hifito"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
	
SectionEnd

; Start menu icons
Section "Create Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\Hifito"
	CreateShortCut "$SMPROGRAMS\Hifito\Hifito.lnk" "$INSTDIR\Hifito.exe" "" "$INSTDIR\Hifito.exe" 0  
	CreateShortCut "$SMPROGRAMS\Hifito\Hifito readme.lnk" "$INSTDIR\Readme.txt" "" "$INSTDIR\Readme.txt" 0    
	CreateShortCut "$SMPROGRAMS\Hifito\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

; Autostart
Section "Start Hifito at system startup"
	CreateShortCut "$SMPROGRAMS\Hifito\Hifito.lnk" "$INSTDIR\Hifito.exe" "" "$INSTDIR\Hifito.exe" 0  
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "Hifito" '"$INSTDIR\Hifito.exe"'
SectionEnd

Section "Start Hifito after installation"
	Exec "$INSTDIR\Hifito.exe"
SectionEnd

;--- Uninstaller ------------------------------------------------------------

Section "Uninstall"
	; Close Hifito
	!insertmacro CloseHifito

	; Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito"
	DeleteRegKey HKCU "Software\Hifito"
	DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "Hifito"

	; Remove files and uninstaller
	Delete $INSTDIR\Hifito.exe
	Delete $INSTDIR\Readme.txt
	Delete $INSTDIR\uninstall.exe

	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\Hifito\*.*"

	; Remove directories used
	RMDir "$SMPROGRAMS\Hifito"
	RMDir "$INSTDIR"

SectionEnd
