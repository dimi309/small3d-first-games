set -e

if [ "$1" != "Debug" ] && [ "$1" != "Release" ]; then
    echo "Please indicate build type: Debug or Release, followed by opengl if you would like to also prepare OpenGL-related libraries."
    exit 1
fi

if [ ! -d "small3d/build" ]; then
   cd small3d
   cd scripts
   ./build.sh $1 $2
   cd ../..
fi

export CMAKE_DEFINITIONS=-DCMAKE_BUILD_TYPE=$1

for sampledir in avoidthebug chasethegoat frogremixed serial-model-renderer
do

    if [ "$2" == "opengl" ] && [ "$sampledir" == "ball" ]; then
	echo "Not building ball sample for OpenGL"
	continue
    fi
    cd $sampledir
    if [ -d "deps" ]; then rm -rf deps; fi
    mkdir deps
    cd deps
    for depdir in include lib shaders
    do
	cp -rf ../../small3d/build/$depdir .
    done
    cp -rf ../../small3d/cmake .
    cd ..
    if [ -d "build" ]; then rm -rf build; fi
    mkdir build
    cd build
    cmake .. $CMAKE_DEFINITIONS
    cmake --build .
    cd ../..
done

echo "all programs built successfully"


