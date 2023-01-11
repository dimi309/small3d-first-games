/*
 *  GameLogic.cpp
 *
 *  Created on: 2014/11/09
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */


#define MAX_Z -1.0f
#define MIN_Z -24.0f
#define MAX_X 24.0f
#define MIN_X -24.0f

#define GROUND_Y -1.0f

#define BUG_TILT_SPEED 0.03f
#define BUG_ROTATION_SPEED 0.06f
#define BUG_SPEED 0.08f
#define BUG_START_ALTITUDE 2.6f

#define GOAT_ROTATION_SPEED 0.1f
#define GOAT_SPEED 0.05f

#define RESULT_TEXT_NAME "resutlText"

#include <memory>
#include <stdexcept>
#include "GameLogic.hpp"


using namespace small3d;

float sgn(float val) {
  return val < 0.0f ? -1.0f : 1.0f;
}

namespace ChaseTheGoat3D {

  GameLogic::GameLogic() :
    goat("goat", "resources/models/Goat/goatAnim",
	 19, 1, 3U),
    bug("bug", "resources/models/Bug/bugAnim", 9),
    bahSound("resources/sounds/bah.ogg") {

    renderer = &Renderer::getInstance("Chase the Goat 3D", 0, 0, 1.2f);

    renderer->createRectangle(startScreenRect, glm::vec3(-1.0f, 1.0f, 1.0f),
      glm::vec3(1.0f, -1.0f, 1.0f));
    renderer->createRectangle(msgRect,
      glm::vec3(-0.95f, -0.6f, -0.5f), glm::vec3(0.0f, -0.8f, -0.5f));
    renderer->createRectangle(skyRect, glm::vec3(-1.0f, 1.0f, 1.0f),
      glm::vec3(1.0f, -1.0f, 1.0f));
    renderer->createRectangle(groundRect, glm::vec3(-25.0f, GROUND_Y, MIN_Z),
      glm::vec3(25.0f, GROUND_Y, MAX_Z));

    Image startScreenTexture("resources/images/startScreen.png");
    renderer->generateTexture("startScreen", startScreenTexture);

    Image groundTexture("resources/images/grass.png");
    renderer->generateTexture("ground", groundTexture);

    Image goatTexture("resources/models/Goat/goat.png");
    renderer->generateTexture("goatTexture", goatTexture);

    bug.setFrameDelay(2);

    gameState = START_SCREEN;

    seconds = 0;

    lightModifier = -0.01f;

    goatState = WALKING_STRAIGHT;
  }

  GameLogic::~GameLogic() {
    renderer->clearBuffers(goat);
    renderer->clearBuffers(bug);
  }

  void GameLogic::initGame() {
    goat.position = glm::vec3(-1.2f, GROUND_Y, -4.0f);
    bug.position = glm::vec3(3.6f, GROUND_Y + BUG_START_ALTITUDE, -5.0f);
    bug.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    goat.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    bug.startAnimating();
    goat.startAnimating();

    startSeconds = glfwGetTime();

  }

  void GameLogic::moveGoat() {
    goatState = TURNING;

    float xDistance = goat.position.x - bug.position.x;
    float zDistance = goat.position.z - bug.position.z;
    float distance = sqrt(xDistance * xDistance + zDistance * zDistance);

    float goatRelX = xDistance / distance;
    float goatRelZ = zDistance / distance;

    float dotPosDir = goatRelX * bug.getOrientation().x + goatRelZ * bug.getOrientation().z; // dot product

    if (dotPosDir > 0.98f) {
      goatState = TURNING;
    }
    else
      goatState = WALKING_STRAIGHT;

    if (goatState == TURNING) {
      goat.rotate(glm::vec3(0.0f, GOAT_ROTATION_SPEED, 0.0f));

    }

    if (goat.position.z > MAX_Z) {
      goat.position.z = MAX_Z;
      goatState = TURNING;
    }
    if (goat.position.z < MIN_Z) {
      goat.position.z = MIN_Z;
      goatState = TURNING;
    }
    if (goat.position.x > MAX_X) {
      goat.position.x = MAX_X;
      goatState = TURNING;
    }
    if (goat.position.x < MIN_X) {
      goat.position.x = MIN_X;
      goatState = TURNING;
    }

    goat.position.x += goat.getOrientation().x * GOAT_SPEED;
    goat.position.z += goat.getOrientation().z * GOAT_SPEED;
    goat.position.y += goat.getOrientation().y * GOAT_SPEED;

    goat.animate();

  }

