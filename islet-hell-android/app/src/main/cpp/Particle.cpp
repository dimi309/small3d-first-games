/*
 * Particle.cpp
 *
 *  Created on: 14 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "Particle.hpp"

Particle::Particle(uint32_t initLife,
  const glm::vec3& speed,
  const glm::vec3& speedModifier,
  const glm::vec4& colour) {
  this->speed = speed;
  this->speedModifier = speedModifier;
  this->colour = colour;
  lifeLeft = initLife;
}

void Particle::process() {
  position += speed;
  rotation += glm::vec3(0.4f, 0.4f, 0.4f);
  speed += speedModifier;
  --lifeLeft;
  if (lifeLeft == 0) {
    dead = true;
  }
}

bool Particle::isDead() {
  return dead;
}

void Particle::kill() {
  dead = true;
}