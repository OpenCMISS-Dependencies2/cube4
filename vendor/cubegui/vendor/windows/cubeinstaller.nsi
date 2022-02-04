;/****************************************************************************
;**  CUBE        http://www.scalasca.org/                                   **
;*****************************************************************************
;**  Copyright (c) 1998-2020                                                **
;**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
;**                                                                         **
;**  Copyright (c) 2009-2015                                                **
;**  German Research School for Simulation Sciences GmbH,                   **
;**  Laboratory for Parallel Programming                                    **
;**                                                                         **
;**  This software may be modified and distributed under the terms of       **
;**  a BSD-style license.  See the COPYING file in the package base         **
;**  directory for details.                                                 **
;****************************************************************************/




; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,
!include fileassoc.nsh

XPStyle on

Function .onInit

	# the plugins dir is automatically deleted when the installer exits
	InitPluginsDir
	File /oname=$PLUGINSDIR\splash.gif install\share\icons\cubelogo.gif
	#optional
	#File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"

	newadvsplash::show 2000 500 500 -2 $PLUGINSDIR\splash.gif

	Pop $0 ; $0 has '1' if the user closed the splash screen early,
			; '0' if everything closed normally, and '-1' if some error occurred.
FunctionEnd


;--------------------------------

; The name of the installer
Name "Cube (scalasca.org)"

; The file to write
OutFile "cube-setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\CUBE

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Scalasca\Cube\v4" "Install_Dir"

; Request application privileges for Windows Vista
;RequestExecutionLevel admin
;--------------------------------
;LicenseText "Cube license agreement"
;LicenseData "install\cubelicense.txt"
; Pages
;Page license
 PageEx license
   LicenseText "CUBE License aggreement" "I agree"
   LicenseData install\cubelicense.txt
 PageExEnd

 PageEx license
   LicenseText "OTF2 License aggreement" "I agree"
   LicenseData install\otf2license.txt
 PageExEnd
 
 PageEx license
   LicenseText "Experimental plug-ins"
   LicenseData install\lib\cube-plugin\plugins.readme
   LicenseForceSelection checkbox
 PageExEnd  
  
Page components
Page directory
Page instfiles




UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Cube4 Installation" 0

  SectionIn RO


  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\bin
  ; Put file there
    File "install\bin\*.*"
  SetOutPath $INSTDIR\bin\platforms
  ; Put file there
    File "install\bin\platforms\*.*"
  SetOutPath $INSTDIR\bin\styles
  ; Put file there
    File "install\bin\styles\*.*"
  SetOutPath $INSTDIR\bin\imageformats
  ; Put file there
    File "install\bin\imageformats\*.*"
  SetOutPath $INSTDIR\bin
  ; Put file there
    File "install\bin\*.*"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\share\icons
  	File "install\share\icons\cubeicon.ico"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\share\doc\cube
  	    File "install\share\doc\cube\*.pdf"
  SetOutPath $INSTDIR\share\doc\cubegui\examples
  	    File "install\share\doc\cubegui\examples\*"
  SetOutPath $INSTDIR\share\doc\cubegui\guide\html
  	    File "install\share\doc\cubegui\guide\html\*"
  SetOutPath $INSTDIR\share\doc\cubegui\plugins-guide\html
  	    File "install\share\doc\cubegui\plugins-guide\html\*"
  SetOutPath $INSTDIR\lib
  	File "install\lib\*.*"
  SetOutPath $INSTDIR\include\cubew
  	File "install\include\cubew\*.*"
  SetOutPath $INSTDIR\include\cubelib
  	File "install\include\cubelib\*.*"
  SetOutPath $INSTDIR\include\cubegui
  	File "install\include\cubegui\*.*"
	

	
  ; Write the installation path into the registry
  WriteRegStr HKLM Software\Scalasca\Cube\v4 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "DisplayName" "Scalasca CUBE v4"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  EnVar::SetHKCU
  EnVar::AddValue "PATH" "$INSTDIR\bin"
  EnVar::AddValue "CUBE_DOCPATH" "http://apps.fz-juelich.de/scalasca/releases/scalasca/2.5/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/2.4/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/2.3/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/2.2/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/2.1/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/2.0/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/1.4/help/;http://apps.fz-juelich.de/scalasca/releases/scalasca/1.3/help/;http://perftools.pages.jsc.fz-juelich.de/cicd/scorep/profile_metrics/;http://perftools-ci.pages.jsc.fz-juelich.de/scorep/profile_metrics/;http://scorepci.pages.jsc.fz-juelich.de/scorep-pipelines/docs/metrics/"
  
!insertmacro APP_ASSOCIATE "cubex" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe -docpath=$\"$INSTDIR\share\doc$\" $\"%1$\""
!insertmacro APP_ASSOCIATE "cube" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe -docpath=$\"$INSTDIR\share\doc$\" $\"%1$\""
!insertmacro APP_ASSOCIATE "cube.gz" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe -docpath=$\"$INSTDIR\share\doc$\" $\"%1$\""	 
  
  !insertmacro UPDATEFILEASSOC
SectionEnd


; The stuff to install
Section /o "Cube4 experimental plugins installation" 1
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\bin
  ; Put file there
    File "install\lib\otf2\bin\otf2.dll"
  SetOutPath $INSTDIR\lib
  	File "install\lib\otf2\lib\libotf2.a"
  SetOutPath $INSTDIR\include\otf2\otf2\*.*
  	File "install\lib\otf2\include\otf2\otf2\*.*"
  SetOutPath $INSTDIR\lib\cube-plugin
  	File "install\lib\cube-plugin\*.*"
SectionEnd



; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\Scalasca\Cube"
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\Cube(scalasca.org).lnk" "$INSTDIR\bin\cube.exe" "-docpath=$\"$INSTDIR\share\doc$\"" "$INSTDIR\share\icons\cubeicon.ico" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubeUserGuide.lnk" "$INSTDIR\share\doc\cubegui\CubeUserGuide.pdf" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubePluginsUserGuide.lnk" "$INSTDIR\share\doc\cubegui\CubePluginsUserGuide.pdf" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubeUserGuide.lnk" "$INSTDIR\share\doc\cubegui\guide\html\index.html" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubePluginsUserGuide.lnk" "$INSTDIR\share\doc\cubegui\plugins-guide\html\index.html" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubeDerivedMetricsGuide.lnk" "$INSTDIR\share\doc\cube\CubeDerivedMetricsGuide.pdf" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubeToolsGuide.lnk" "$INSTDIR\share\doc\cube\CubeToolsGuide.pdf" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\CubeToolDevelopmentGuide.lnk" "$INSTDIR\share\doc\cube\CubeToolDevelopmentGuide.pdf" "" "" 0
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\Example.lnk" "$INSTDIR\share\doc\cubegui\examples\trace.cubex" "" "" 0
  
SectionEnd
; Optional section (can be disabled by the user)
Section "Desktop Shortcuts"
  CreateShortCut  "$DESKTOP\Cube(scalasca.org).lnk" "$INSTDIR\bin\cube.exe" "-docpath=$\"$INSTDIR\share\doc$\"" "$INSTDIR\share\icons\cubeicon.ico" 0
SectionEnd
;--------------------------------

; Uninstaller

Section "Uninstall"
	
  ; Remove files registration
   !insertmacro APP_UNASSOCIATE "cube" "Cube.Profile"
   !insertmacro APP_UNASSOCIATE "cube.gz" "Cube.Profile"
   !insertmacro APP_UNASSOCIATE "cubex" "Cube.Profile"   
   
  ; Adopt PATH variable
  EnVar::SetHKCU
  EnVar::DeleteValue "PATH" "$INSTDIR\bin"
  EnVar::Delete "CUBE_DOCPATH"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4"
  DeleteRegKey HKLM "Software\Scalasca\Cube\v4"

  ; Remove files and uninstaller
  
  Delete "$INSTDIR\cubeinstaller.nsi"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\bin\bearer\*.*"
  Delete "$INSTDIR\bin\platforms\*.*"
  Delete "$INSTDIR\bin\styles\*.*"
  Delete "$INSTDIR\bin\imageformats\*.*"
  Delete "$INSTDIR\bin\*.*"
  Delete "$INSTDIR\lib\cube-plugin\*.*"
  Delete "$INSTDIR\lib\*.*"
  Delete "$INSTDIR\include\cubew\*.*"  
  Delete "$INSTDIR\include\cubelib\*.*"
  Delete "$INSTDIR\include\cubegui\*.*"
  Delete "$INSTDIR\include\otf2\otf2\*.*"
  Delete "$INSTDIR\include\*.*"
  Delete "$INSTDIR\share\doc\cubegui\examples\*.*"
  Delete "$INSTDIR\share\doc\cubew\examples\*.*"
  Delete "$INSTDIR\share\doc\cubelib\examples\*.*"
  Delete "$INSTDIR\share\doc\cube\examples\*.*"
  Delete "$INSTDIR\share\doc\cubegui\guide\html\*.*"
  Delete "$INSTDIR\share\doc\cubegui\plugins-guide\html\*.*"
  Delete "$INSTDIR\share\doc\cube\*.*"
  Delete "$INSTDIR\share\doc\cubegui\*.*"
  Delete "$INSTDIR\share\icons\*.*"
  Delete "$INSTDIR\*.*"
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Scalasca\Cube\*.*"
  Delete "$SMPROGRAMS\Scalasca\*.*"
  Delete "$DESKTOP\Cube(scalasca.org).lnk"
  ; Remove directories used

  RMDir "$INSTDIR\bin\bearer"
  RMDir "$INSTDIR\bin\platforms"
  RMDir "$INSTDIR\bin\styles"
  RMDir "$INSTDIR\bin\imageformats"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\lib\cube-plugin"
  RMDir "$INSTDIR\lib"
  RMDir "$INSTDIR\include\cubegui"
  RMDir "$INSTDIR\include\cubelib"
  RMDir "$INSTDIR\include\cubew"
  RMDir "$INSTDIR\include\cube"
  RMDir "$INSTDIR\include\otf2\otf2"  
  RMDir "$INSTDIR\include\otf2"  
  RMDir "$INSTDIR\include"
  RMDir "$INSTDIR\share\doc\cube\examples"
  RMDir "$INSTDIR\share\doc\cubew\examples"
  RMDir "$INSTDIR\share\doc\cubelib\examples"
  RMDir "$INSTDIR\share\doc\cubegui\examples"
  RMDir "$INSTDIR\share\doc\cubegui\guide\html"
  RMDir "$INSTDIR\share\doc\cubegui\plugins-guide\html"
  RMDir "$INSTDIR\share\doc\cube"
  RMDir "$INSTDIR\share\doc\cubegui\guide"
  RMDir "$INSTDIR\share\doc\cubegui\plugins-guide"
  RMDir "$INSTDIR\share\doc\cubegui"
  RMDir "$INSTDIR\share\doc\cubelib"
  RMDir "$INSTDIR\share\doc\cubew"
  RMDir "$INSTDIR\share\doc\cube"
  RMDir "$INSTDIR\share\doc"
  RMDir "$INSTDIR\share\icons"
  RMDir "$INSTDIR\share"
  RMDir "$PROGRAMFILES\CUBE"
  RMDir "$INSTDIR"
  RMDir "$SMPROGRAMS\Scalasca\Cube"
  RMDir "$SMPROGRAMS\Scalasca"
SectionEnd


