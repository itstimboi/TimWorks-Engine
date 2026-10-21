@REM  @REM  @REM set "EXTERNAL_INCLUDE=include/"
@REM  @REM  @REM set "LIB_PATH=lib/"

@REM  @REM  g++ -I"../include/" -I"../Engine/" -L"../lib/" ^
@REM  @REM  ../glad.c ^
@REM  @REM  ../Engine/*.cpp ^
@REM  @REM  ../Engine/Camera/*.cpp ^
@REM  @REM  ../Engine/Rendering/*.cpp ^
@REM  @REM  ../Engine/Textures/*.cpp ^
@REM  @REM  ../Engine/Light/*.cpp ^
@REM  @REM  ../Engine/Mesh/*.cpp ^
@REM  @REM  ../Engine/Input/*.cpp ^
@REM  @REM  ../Engine/Map/*.cpp ^
@REM  @REM  ../Engine/Graphics/*.cpp ^
@REM  @REM  ../Engine/Windows/*.cpp ^
@REM  @REM  ../Engine/Sprites/*.cpp ^
@REM  @REM  ../Engine/Audio/*.cpp ^
@REM  @REM  ../include/ImGUI/*.cpp ^
@REM  @REM  -lglfw3 -lgdi32 -lopengl32 -luser32 -lkernel32 -ld3dcompiler -ldxgi -ld3d11 -ld3d12 -lversion ^
@REM  @REM  -static -static-libstdc++ -static-libgcc ^
@REM  @REM  -o ../Build/Engine.exe

@REM  @REM  echo All Done

@REM  @REM  @REM Engine/Audio/stb_vorbis.c ^

@REM  @REM  ../Engine/*.cpp ^
@REM  @REM  ../Engine/Windows/*.cpp ^

@REM  @REM  @echo off

@REM  @REM  echo ========================================
@REM  @REM  echo Building TimWorks Engine DLL...
@REM  @REM  echo ========================================

@REM  @REM  if not exist "../Build" mkdir "../Build"
@REM  @REM  if not exist "../Game/include" mkdir "../Game/include"
@REM  @REM  if not exist "../Game/lib" mkdir "../Game/lib"

@REM  @REM  g++ -shared ^
@REM  @REM  -I"../include/" ^
@REM  @REM  -I"../Engine/" ^
@REM  @REM  -L"../lib/" ^
@REM  @REM  ../glad.c ^
@REM  @REM  ../Engine/Camera/*.cpp ^
@REM  @REM  ../Engine/Rendering/*.cpp ^
@REM  @REM  ../Engine/Textures/*.cpp ^
@REM  @REM  ../Engine/Light/*.cpp ^
@REM  @REM  ../Engine/Mesh/*.cpp ^
@REM  @REM  ../Engine/Input/*.cpp ^
@REM  @REM  ../Engine/Map/*.cpp ^
@REM  @REM  ../Engine/Graphics/*.cpp ^
@REM  @REM  ../Engine/Sprites/*.cpp ^
@REM  @REM  ../Engine/Audio/*.cpp ^
@REM  @REM  ../include/ImGUI/*.cpp ^
@REM  @REM  -lglfw3 ^
@REM  @REM  -lgdi32 ^
@REM  @REM  -lopengl32 ^
@REM  @REM  -luser32 ^
@REM  @REM  -lkernel32 ^
@REM  @REM  -ld3dcompiler ^
@REM  @REM  -ldxgi ^
@REM  @REM  -ld3d11 ^
@REM  @REM  -ld3d12 ^
@REM  @REM  -lversion ^
@REM  @REM  -static-libstdc++ ^
@REM  @REM  -static-libgcc ^
@REM  @REM  -o "../Build/TimWorks-Engine.dll" -Wl,--out-implib,../Game/lib/libTimWorksEngine64.a

@REM  @echo off
@REM  setlocal EnableDelayedExpansion

@REM  cd /d "%~dp0"

@REM  echo ========================================
@REM  echo   Building TimWorks Engine
@REM  echo ========================================
@REM  echo.

@REM  set "CC=gcc"
@REM  set "CXX=g++"
@REM  set "ROOT=%CD%"
@REM  set "OBJDIR=%ROOT%\Build\obj"

@REM  if not exist "%OBJDIR%" mkdir "%OBJDIR%"

@REM  echo Source directory:
@REM  echo %ROOT%\Engine
@REM  echo.

@REM  echo [1/2] Compiling C++ files...
@REM  echo.

@REM  REM ================================================================
@REM  REM Compile every .cpp individually
@REM  REM ================================================================

@REM  for /R "%ROOT%\Engine" %%F in (*.cpp) do (

@REM      echo Compiling:
@REM      echo   %%F

@REM      "%CXX%" -c ^
@REM          -I"%ROOT%\..\include" ^
@REM          -I"%ROOT%\..\Engine" ^
@REM          ../Engine/*.cpp ^
@REM          ../Engine/Camera/*.cpp ^
@REM          ../Engine/Rendering/*.cpp ^
@REM          ../Engine/Textures/*.cpp ^
@REM          ../Engine/Light/*.cpp ^
@REM          ../Engine/Mesh/*.cpp ^
@REM          ../Engine/Input/*.cpp ^
@REM          ../Engine/Map/*.cpp ^
@REM          ../Engine/Graphics/*.cpp ^
@REM          ../Engine/Windows/*.cpp ^
@REM          ../Engine/Sprites/*.cpp ^
@REM          ../Engine/Audio/*.cpp ^
@REM          "%%F" ^
@REM          -o "%OBJDIR%\%%~nF.o"

@REM      if errorlevel 1 (
@REM          echo.
@REM          echo FAILED:
@REM          echo %%F
@REM          goto :compile_failed
@REM      )
@REM  )

@REM  REM ================================================================
@REM  REM Compile ImGui
@REM  REM ================================================================

@REM  echo.
@REM  echo Compiling ImGui...
@REM  echo.

@REM  for /R "%ROOT%\include\ImGUI" %%F in (*.cpp) do (

@REM      echo Compiling:
@REM      echo   %%F

@REM      "%CXX%" -c ^
@REM          -I"%ROOT%\..\include" ^
@REM          -I"%ROOT%\..\Engine" ^
@REM          "%%F" ^
@REM          -o "%OBJDIR%\imgui_%%~nF.o"

@REM      if errorlevel 1 (
@REM          echo.
@REM          echo FAILED:
@REM          echo %%F
@REM          goto :compile_failed
@REM      )
@REM  )

@REM  REM ================================================================
@REM  REM GLAD
@REM  REM ================================================================

@REM  echo.
@REM  echo Compiling GL VERSIONS...
@REM  echo.

@REM  "%CC%" -c ^
@REM      -I"%ROOT%\..\include" ^
@REM      -I"%ROOT%\..\Engine" ^
@REM      "%ROOT%\..\Engine\OpenGL\*.c" ^
@REM      -o "%OBJDIR%\glad46.o"

@REM  if errorlevel 1 goto :compile_failed

@REM  REM ================================================================
@REM  REM Link
@REM  REM ================================================================

@REM  echo.
@REM  echo ========================================
@REM  echo   Linking TimWorks-Engine.dll
@REM  echo ========================================
@REM  echo.

@REM  "%CXX%" -shared ^
@REM      "%OBJDIR%\*.o" ^
@REM      -L"%ROOT%\lib" ^
@REM      -lglfw3 ^
@REM      -lgdi32 ^
@REM      -lopengl32 ^
@REM      -luser32 ^
@REM      -lkernel32 ^
@REM      -ld3dcompiler ^
@REM      -ldxgi ^
@REM      -ld3d11 ^
@REM      -ld3d12 ^
@REM      -lversion ^
@REM      -static-libstdc++ ^
@REM      -static-libgcc ^
@REM      -static ^
@REM      -o "%ROOT%\..\Build\TimWorks-Engine.dll" ^
@REM      -Wl,--out-implib,"%ROOT%\..\Game\lib\libTimWorksEngine64.a"

@REM  if errorlevel 1 goto :link_failed

@REM  echo.
@REM  echo ========================================
@REM  echo   BUILD SUCCESSFUL
@REM  echo ========================================
@REM  echo.

@REM  exit /b 0


@REM  :compile_failed

@REM  echo.
@REM  echo ========================================
@REM  echo   COMPILATION FAILED
@REM  echo ========================================
@REM  echo.

@REM  exit /b 1


@REM  :link_failed

@REM  echo.
@REM  echo ========================================
@REM  echo   LINKING FAILED
@REM  echo ========================================
@REM  echo.

@REM  exit /b 1

@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

echo ========================================
echo   Building TimWorks Engine
echo ========================================
echo.

set "CC=gcc"
set "CXX=g++"

set "ROOT=%CD%"
set "ENGINE=%ROOT%\..\Engine"
set "INCLUDE=%ROOT%\..\include"
set "LIB=%ROOT%\..\lib"

set "BUILD=%ROOT%\..\Build"
set "OBJDIR=obj"

if not exist "%BUILD%" mkdir "%BUILD%"
if not exist "%OBJDIR%" mkdir "%OBJDIR%"
if not exist "%ROOT%\..\Game\lib" mkdir "%ROOT%\..\Game\lib"

echo Root:
echo   %ROOT%
echo.

echo Engine:
echo   %ENGINE%
echo.

echo Object output:
echo   %OBJDIR%
echo.

REM ================================================================
REM Compile C++ files
REM ================================================================

echo ========================================
echo   [1/3] Compiling C++ files
echo ========================================
echo.

for /R "%ENGINE%" %%F in (*.cpp) do (

    echo Compiling:
    echo   %%F

    "%CXX%" -c ^
        -I"%INCLUDE%" ^
        -I"%ENGINE%" ^
        "%%F" ^
        -o "%OBJDIR%\%%~nF.o"

    if errorlevel 1 (
        echo.
        echo FAILED:
        echo %%F
        goto :compile_failed
    )
)

REM ================================================================
REM Compile ImGui
REM ================================================================

echo.
echo ========================================
echo   [2/3] Compiling ImGui
echo ========================================
echo.

for /R "%INCLUDE%\ImGUI" %%F in (*.cpp) do (

    echo Compiling:
    echo   %%F

    "%CXX%" -c ^
        -I"%INCLUDE%" ^
        -I"%ENGINE%" ^
        "%%F" ^
        -o "%OBJDIR%\imgui_%%~nF.o"

    if errorlevel 1 (
        echo.
        echo FAILED:
        echo %%F
        goto :compile_failed
    )
)

REM ================================================================
REM Compile C files
REM ================================================================

echo.
echo ========================================
echo   [3/3] Compiling C files
echo ========================================
echo.

for /R "%ENGINE%" %%F in (*.c) do (

    echo Compiling:
    echo   %%F

    "%CC%" -c ^
        -I"%INCLUDE%" ^
        -I"%ENGINE%" ^
        "%%F" ^
        -o "%OBJDIR%\%%~nF.o"

    if errorlevel 1 (
        echo.
        echo FAILED:
        echo %%F
        goto :compile_failed
    )
)

REM ================================================================
REM Link DLL
REM ================================================================

echo.
echo ========================================
echo   Linking TimWorks-Engine.dll
echo ========================================
echo.

"%CXX%" -shared ^
    "%OBJDIR%\*.o" ^
    -L"%LIB%" ^
    -lglfw3 ^
    -lgdi32 ^
    -lopengl32 ^
    -luser32 ^
    -lkernel32 ^
    -ld3dcompiler ^
    -ldxgi ^
    -ld3d11 ^
    -ld3d12 ^
    -lversion ^
    -static-libstdc++ ^
    -static-libgcc ^
    -o "%BUILD%\TimWorks-Engine.dll" ^
    -Wl,--out-implib,"%ROOT%\..\Game\lib\libTimWorksEngine64.a"

if errorlevel 1 goto :link_failed

echo.
echo ========================================
echo   BUILD SUCCESSFUL
echo ========================================
echo.

echo DLL:
echo   %BUILD%\TimWorks-Engine.dll
echo.

echo Import library:
echo   %ROOT%\..\Game\lib\libTimWorksEngine64.a
echo.

exit /b 0


:compile_failed

echo.
echo ========================================
echo   COMPILATION FAILED
echo ========================================
echo.

exit /b 1


:link_failed

echo.
echo ========================================
echo   LINKING FAILED
echo ========================================
echo.

exit /b 1