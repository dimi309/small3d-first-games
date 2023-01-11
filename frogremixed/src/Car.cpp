/*
 * Car.cpp
 *
 *  Created on: 2018/11/10
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "Car.hpp"
#include <string>
#include <small3d/WavefrontFile.hpp>

Car::Car() :
  SceneObject("car", "resources/models/car/car.obj") {
  WavefrontFile w("resources/models/car/wheel.obj");
  wheel = Model(w, "");
  this->colour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

Car::~Car() {
  if (renderer) {
    renderer->clearBuffers(wheel);
    renderer->clearBuffers(static_cast<SceneObject &>(*this));
  }
}

void Car::render(Renderer *renderer) {
  this->renderer = renderer;
  renderer->render(*this, colour);

  renderer->render(this->wheel,
		   glm::vec3(this->position.x + (this->getRotationXYZ().y > 0.0f ? 1.4f : 1.9f),
			     this->position.y,
			     this->position.z + 1.0f),
		   this->getRotationXYZ(),
		   glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

  renderer->render(this->wheel,
		   glm::vec3(this->position.x - (this->getRotationXYZ().y > 0.0f ? 1.9f : 1.4f),
			     this->position.y,
			     this->position.z + 1.0f),
		   this->getRotationXYZ(),
		   glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		   
  
}
   
void Car::move() {
  this->position.x += this->speed;
}
