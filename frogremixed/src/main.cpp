/*
 * main.cpp
 *
 *  Created on: 2018/11/10
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include <small3d/Renderer.hpp>
#include <small3d/SceneObject.hpp>
#include <GLFW/glfw3.h>
#include "Frog.hpp"
#include "Menu.hpp"
#include "Car.hpp"
#include "Plank.hpp"
#include <small3d/Sound.hpp>
#include <small3d/WavefrontFile.hpp>

#define MAX_Z -1.0f
#define MIN_Z -24.0f
#define MAX_CAM_Z -3.0f
#define GROUND_Y -1.0f
#define MAX_X 17.0f
#define MIN_X -17.0f
#define MAX_CAM_X 3.0f
#define MIN_CAM_X -3.0f
#define TIME_TO_MAIN_MENU 2

using namespace small3d;

bool downkey, leftkey, rightkey, upkey, esckey, enterkey;

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
  int mods) {
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    downkey = true;
  if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    upkey = true;
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    leftkey = true;
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    rightkey = true;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    esckey = true;
  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    enterkey = true;
  if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    downkey = false;
  if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
    upkey = false;
  if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
    leftkey = false;
  if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
    rightkey = false;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    esckey = false;
  if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE)
    enterkey = false;

}


class Game {

private:

  const uint32_t framerate = 60;

  bool quitting = false;

  GLFWwindow* window;
  Renderer* renderer;

  

  std::shared_ptr<Frog> frog;
  Plank plank[3];
  Car car[2];
  std::shared_ptr<Menu> menu;

  std::shared_ptr<Sound> pondSound;
  std::shared_ptr<Sound> carSound;
  std::shared_ptr<Sound> drumSound;

  enum GameState { MENU, PLAYING, CRUSHED, DROWNED, WON };
  double timeToMainMenu = 0;
  double timeInMainMenu = 0;
  uint32_t wins = 0;
  uint32_t losses = 0;

  Model skyRect, terrainRect, msgRect, msgDrownedRect, 
    scoreRect;

  const std::string MSG_CRUSHED = "crushedTexture";
  const std::string MSG_DROWNED = "drownedTexture";
  const std::string MSG_WON = "wonTexture";
  const std::string MSG_SCORE = "scoreTexture";

  GameState gameState = MENU;

  void processInput() {

    if (gameState == PLAYING) {
      if (upkey && frog->position.z > MIN_Z)
        frog->position.z -= 0.1f;
      if (downkey && frog->position.z < MAX_Z)
        frog->position.z += 0.1f;
      if (leftkey && frog->position.x > MIN_X)
        frog->position.x -= 0.1f;
      if (rightkey && frog->position.x < MAX_X)
        frog->position.x += 0.1f;

      if (!(upkey || downkey || leftkey || rightkey) && frog->isJumping()) {
        frog->stopJumping();
      }
      else if ((upkey || downkey || leftkey || rightkey) && !frog->isJumping()) {
        frog->startJumping();
      }

      if (upkey) frog->setRotation(Vec3(0.0f, 0.0f, 0.0f));
      if (downkey) frog->setRotation(Vec3(0.0f, 3.14f, 0.0f));
      if (rightkey) frog->setRotation(Vec3(0.0f, -1.57f, 0.0f));
      if (leftkey) frog->setRotation(Vec3(0.0f, 1.57f, 0.0f));

      if (esckey) {
        gameState = MENU;
        timeInMainMenu = 0;
      }
    }
    else if (gameState == MENU) {
      if (enterkey) {
        gameState = PLAYING;
        frog->position = Vec3(0.0f, GROUND_Y, -4.0f);
        frog->setRotation(Vec3(0.0f, 0.0f, 0.0f));
      }

      if (esckey && timeInMainMenu > 1) quitting = true;

    }
  }

  void init() {

    renderer = &Renderer::getInstance("Frog Remixed");
    renderer->lightIntensity += 0.3f;

    menu = std::make_shared<Menu>(renderer);

    std::vector<std::shared_ptr<small3d::Model>> frogAnim;
    frogAnim.emplace_back(std::make_shared<Model>(WavefrontFile("resources/models/frog/frog_000001.obj")));
    frogAnim.emplace_back(std::make_shared<Model>(WavefrontFile("resources/models/frog/frog_000002.obj")));
    frogAnim.emplace_back(std::make_shared<Model>(WavefrontFile("resources/models/frog/frog_000003.obj")));
    frogAnim.emplace_back(std::make_shared<Model>(WavefrontFile("resources/models/frog/frog_000004.obj")));
    frogAnim.emplace_back(std::make_shared<Model>(WavefrontFile("resources/models/frog/frog_000005.obj")));

    frog = std::make_shared<Frog>(frogAnim);

    pondSound = std::make_shared<Sound>("resources/sounds/0245.ogg");
    carSound = std::make_shared<Sound>("resources/sounds/0600.ogg");
    drumSound = std::make_shared<Sound>("resources/sounds/0433.ogg");

    window = renderer->getWindow();

    Image terrainTexture("resources/images/terrain300x300.png");
    renderer->generateTexture("terrainTexture", terrainTexture);

    glfwSetKeyCallback(window, keyCallback);

    plank[0].position = Vec3(0.0f, -1.02f, -16.0f);
    plank[0].setRotation(Vec3(0.0f, 1.57f, 0.0f));
    plank[0].speed = 0.2f;

    plank[1].position = Vec3(0.0f, -1.02f, -17.0f);
    plank[1].setRotation(Vec3(0.0f, 1.57f, 0.0f));
    plank[1].speed = 0.1f;

    plank[2].position = Vec3(0.0f, -1.02f, -18.0f);
    plank[2].setRotation(Vec3(0.0f, 1.57f, 0.0f));
    plank[2].speed = 0.15f;

    car[0].position = Vec3(0.0f, -0.5f, -7.5f);
    car[0].setRotation(Vec3(0.0f, -1.57f, 0.0f));
    car[0].speed = -0.35f;

    car[1].position = Vec3(5.0f, -0.5f, -9.5f);
    car[1].setRotation(Vec3(0.0f, 1.57f, 0.0f));
    car[1].speed = 0.75f;

    renderer->createRectangle(skyRect, Vec3(-1.0f, 1.0f, 1.0f),
      Vec3(1.0f, -1.0f, 1.0f));

    renderer->createRectangle(terrainRect, Vec3(-35.0f, GROUND_Y, MIN_Z),
      Vec3(35.0f, GROUND_Y, MAX_Z));

    renderer->createRectangle(msgRect, Vec3(-0.4f, 0.8f, -1.0f),
      Vec3(0.4f, 0.4f, -1.0f));

    renderer->createRectangle(msgDrownedRect, Vec3(-0.8f, 0.8f, -1.0f),
      Vec3(0.8f, 0.4f, -1.0f));

    renderer->createRectangle(scoreRect, Vec3(-0.4f, -0.2f, 1.0f),
      Vec3(0.4f, -0.4f, 1.0f));

    renderer->generateTexture(MSG_CRUSHED,
      "YOU'VE BEEN CRUSHED", Vec3(0.0f, 1.0f, 0.0f));
    renderer->generateTexture(MSG_DROWNED,
      "YOU'VE HAVE DROWNED EVEN THOUGH YOU ARE A FROG", Vec3(0.0f, 1.0f, 0.0f));
    renderer->generateTexture(MSG_WON,
      "YOU HAVE WON!!!", Vec3(1.0f, 1.0f, 0.0f));

  }

  void processWorld() {
    if (car[0].position.x > MIN_X)  car[0].move();
    else car[0].position.x = MAX_X;

    if (car[1].position.x < MAX_X) car[1].move();
    else car[1].position.x = MIN_X;

    if (car[0].containsCorners(*frog) || car[1].containsCorners(*frog)) {
      frog->setRotation(Vec3(-1.57f, 0.0f, 0.0f));
      gameState = CRUSHED;
      ++losses;
      updateScoreMessage();
      timeToMainMenu = TIME_TO_MAIN_MENU;
      carSound->play();
    }

    if (frog->position.z < -15.5f && frog->position.z > -18.5f &&
      !plank[0].containsCorners(*frog) &&
      !plank[1].containsCorners(*frog) &&
      !plank[2].containsCorners(*frog)) {
      frog->position.y = GROUND_Y - 1.0f;
      gameState = DROWNED;
      ++losses;
      updateScoreMessage();
      timeToMainMenu = TIME_TO_MAIN_MENU;
      pondSound->play();
    }
    else {
      frog->position.y = GROUND_Y;
    }

    if (frog->position.z < -20.0f) {
      gameState = WON;
      ++wins;
      updateScoreMessage();
      timeToMainMenu = TIME_TO_MAIN_MENU;
      drumSound->play();
    }

    for (uint32_t idx = 0; idx < 3; ++idx) {
      if (plank[idx].position.x > MAX_X || plank[idx].position.x < MIN_X)
        plank[idx].speed = -plank[idx].speed;
    }

    plank[0].move();
    plank[1].move();
    plank[2].move();

    for (uint32_t idx = 0; idx < 3; ++idx) {
      if (plank[idx].containsCorners(*frog)) frog->position.x += plank[idx].speed;
    }

  }

  void renderGame() {
    

    // Follow the frog ...
    renderer->cameraPosition = frog->position;

    // But be a bit higher...
    renderer->cameraPosition.y += 13.0f;

    // ... and look down.
    renderer->setCameraRotation(Vec3(-1.0f, 0.0f, 0.0f));

    // Limit left and right position while following the frog->
    if (renderer->cameraPosition.x < MIN_CAM_X) renderer->cameraPosition.x =
      MIN_CAM_X;
    if (renderer->cameraPosition.x > MAX_CAM_X) renderer->cameraPosition.x =
      MAX_CAM_X;

    // Also be a bit behind the frog->
    renderer->cameraPosition.z += 4.7f;
    if (renderer->cameraPosition.z > MAX_CAM_Z) renderer->cameraPosition.z =
      MAX_CAM_Z;

    // Draw the sky
    renderer->render(skyRect, Vec4(0.4f, 0.37f, 1.0f, 1.0f), false);


    // Draw the ground
    renderer->render(terrainRect, "terrainTexture", true);

    renderer->render(*frog, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    plank[0].render(renderer);
    plank[1].render(renderer);
    plank[2].render(renderer);

    car[0].render(renderer);
    car[1].render(renderer);

    if (gameState == CRUSHED) {
      renderer->render(msgRect, MSG_CRUSHED, false);
    }

    if (gameState == DROWNED) {
      renderer->render(msgDrownedRect, MSG_DROWNED, false);
    }

    if (gameState == WON) {
      renderer->render(msgRect, MSG_WON, false);
    }

    renderer->swapBuffers();
  }

  void updateScoreMessage() {
    std::string winslosses = "Wins: ";
    winslosses += std::to_string(wins);
    winslosses += " Losses: ";
    winslosses += std::to_string(losses);
    renderer->generateTexture(MSG_SCORE, winslosses,  
      Vec3(1.0f, 1.0f, 1.0f));
  }

  void renderMenu() {
    renderer->setBackgroundColour(Vec4(0.0f, 0.3f, 1.0f, 1.0f));
    menu->render();
    if (wins > 0 || losses > 0) {
      renderer->render(scoreRect, MSG_SCORE, 0, false);
    }
    renderer->swapBuffers();
  }

public:

  ~Game() {
    renderer->clearBuffers(static_cast<SceneObject&>(*frog));
  }

  int run() {

    double seconds = glfwGetTime();
    double prevSeconds = seconds;
    double secondsInterval = 1.0 / framerate;

    init();

    while (!glfwWindowShouldClose(window) && !quitting) {

      glfwPollEvents();

      seconds = glfwGetTime();
      if (seconds - prevSeconds > secondsInterval) {

        processInput();

        switch (gameState) {
        case MENU:
          timeInMainMenu += seconds - prevSeconds;
          renderMenu();
          break;
        case PLAYING:
          processWorld();
          frog->animate();
          renderGame();
          break;
        case CRUSHED:
          renderGame();
          timeToMainMenu -= seconds - prevSeconds;
          if (timeToMainMenu < 0) gameState = MENU;
          break;
        case DROWNED:
          renderGame();
          timeToMainMenu -= seconds - prevSeconds;
          if (timeToMainMenu < 0) gameState = MENU;
          break;
        case WON:
          renderGame();
          timeToMainMenu -= seconds - prevSeconds;
          if (timeToMainMenu < 0) gameState = MENU;
          break;
        default:
          break;
        }

        prevSeconds = seconds;
      }
    }

    return 0;
  }

};

int main(int argc, char** argv) {
  Game game;
  return game.run();
}
