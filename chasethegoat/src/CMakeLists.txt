find_package(small3d REQUIRED)

add_executable(chasethegoat3d main.cpp GameLogic.cpp GameLogic.hpp KeyInput.hpp)

target_link_libraries(chasethegoat3d PRIVATE small3d::small3d)

if(MSVC)
  set_target_properties(chasethegoat3d PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    "${PROJECT_BINARY_DIR}/bin")
endif()
