@REM set "EXTERNAL_INCLUDE=include/"
@REM set "LIB_PATH=lib/"

g++ -I"../include/" -I"../Engine/" -L"../lib/" ^
../glad.c ^
../Engine/*.cpp ^
../Engine/Camera/*.cpp ^
../Engine/Rendering/*.cpp ^
../Engine/Textures/*.cpp ^
../Engine/Light/*.cpp ^
../Engine/Mesh/*.cpp ^
../Engine/Map/*.cpp ^
../Engine/Graphics/*.cpp ^
../Engine/Windows/*.cpp ^
../Engine/Sprites/*.cpp ^
../Engine/Audio/*.cpp ^
../include/ImGUI/*.cpp ^
-lglfw2 -lgdi32 -ld3dcompiler -ldxgi -ld3d11 -ld3d12 -lopengl32 -luser32 -lkernel32 ^
-static -static-libstdc++ -static-libgcc ^
-o ../Build/Engine.exe

echo All Done

@REM Engine/Audio/stb_vorbis.c ^