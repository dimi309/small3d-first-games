/*
 * Particle.hpp
 *
 *  Created on: 14 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */
#pragma once

#include <glm/glm.hpp>

class Particle {
private:
  glm::vec3 speed;
  glm::vec3 speedModifier;
  bool dead = false;
  uint32_t lifeLeft;
public:
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec4 colour;

  Particle(uint32_t initLife, const glm::vec3& speed = glm::vec3(0.0f, 0.04f, 0.0f),
    const glm::vec3& speedModifier = glm::vec3(0.0f, -0.009f, 0.0f),
    const glm::vec4& colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  void process();
  bool isDead();
  void kill();
};