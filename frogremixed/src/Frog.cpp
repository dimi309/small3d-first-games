/*
 * Frog.hpp
 *
 *  Created on: 2018/11/10
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "Frog.hpp"

Frog::Frog(std::vector<std::shared_ptr<small3d::Model>> froganim) : SceneObject("frog", froganim, 5) {
  this->State = STILL;
  this->setFrameDelay(4);
}

Frog::~Frog() {
}

void Frog::startJumping() {
  this->State = JUMPING;
  this->startAnimating();
}

void Frog::stopJumping() {
  this->State = STILL;
  this->stopAnimating();
}

bool Frog::isJumping() {
  return this->State == JUMPING;
}

