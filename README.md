[![Build status](https://ci.appveyor.com/api/projects/status/pwibl81sxh2t8f1g/branch/develop?svg=true)](https://ci.appveyor.com/project/garciaadrian/ecu/branch/develop)

# Table of content
* [Installation](#installation)
* [Building from source](#building-from-source)

# Installation
To grab the latest development version click [here](https://ci.appveyor.com/project/garciaadrian/ecu/branch/develop) then click artifacts and download the zip archive or the installer

# Building from source
## Requirements
- Visual Studio 2015/2017
- Qt5
- CMake 3.6 or later
- Python 2.7 and pip
- 7z

Open command prompt and type

```
ers pull
ers setup
ers build
ers devenv
```

In your shell invoke `ers` or `./ecu.py` if using bash/cygwin to see what commands are available
