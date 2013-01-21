;BGGradient 000000 0076FB FFFFFF

!define MUI_PRODUCT "VideoShellList"
!define MUI_VERSION "1.1"

!include "MUI.nsh"

Name "Video Shell List Installer"
OutFile "..\vsl-${MUI_VERSION}-setup.exe"
InstallDir $PROGRAMFILES\${MUI_PRODUCT}

!define MUI_CUSTOMPAGECOMMANDS

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Fiziki" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "VSL.StartFolder"

!define TEMP $R0

!define MUI_CHECKBITMAP  "check.bmp"
!define MUI_PROGRESSBAR  "colored"
!define MUI_ICON "install.ico"
!define MUI_UNICON "uninstall.ico"
!define MUI_WELCOMEPAGE
 !define MUI_HEADERBITMAP "header.bmp"
 !define MUI_SPECIALBITMAP  "left.bmp"
 !define MUI_HEADERBITMAP_RIGHT
!define MUI_LICENSEPAGE
!define MUI_COMPONENTSPAGE
!define MUI_DIRECTORYPAGE
!define MUI_STARTMENUPAGE
!define MUI_FINISHPAGE
  
!define MUI_ABORTWARNING
  
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE

!insertmacro MUI_LANGUAGE "Russian"

!insertmacro MUI_PAGECOMMAND_WELCOME
!insertmacro MUI_PAGECOMMAND_LICENSE
!insertmacro MUI_PAGECOMMAND_COMPONENTS
!insertmacro MUI_PAGECOMMAND_DIRECTORY
!insertmacro MUI_PAGECOMMAND_STARTMENU
!insertmacro MUI_PAGECOMMAND_INSTFILES
!insertmacro MUI_PAGECOMMAND_FINISH

LicenseData "License.txt"

;Function .onInit
;  MessageBox MB_YESNO "������ ����� ����������� ��������� VideoShellList. ����������?" IDYES cont
;    Abort
; cont:
;FunctionEnd

!ifndef NOINSTTYPES
  InstType "�����������"
  InstType "������"
!endif

Section "!����" SKernel
 SectionIn 1 2 RO
 SetOutPath "$INSTDIR"
 File "VideoShellList.dll"
 File "register.bat"
 File "unregister.bat"
 File "fsys.dll"
 File "minfo.dll"
 File "reg.dll"

 WriteUninstaller "$INSTDIR\uninstall.exe"

 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL" "DisplayName" "VideoShellList (remove only)"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL" "UninstallString" '"$INSTDIR\uninstall.exe"'
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL" "DisplayIcon" '"$INSTDIR\uninstall.exe"'
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL" "DisplayVersion" '"${MUI_VERSION}"'
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL" "Publisher" '"Trofimov Alexey"'

 !insertmacro MUI_STARTMENU_WRITE_BEGIN
   
   SetShellVarContext All 
   CreateDirectory "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}"
   CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\���������� ������.lnk" "$INSTDIR\register.bat"
   CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\�������� ��������� ������.lnk" "$INSTDIR\unregister.bat"
   CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\�������.lnk" "$INSTDIR\Uninstall.exe"

 !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

SubSection /e "�������" SPlugs

SubSection "�������" STables

Section "HTML" SSubHTML
  SectionIn 1 2 
 SetOutPath "$INSTDIR\Plugins"
 File "Plugins\HTML.vsp"
SectionEnd

SubSectionEnd
SubSection "����-�����" SList

Section "LightAlloy" SSubLA
  SectionIn 1 2 
 SetOutPath "$INSTDIR\Plugins"
 File "Plugins\LightAlloy.vsp"
SectionEnd

Section "WPL" SSubWPL
  SectionIn 1 2 
 SetOutPath "$INSTDIR\Plugins"
 File "Plugins\WPL.vsp"
SectionEnd

Section "ASX" SSubASX
  SectionIn 1 2 
 SetOutPath "$INSTDIR\Plugins"
 File "Plugins\ASX.vsp"
SectionEnd

Section "������" SSubTXT
  SectionIn 1 2 
 SetOutPath "$INSTDIR\Plugins"
 File "Plugins\TXT.vsp"
SectionEnd

SubSectionEnd
SubSectionEnd

Section "�������� ����" SSrc
  SectionIn 2 
 SetOutPath "$INSTDIR\Sources"
 File "vsl_src.rar"
SectionEnd

Section "�������������� ������" SReg
 SectionIn 1 2
 SetOutPath "$INSTDIR\"
 RegDll "$INSTDIR\VideoShellList.dll"
 IfErrors errs errsno
errs:
  MessageBox MB_OK "���������� ���������������� ������. ���������� ��������� register.bat � ������� ����� ����������."
errsno:
SectionEnd

LangString DESC_SSrc	${LANG_RUSSIAN} "����������, ���� ������ �������� ��������� ��� �������� ����� ������."
LangString DESC_SKernel	${LANG_RUSSIAN} "�������� ���������."
LangString DESC_SPlugs	${LANG_RUSSIAN} "�������������� ������ ��� ������ �������� ������ (��� ������ ��������� ��� ������� ���� ������!)."

LangString DESC_SList	${LANG_RUSSIAN} "������ ��� �������� ����-������."
LangString DESC_STables	${LANG_RUSSIAN} "������ ��� ������ �����������"

LangString DESC_SReg	${LANG_RUSSIAN} "���������������� ������ � ������� ����� ���������. � ��������� ������ �������� ���� ����� ������� �� ���� ���� ��� ��������� register.bat � ������� �������� ���������"

!insertmacro MUI_FUNCTIONS_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SSrc} $(DESC_SSrc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SKernel} $(DESC_SKernel)
  !insertmacro MUI_DESCRIPTION_TEXT ${SPlugs} $(DESC_SPlugs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SList} $(DESC_SList)
  !insertmacro MUI_DESCRIPTION_TEXT ${STables} $(DESC_STables)
  !insertmacro MUI_DESCRIPTION_TEXT ${SReg} $(DESC_SReg)
!insertmacro MUI_FUNCTIONS_DESCRIPTION_END

Section "Uninstall"
; SetOutPath "$INSTDIR\"
; UnRegDll "$INSTDIR\VideoShellList.dll"
 ExecWait "regsvr32.exe $\"$INSTDIR\VideoShellList.dll$\" /u /s"

 IfErrors 0 errno
 MessageBox MB_OK "���������� ����������������� ������. ���������� ��������� ���� unregister.bat � ������� �������� ���������."
 Abort
errno:
 DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fiziki.VSL"

 ReadRegStr ${TEMP} "${MUI_STARTMENUPAGE_REGISTRY_ROOT}" "${MUI_STARTMENUPAGE_REGISTRY_KEY}" "${MUI_STARTMENUPAGE_REGISTRY_VALUENAME}"
 StrCmp ${TEMP} "" noshortcuts
  RMDir /r "$SMPROGRAMS\${TEMP}"
 noshortcuts:

; Sleep 1000
 RmDir /r "$INSTDIR"

 IfErrors 0 endu
 MessageBox MB_OK "��������� ����� ������������. �������� ��� ���� ���������� � ������� ������� ��������� �������."
 Abort
endu:
 Delete "$INSTDIR\uninstall.exe"
SectionEnd
