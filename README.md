ECU is a sofware used to make detailed performance analysis on the
iRacing MP4-30 and to automate some systems

[![Build status](https://ci.appveyor.com/api/projects/status/pwibl81sxh2t8f1g/branch/develop?svg=true)](https://ci.appveyor.com/project/garciaadrian/ecu/branch/develop)

# Requirements
- Visual Studio 2015
- CMake
- Python 2.7 and pip
- npm
- Windows 7 or newer

# Installing
If you're not interested in developing then click the build button above me.
Then go to artifacts to get a zip file of the newest build.

Open command prompt and type

```
ers setup
```

Open the generated .sln in the build folder and compile from visual studio or to build from the command line type

```
ers build
```

The generated solution has an odd problem where the working directory is set to 'ECU/build' directory.
The working directory should be 'ECU/build/bin/{Configuration}'

To fix this open the solution generated in the build directory and change the configuration to Debug. Right-click ECU-core and choose properties. Click the Debugging section and copy the following section into 'Working Directory'

```
$(ProjectDir)bin\Debug
```

Now change the configuration to Release and copy the follow into 'Working Directory'

```
$(ProjectDir)bin\Release
```
