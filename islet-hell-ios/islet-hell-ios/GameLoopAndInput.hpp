/*
 * GameLoopAndInput.hpp
 *
 *  Created on: 9 Oct 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#pragma once
// see https://stackoverflow.com/questions/15328751/android-macro-suddenly-not-defined
#if !defined(ANDROID) && !defined(__ANDROID__) && !defined(SMALL3D_IOS)
#include <GLFW/glfw3.h>
#endif

#include "KeyInput.hpp"
#if defined(ANDROID) || defined(__ANDROID__) || defined(SMALL3D_IOS)
#include <cstdint>
#endif

class GameLoopAndInput {
private:
  void (*process)(const KeyInput& keyInput) = nullptr;
  void (*render)() = nullptr;
  bool stop_ = false;
#if !defined(ANDROID) && !defined(__ANDROID__) && !defined(SMALL3D_IOS)
  GLFWwindow *window = nullptr;
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
    int mods);
#endif
  static KeyInput input;

public:
  GameLoopAndInput();
#if !defined(ANDROID) && !defined(__ANDROID__) && !defined(SMALL3D_IOS)
  GameLoopAndInput(GLFWwindow *window, void (*process_function)(const KeyInput&), void (*render_function)());
#endif
  void run(const uint32_t fps);
  void stop();
};

  
