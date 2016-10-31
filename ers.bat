@echo off
REM tools\build\find.exe src/ -regex ".*\.\(h\|cpp\)" | tools\build\xargs.exe tools\build\clang-format -i style=file

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

python ers.py --setup

git submodule init
git submodule update

IF NOT EXIST "build\bin" (
   ECHO Creating build\bin
   mkdir build\bin
)

IF NOT EXIST "build\bin\Debug" (
   ECHO Creating build\bin\Debug
   mkdir build\bin\Debug
)

IF NOT EXIST "build\bin\Release" (
   ECHO Creating build\bin\Release
   mkdir build\bin\Release
)

pushd libs\cef_binary_3.2704.1414.g185cd6c_windows64\Debug
for %%f in (*.*) do mklink /H ..\..\..\build\bin\Debug\%%f %%f
popd

pushd libs\cef_binary_3.2704.1414.g185cd6c_windows64\Release
for %%f in (*.*) do mklink /H ..\..\..\build\bin\Release\%%f %%f
popd

pushd libs\cef_binary_3.2704.1414.g185cd6c_windows64\Resources
for %%f in (*.*) do mklink /H ..\..\..\build\bin\Release\%%f %%f
for %%f in (*.*) do mklink /H ..\..\..\build\bin\Debug\%%f %%f
popd

pushd build\bin\Debug
mklink /J locales ..\..\..\libs\cef_binary_3.2704.1414.g185cd6c_windows64\Resources\locales
popd

pushd build\bin\Release
mklink /J locales ..\..\..\libs\cef_binary_3.2704.1414.g185cd6c_windows64\Resources\locales
popd

pushd libs\cef_binary_3.2704.1414.g185cd6c_windows64
mkdir build && pushd build
cmake -G "Visual Studio 14 Win64" ..
msbuild /p:Configuration=Release /p:Platform="x64" cef.sln
msbuild /p:Configuration=Debug /p:Platform="x64" cef.sln
popd
popd
tools\build\premake5.exe vs2015
