@echo off

if "%1" == "regenerate" (

goto :cmake

)

rmdir /q /s Target
mkdir Target

:cmake

cmake.exe -G "Visual Studio 15 2017" -TLLVM -A x64 -BTarget -H.

pause
