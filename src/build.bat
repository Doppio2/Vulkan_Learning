@echo off

REM TODO: короче в path он либу не вставил. Мне вручную придется пути прописывать и т.д, а так должно работать.

set VulkanPath=C:\VulkanSDK\1.4.313.0
set VulkanInclude=%VulkanPath%\Include
set VulkanLib=%VulkanPath%\Lib

set CommonCompilerFlags=-MT -nologo -D_CRT_SECURE_NO_WARNINGS -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4530 -wd4201 -wd4100 -wd4101 -wd4189 -FC -Z7 %VulkanLib%\vulkan-1.lib
set CommonLinkerFlags=user32.lib Gdi32.lib Shell32.lib winmm.lib -opt:ref

set Includes=/I %VulkanInclude%

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
cl %CommonCompilerFlags% %Includes% ../src/win32_vulkan_app.cpp /link %CommonLinkerFlags% 
popd
