/*
 * Plank.cpp
 *
 *  Created on: 2018/11/10
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "Plank.hpp"
#include <small3d/WavefrontFile.hpp>

Plank::Plank() :
  SceneObject("plank", small3d::Model(small3d::WavefrontFile("resources/models/plank/plank.obj"))) {
  colour = Vec4(0.5f, 0.3f, 0.3f, 1.0f);
}

Plank::~Plank() {
  if (renderer) {
    renderer->clearBuffers(static_cast<SceneObject &>(*this));
  }
}

void Plank::render(Renderer *renderer) {
  this->renderer = renderer;
  renderer->render(*this, colour);
  
}

void Plank::move() {
  this->position.x += this->speed;
}
