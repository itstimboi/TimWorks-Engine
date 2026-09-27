@REM set "EXTERNAL_INCLUDE=include/"
@REM set "LIB_PATH=lib/"

@REM "../../../msys64/ucrt64/bin/g++.exe"
@REM  g++ -I"../include/" -L"../lib/" ^
@REM  build.cpp ^
@REM  ../include/ImGUI/*.cpp ^
@REM  ../include/rlImGUI/*.cpp ^
@REM  -lraylib ^
@REM  -lwinmm ^
@REM  -lgdi32 ^
@REM  -lopengl32 ^
@REM  -luser32 ^
@REM  -lkernel32 ^
@REM  -mwindows ^
@REM  -o build.exe

g++ -I"../include/" -L"../lib/" build.cpp ../include/ImGUI/*.cpp ../include/rlImGUI/*.cpp -lraylib -lwinmm -lgdi32 -lopengl32 -luser32 -lkernel32 -ld3dcompiler -ldxgi -ld3d11 -ld3d12 -static -static-libstdc++ -static-libgcc -mwindows -o build.exe

echo All Done