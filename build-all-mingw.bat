@echo off
setlocal enabledelayedexpansion
set args_ok=false

if /I "%~1" == "debug" (
set args_ok=true
set BUILDTYPE=Debug
)
if /I "%~1" == "release" (
set args_ok=true
set BUILDTYPE=Release
)

if "%args_ok%" == "false" (
echo Please indicate build type: Debug or Release
endlocal & exit /b 1
)

if not exist small3d\build (
if %errorlevel% neq 0 endlocal & exit /b %errorlevel%
cd small3d
if %errorlevel% neq 0 endlocal & exit /b %errorlevel%
cd scripts
if %errorlevel% neq 0 endlocal & exit /b %errorlevel%
call build-mingw %1
if %errorlevel% neq 0 endlocal & exit /b %errorlevel%
cd ..\..
)

if /I "%~1" == "debug" set CMAKE_DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug
if /I "%~1" == "release" set CMAKE_DEFINITIONS=-DCMAKE_BUILD_TYPE=Release

for %%A in (avoidthebug, chasethegoat, frogremixed) do (
cd %%A
if exist deps rmdir /Q /S deps
mkdir deps
cd deps
for %%B in (include, lib, shaders) do (
xcopy ..\..\small3d\build\%%B %%B\ /s /e /y
if "!errorlevel!" neq "0" endlocal & exit /b !errorlevel!
)
xcopy ..\..\small3d\cmake cmake\ /s /e /y
if "!errorlevel!" neq "0" endlocal & exit /b !errorlevel!
cd ..
if exist build rmdir /Q /S build
mkdir build
cd build
cmake .. -G"MinGW Makefiles" %CMAKE_DEFINITIONS%
if "!errorlevel!" neq "0" endlocal & exit /b !errorlevel!
cmake --build .
if "!errorlevel!" neq "0" endlocal & exit /b !errorlevel!
cd ..\..
)

@echo all programs built successfully

@endlocal
