@REM set "EXTERNAL_INCLUDE=include/"
@REM set "LIB_PATH=lib/"

g++ -I"../include/" -L"../lib/" ^
../ModelMaker/*.cpp ^
../ModelMaker/Model/*.cpp ^
../ModelMaker/FileDialogs/*.cpp ^
../ModelMaker/Base64/*.cpp ^
../ModelMaker/Textures/*.cpp ^
../ModelMaker/Tools/*.cpp ^
../include/ImGUI/*.cpp ^
../include/rlImGUI/*.cpp ^
-lraylib ^
-lcomdlg32 ^
-lwinmm ^
-lgdi32 ^
-lopengl32 ^
-luser32 ^
-lkernel32 ^
-mwindows ^
-o ../Build/ModelMaker.exe -ld3dcompiler -ldxgi -ld3d11 -ld3d12 -static-libgcc -static-libstdc++ -static

@REM -static-libwinpthread-1

echo All Done