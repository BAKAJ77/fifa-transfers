@echo off
set /p compilerID="Enter the action to generate project files for (Refer to Premake Docs for available actions): "

set generatorID=""
if "%compilerID%"=="vs2022" set generatorID="Visual Studio 17 2022"
if "%compilerID%"=="vs2019" set generatorID="Visual Studio 16 2019"
if "%compilerID%"=="gmake2" set generatorID="MinGW Makefiles"

if %generatorID%=="" (
    echo The entered action is not valid/supported.
    goto exit
)

cd /d "%~dp0\bin\premake\win64"
premake5 %compilerID% --file="..\..\..\premake.lua"

cd /d "%~dp0\libs\glfw"
cmake -B build -D USE_MSVC_RUNTIME_LIBRARY_DLL=false -A x64 -G %generatorID%

cd /d "%~dp0\libs\freetype"
cmake -B build -A x64 -G %generatorID%

:exit
pause