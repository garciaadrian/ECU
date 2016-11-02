@echo off

REM move this to python script


SET DIR=%~dp0

SET FOUND_PYTHON_EXE=""
1>NUL 2>NUL CMD /c where python2
IF NOT ERRORLEVEL 1 (
   SET FOUND_PYTHON_EXE=python2
)

IF %FOUND_PYTHON_EXE% EQU "" (
   1>NUL 2>NUL CMD /c where python
   IF NOT ERRORLEVEL 1 (
      SET FOUND_PYTHON_EXE=python
   )
)

IF %FOUND_PYTHON_EXE% EQU "" (
  ECHO ERROR: no Python executable found on PATH.
  ECHO Make sure you can run 'python' or 'python2' in a Command Prompt.
  EXIT /b
)

SET "DIR=%DIR%\venv"
SET "DEACTIVATE=%DIR%\venv\Scripts\deactivate.bat"

IF "%1" == "setup" (
   IF NOT DEFINED DevEnvDir (
      CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
   )
   GOTO :setup
) ELSE (
       CALL "%DIR%\Scripts\activate.bat"
       python ers.py %*
       CALL "%DIR%\Scripts\deactivate.bat"
       EXIT /b %ERRORLEVEL%
       GOTO :eof
)

:setup
1>NUL 2>NUL CMD /c where virtualenv
IF ERRORLEVEL 1 (
   ECHO ERROR: virtualenv not installed.
   EXIT /b 1
)

IF NOT EXIST "%DIR%" (
       virtualenv venv
       PUSHD venv\Scripts
       CALL activate
       pip install click
       pip install wget
       POPD
)

git submodule init
git submodule update

python ers.py cef

CALL venv\Scripts\deactivate.bat

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
msbuild /p:Configuration=Release /p:Platform="x64" /nologo /m /v:m cef.sln
msbuild /p:Configuration=Debug /p:Platform="x64" /nologo /m /v:m cef.sln
popd
popd
tools\build\premake5.exe vs2015
