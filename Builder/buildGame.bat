@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

echo ========================================
echo   Building Tabuewin Executable
echo ========================================
echo.

set "CC=gcc"
set "CXX=g++"
set "ROOT=%CD%/../Game"
set "SRC=%CD%/../Game/src/"
set "INC=%CD%/../Game/include/"
set "LIB=%CD%/../Game/lib/"

echo Source directory:
echo %ROOT%\
echo.

echo Compiling C++ files...
echo.

echo ========================================
echo   Compiling Source Code
echo ========================================
echo.

%CXX% -I"%INC%" -L"%LIB%"