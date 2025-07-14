@echo off
:: SmartIncubator Flash Script Stub
:: Author: Samson
:: Target: EK-TM4C1294XL
:: Toolchain: EWARM

REM === Configure paths ===
SET IAR_PATH=" C:\Program Files\IAR Systems\Embedded Workbench 9.1"
SET PROJECT_PATH="C:\ti\TivaWare_C_Series-2.2.0.295\examples\boards\ek-tm4c1294xl\SmartIncubatorProject.ewp"

REM === Call IAR Build Tool ===
REM Example only; confirm actual command-line utility name and flags
REM IAR uses 'IarBuild.exe' for batch compilation

ECHO Building project...
%IAR_PATH%\common\bin\IarBuild.exe %PROJECT_PATH% -build SmartIncubatorConfig -log all

REM === Flash Firmware ===
REM Replace with actual flash utility or command
ECHO Flashing to EK-TM4C1294XL...
REM %IAR_PATH%\tools\FlashTool.exe --device TM4C1294NCPDT --input Output\SmartIncubator.hex

ECHO Done.
PAUSE
