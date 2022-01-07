/*
 * Enemy.hpp
 *
 *  Created on: 3 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */
#pragma once
#include "GameObject.hpp"
#include <memory>
class Enemy : public GameObject {
private:
  typedef enum class state_enum {turning_left, turning_right, going_up, going_down, flying_straight} state_enum;
  state_enum state = state_enum::turning_left;
  bool changingAltitude = false;
public:
  void act(const GameObject&);
  Enemy(GameObject& obj, const glm::ivec2& sector);
};
