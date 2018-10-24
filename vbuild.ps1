pushd .\build
cl ..\src\main.cpp /W4 /EHsc SDL2.lib SDL2main.lib SDL2_image.lib -link /subsystem:console /MACHINE:X64
popd
