## Building instructions for Windows

### Dependencies

Install VS build tools and CMAKE
```
winget install Microsoft.VisualStudio.BuildTools
winget install Kitware.CMake
```
Clone JUCE framework
```
cd PixelDrive
git clone https://github.com/juce-framework/JUCE.git
```
Copy the ASIO SDK to PixelDrive/asiosdk_*
This can be downloaded from: https://www.steinberg.net/en/company/developers.html

### Run Cmake:
```
cmake -Bbuild (-GgeneratorName) (-DJUCE_BUILD_EXTRAS=ON)

# For 64 bit builds
cmake -Bbuild -DCMAKE_GENERATOR_PLATFORM=x64 -A x64 -G "Visual Studio 16 2019"

# To build the debug version of the standalone application
cmake --build build --config Debug --target PixelDrivePlugin_Standalone

# To build the release version of the VST3 application
cmake --build build --config Release --target PixelDrivePlugin_VST3
```

This guide allows for building the JUCE application without Visual Studio or Projucer.

Building can also be done using VS code and the CMake extension.
This simplifies the building process and allows you to build and launch
the app from VS code.
