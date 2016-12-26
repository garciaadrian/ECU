ECU is a sofware used to make detailed performance analysis on the
iRacing MP4-30

#Installing
Open command prompt and type

```
ers setup
```

The generated solution has an odd problem where the working directory is set to 'ECU/build' directory. This only happens to the
debug builds. The working directory should be 'ECU/build/bin/Debug'

To fix this open the solution generated in the build directory and make sure the configuration is Debug. Right-click ECU-core and choose properties. Click the Debugging section and copy the following section into 'Working Directory'

```
$(ProjectDir)bin\Debug
```