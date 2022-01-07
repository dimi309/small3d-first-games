[small3d](https://github.com/dimi309/small3d) sample games and demos
====================================================================

[![Build status](https://ci.appveyor.com/api/projects/status/l1lndxjhxmrfmouf?svg=true)](https://ci.appveyor.com/project/dimi309/small3d-samples)

# Building

Four of the games are included in IDE projects for mobile devices:

	- avoidthebug-android & islet-hell-android: just open with Android Studio and build and deploy from there

	- avoidthebug-ios & islet-hell-ios: the same, but with Xcode

The rest of the games can be built by executing:
 
	- `build-all-mingw.bat` for a MinGW gcc build on Windows 
	- `build-all-vs.bat` for a Visual Studio (2019) build on Windows
	- `build-all.sh` for gcc/llvm builds on MacOS and Linux

## Prerequisites:

The following need to be installed, with the relevant environment variables and tools accessible via the command line:

	- Some compiler, gcc, Visual Studio, clang, etc.
	- Vulkan SDK (if it will be used)
	- 7zip (only on Windows)
	- CMake
	- Git
