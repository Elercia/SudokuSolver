@ECHO OFF
pushd %~dp0\
call bin\premake5.exe vs2022 
popd
PAUSE