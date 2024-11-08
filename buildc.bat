@echo off
SET java_exec="C:\Program Files\Java\jdk-22\bin\java.exe"
SET jar_name=\buildc.jar
SET jar_location=%cd%%jar_name%
%java_exec% -jar %jar_location%
REM -Dfile.encoding=windows-1252
echo =======================================================