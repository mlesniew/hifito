!include WinMessages.nsh
!include "MUI2.nsh"

;--- General ----------------------------------------------------------------

	Name "Hifito"
	OutFile "Hifito-setup.exe"

	InstallDir $PROGRAMFILES\Hifito
	InstallDirRegKey HKLM "Software\Hifito" "InstallDir"	

	RequestExecutionLevel admin
	
	!define MUI_ICON "Hifito.ico"
	!define MUI_UNICON "Hifito.ico"
	
;--- Functions --------------------------------------------------------------

!macro CloseHifito
	retry:
	FindWindow $0 "HifitoMessageWindowClass" "HifitoMessageWindow"
		StrCmp $0 0 notRunning
		SendMessage $0 ${WM_CLOSE} 0 0
		; Give the instance some time to close and retry 
		sleep 1000	
		goto retry
	notRunning:
!macroend
	
;--- Variables --------------------------------------------------------------
	
	Var StartMenuFolder
	
;--- Interface settings -----------------------------------------------------

	!define MUI_ABORTWARNING

;--- Pages ------------------------------------------------------------------
	
	!insertmacro 	MUI_PAGE_WELCOME
	
	!define 		MUI_LICENSEPAGE_CHECKBOX
	!insertmacro 	MUI_PAGE_LICENSE "License.txt"
	
	!define 		MUI_COMPONENTSPAGE_NODESC
	!insertmacro 	MUI_PAGE_COMPONENTS
	
	!insertmacro 	MUI_PAGE_DIRECTORY
	
	!define 		MUI_STARTMENUPAGE_REGISTRY_ROOT 				"HKCU" 
	!define 		MUI_STARTMENUPAGE_REGISTRY_KEY 					"Software\Hifito" 
	!define 		MUI_STARTMENUPAGE_REGISTRY_VALUENAME 			"Hifito"
	!insertmacro 	MUI_PAGE_STARTMENU Application $StartMenuFolder
	
	!insertmacro 	MUI_PAGE_INSTFILES
	
	!define 		MUI_FINISHPAGE_RUN   			"$INSTDIR\Hifito.exe"
	!define 		MUI_FINISHPAGE_SHOWREADME 		"$INSTDIR\Readme.txt"
	!define 		MUI_FINISHPAGE_LINK				"Visit Hifito Website"
	!define 		MUI_FINISHPAGE_LINK_LOCATION 	"http://mlesniew.wordpress.com/Hifito"
	!insertmacro 	MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH

;--- Language ---------------------------------------------------------------
	
!insertmacro MUI_LANGUAGE "English"

;--- Sections ---------------------------------------------------------------

Section "Hifito program files" SecDefault
	SectionIn RO

	; Close Hifito
	!insertmacro CloseHifito
	
	; Install files
	SetOutPath $INSTDIR
	File "Hifito.exe"
	File "License.txt"
	File "Readme.txt"
	File "Hifito.exe"
	
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "DisplayName" "Hifito"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hifito" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
	
	; Start menu entries
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Hifito.lnk" "$INSTDIR\Hifito.exe"
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	!insertmacro MUI_STARTMENU_WRITE_END
	
SectionEnd

; Autostart
Section "Launch Hifito on system startup" SecAutostart
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "Hifito" '"$INSTDIR\Hifito.exe"'
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
	Delete $INSTDIR\uninstall.exe

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	
	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\$StartMenuFolder\*.*"

	; Remove directories used
	RMDir "$SMPROGRAMS\Hifito"
	RMDir "$INSTDIR"

SectionEnd