  void GameLogic::moveBug(const KeyInput& keyInput) {

    auto bugRotation = bug.getRotationXYZ();

    if (keyInput.left) {
      bugRotation.y += BUG_ROTATION_SPEED;
    }
    else if (keyInput.right) {
      bugRotation.y -= BUG_ROTATION_SPEED;
    }

    if (keyInput.down) {
      bugRotation.x += BUG_TILT_SPEED;

    }
    else if (keyInput.up) {
      bugRotation.x -= BUG_TILT_SPEED;
    }

    bug.setRotation(bugRotation);

    if (keyInput.space) {
      bug.position.x += bug.getOrientation().x * BUG_SPEED;
      bug.position.z += bug.getOrientation().z * BUG_SPEED;
      bug.position.y += bug.getOrientation().y * BUG_SPEED;
    }

    if (bug.position.y < GROUND_Y + 0.5f)
      bug.position.y = GROUND_Y + 0.5f;

    if (bug.position.z > MAX_Z)
      bug.position.z = MAX_Z;
    if (bug.position.z < MIN_Z)
      bug.position.z = MIN_Z;
    if (bug.position.x > MAX_X)
      bug.position.x = MAX_X;
    if (bug.position.x < MIN_X)
      bug.position.x = MIN_X;


    // Bug chase camera
    renderer->cameraPosition = bug.position;
    renderer->cameraPosition.x -= bug.getOrientation().x * 1.7f;
    renderer->cameraPosition.z -= bug.getOrientation().z * 1.7f;
    renderer->cameraPosition.y -= bug.getOrientation().y * 1.7f;
    renderer->setCameraRotation(bug.getRotationXYZ());
    if (renderer->cameraPosition.y < GROUND_Y + 1.0f)
      renderer->cameraPosition.y = GROUND_Y + 1.0f;

    bug.animate();

    if (goat.contains(bug.position)) {
      gameState = START_SCREEN;
      bahSound.play();
      seconds = (glfwGetTime() - startSeconds);
      renderer->generateTexture(RESULT_TEXT_NAME, "Goat not bitten for " + std::to_string(seconds) +
        " seconds", glm::vec3(0.5f, 1.0f, 0.0f));
    }
  }

  void GameLogic::processGame(const KeyInput& keyInput) {
    moveBug(keyInput);
    moveGoat();
  }

  void GameLogic::processStartScreen(const KeyInput& keyInput) {
    if (keyInput.enter) {
      initGame();
      gameState = PLAYING;
    }
  }

  void GameLogic::process(const KeyInput& keyInput) {
    switch (gameState) {
    case START_SCREEN:
      processStartScreen(keyInput);
      break;
    case PLAYING:
      processGame(keyInput);
      break;
    default:
      throw std::runtime_error("Urecognised game state");
    }
  }

  void GameLogic::render() {
    renderer->setBackgroundColour(glm::vec4(0.4f, 0.37f, 1.0f, 1.0f));

    if (gameState == START_SCREEN) {

      renderer->render(startScreenRect, glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "startScreen", false);

      if (seconds != 0) {
        renderer->render(msgRect, RESULT_TEXT_NAME, false);
      }

    }
    else {

      // Draw the background

      renderer->render(groundRect, "ground", true);

      renderer->render(goat, "goatTexture");
      
      renderer->render(bug, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    }
    renderer->swapBuffers();
  }

}
