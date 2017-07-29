@echo off
 
echo Extract Gxx Files
 
for /f "tokens=*" %%m in ("%CD%") do set DST_DIRNAME=%%~nm_GB

set ZIP="C:\Program Files\7-Zip\7z.exe"

mkdir %DST_DIRNAME%
 
echo Top layer:*.GTL
copy *.GTL %DST_DIRNAME%
echo Bottom layer:*.GBL
copy *.GBL %DST_DIRNAME%
echo Solder Stop Mask Top:*.GTS
copy *.GTS %DST_DIRNAME%
echo Solder Stop Mask Bottom:*.GBS
copy *.GBS %DST_DIRNAME%
echo Silk Top:*.GTO
copy *.GTO %DST_DIRNAME%
echo Silk Bottom:*.GBO
copy *.GBO %DST_DIRNAME%
echo NC Drill:*.TXT
copy *.TXT %DST_DIRNAME%
echo Mechanical layer :*.GML
copy *.GML %DST_DIRNAME%

%ZIP% a -tzip -r %DST_DIRNAME%.zip %DST_DIRNAME%

rd /s /q %DST_DIRNAME%

echo end.
pause