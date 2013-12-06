@echo off
set file=temp.reg
set hstart=%cd%\bin\hstart
set binPath=%cd%\bin\ShutdownServer.exe
set hstart=%hstart:\=\\%
set binPath=%binPath:\=\\% 

echo Windows Registry Editor Version 5.00 > temp.reg
echo. >> temp.reg
echo [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run] >> temp.reg
echo "ShutdownServer"="%hstart% /NOCONSOLE %binPath%" >> temp.reg

temp.reg
del temp.reg
