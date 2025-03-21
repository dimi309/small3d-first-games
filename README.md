[small3d](https://github.com/dimi309/small3d) first games 
==========================================================

# Building

This repository contains the small3d repository as a submodule. Use 
the `--recursive` flag when cloning, otherwise the small3d subdirectory 
will remain empty on your drive.

Build by executing:
 
	- `build-all-mingw.bat` for a MinGW gcc build on Windows 
	- `build-all-vs.bat` for a Visual Studio build on Windows
	- `build-all.sh` for gcc/llvm builds on Linux
	
# Running

After the build script has run, the executable for each game can be found in its
`build/bin` subdirectory.

# Prerequisites

The following need to be installed, with the relevant environment variables and 
tools accessible via the command line:

	- Some compiler, gcc, Visual Studio, clang, etc.
	- 7zip (only on Windows)
	- CMake
	- Git
