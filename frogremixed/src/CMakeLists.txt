find_package(small3d REQUIRED)

add_executable(frogremixed main.cpp Frog.hpp Frog.cpp Menu.hpp Menu.cpp Car.hpp
  Car.cpp Plank.hpp Plank.cpp)

target_link_libraries(frogremixed PRIVATE small3d::small3d)

if(MSVC)
  set_target_properties(frogremixed PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    "${PROJECT_BINARY_DIR}/bin")
endif()
