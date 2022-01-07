/*
 * Enemy.cpp
 *
 *  Created on: 3 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include "Enemy.hpp"

Enemy::Enemy(GameObject& obj, const glm::ivec2& sector) : GameObject(obj, sector) {

}

void Enemy::act(const GameObject& other) {

  auto yDir = other.position.y - this->position.y;
  auto pitch = this->getTotalRotX();

  if (std::abs(yDir) > 1.7f) {
    changingAltitude = true;
    if (std::abs(this->getTotalRotZ()) > 0.01f) { // there is roll
      this->rotateXZ(0.0f, -planeTurnSpeed * this->getTotalRotZ() / std::abs(this->getTotalRotZ())); // reverse it at rotation speed
      if (std::abs(this->getTotalRotZ()) <= 0.01f) {
        this->resetTotalRotX();
      }
    }
    else { // there is no roll
      yDir /= std::abs(yDir); // normalise altitude difference
      if (std::abs(pitch)  < 2.0f) { 
        this->rotateXZ(yDir * planeTurnSpeed, 0.0f); // change pitch at plane spid
      }
    }
  }
  else if (std::abs(pitch) > 0.01f && changingAltitude) { // not much altitude difference
    this->rotateXZ(-planeTurnSpeed * pitch / std::abs(pitch), 0.0f); // decrease pitch
    if (std::abs(this->getTotalRotX()) < 0.01f) {
      changingAltitude = false;
    }
  }
  
  if (!changingAltitude) {
    if (this->dotDistOrient(other) < 0.99f) { // other object not in front of this object
      if (this->tripleProdDistOrient(other) > 0) { // which way to turn?
        if (this->getTotalRotZ() < 1.57f) { // if not enough roll
          this->rotateXZ(0.0f, planeTurnSpeed); // change roll
        }
        else {
          this->rotateXZ(planeTurnSpeed, 0.0f); // if enough roll change pitch to turn
        }
      }
      else {
        if (this->getTotalRotZ() > -1.57f) { // if not enough roll in the other direction
          this->rotateXZ(0.0f, -planeTurnSpeed); // change roll
        }
        else {
          this->rotateXZ(planeTurnSpeed, 0.0f); // if enough roll change pitch to turn
        }
      }
    }
    else if (std::abs(this->getTotalRotZ()) > 0.01f) { // if other object is in front of this object and there is roll
      this->rotateXZ(0.0f, -planeTurnSpeed * this->getTotalRotZ() / std::abs(this->getTotalRotZ())); // decrease roll
      if (std::abs(this->getTotalRotZ()) <= 0.01f) {
        this->resetTotalRotX();
      }
    }
  }
  this->position += this->getOrientation() * enemySpeed; // move forward
}