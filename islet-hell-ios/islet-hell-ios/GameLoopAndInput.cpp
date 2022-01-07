/*
 * GameLoopAndInput.cpp
 *
 *  Created on: 9 Oct 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "GameLoopAndInput.hpp"
#include <small3d/Logger.hpp>
#include <stdexcept>

KeyInput GameLoopAndInput::input;

GameLoopAndInput::GameLoopAndInput() {

}
#if !defined(ANDROID) && !defined(__ANDROID__) && !defined(SMALL3D_IOS)

void GameLoopAndInput::keyCallback(GLFWwindow* window, int key, int scancode, int action,
  int mods) {
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    input.down = true;
  if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    input.up = true;
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    input.left = true;
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    input.right = true;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    input.esc = true;
  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    input.enter = true;
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    input.space = true;
  if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    input.lshift = true;
  if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    input.down = false;
  if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
    input.up = false;
  if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
    input.left = false;
  if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
    input.right = false;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    input.esc = false;
  if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
    input.enter = false;
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    input.space = false;
  if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
    input.lshift = false;
}

GameLoopAndInput::GameLoopAndInput(GLFWwindow* window, void (*process_function)(const KeyInput&), void (*render_function)()) {
  if (!process_function) {
    throw std::runtime_error("process_function must be defined.");
  }
  if (!render_function) {
    throw std::runtime_error("render_function must be defined.");
  }
  process = process_function;
  render = render_function;
  this->window = window;
  glfwSetKeyCallback(this->window, keyCallback);
}

void GameLoopAndInput::run(const uint32_t fps) {

  try {
    auto seconds = glfwGetTime();
    auto prevSeconds = seconds;
    auto secondsInterval = 1.0 / fps;
    auto zpos = -15.1f, zrot = 0.0f, xpos = 0.5f;

    while (!glfwWindowShouldClose(window) && !stop_) {
      glfwPollEvents();

      seconds = glfwGetTime();
      if (seconds - prevSeconds > secondsInterval) {
        prevSeconds = seconds;
        process(input);
        render();
      }
    }
  }
  catch (std::runtime_error& e) {
    LOGERROR(e.what());
  }
}
#endif

void GameLoopAndInput::stop() {
  this->stop_ = true;
}