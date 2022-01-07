/*
 * GameObject.cpp
 *
 *  Created on: 2 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */
#include "GameObject.hpp"

GameObject::GameObject(const std::string name, const std::string modelPath, const std::string& modelMeshName, 
  const glm::ivec2& sector, const glm::ivec2& maxSector) :
  SceneObject(name, modelPath, modelMeshName)
{
  this->sector = sector;
  this->mapSize = maxSector;

}

GameObject::GameObject(GameObject& obj, const glm::ivec2& sector) : small3d::SceneObject(obj) {
  this->sector = sector;
  this->mapSize = obj.mapSize;
}

void GameObject::positionOnMap(Map& map) {
  sectorChanged = false;

  if (this->position.x < -sectorFloatRadius) {
    --sector.x;
    this->position.x = sectorFloatRadius;
    sectorChanged = true;
  }

  if (this->position.x > sectorFloatRadius ) {
    ++sector.x;
    this->position.x = -sectorFloatRadius;
    sectorChanged = true;
  }

  if (this->position.z < -sectorFloatRadius) {
    --sector.y;
    this->position.z = sectorFloatRadius;
    sectorChanged = true;
  }

  if (this->position.z > sectorFloatRadius) {
    ++sector.y;
    this->position.z = -sectorFloatRadius;
    sectorChanged = true;
  }

  if (sector.x < 0) {
    sector.x = map.getXsize() - 1;
  } else if (sector.x >= map.getXsize()) {
    sector.x = 0;
  } else if (sector.y < 0) {
    sector.y = map.getYsize() - 1;
  } else if (sector.y >= map.getYsize()) {
    sector.y = 0;
  }

  this->position = glm::clamp(this->position, glm::vec3(-sectorFloatRadius, seaLevel - 0.01f, -sectorFloatRadius), glm::vec3(sectorFloatRadius, 15.0f, sectorFloatRadius));
}

glm::vec3 GameObject::getGlobalPosition() const {
  return glm::vec3(sector.x * 2 * sectorFloatRadius + sectorFloatRadius + position.x, position.y, sector.y * 2 * sectorFloatRadius + sectorFloatRadius + position.z);
}

glm::ivec2 GameObject::getSector() const {
  return sector;
}

void GameObject::setSector(glm::ivec2 sector) {
  this->sector = sector;
}

void GameObject::rotateXZ(float x, float z) {
  totalRotX += x;
  if (totalRotX > 6.28f) {
    totalRotX -= 6.28f;
  }
  totalRotZ += z;
  if (totalRotZ > 6.28f) {
    totalRotZ -= 6.28f;
  }
  auto oldRot = this->getRotation();
  auto rotX = oldRot * glm::rotate(glm::mat4x4(1.0f), x, glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotZ = rotX * glm::rotate(glm::mat4x4(1.0f), z, glm::vec3(0.0f, 0.0f, 1.0f));
  this->setRotation(rotZ);
}

glm::vec3 GameObject::globalDistanceXYZ(const GameObject& other) const {
  auto sectorDistX = other.sector.x - this->sector.x;
  if (std::abs(sectorDistX) > mapSize.x / 2) {
    sectorDistX = -(sectorDistX / std::abs(sectorDistX)) * (mapSize.x - std::abs(other.sector.x));
  }

  auto sectorDistY = other.sector.y - this->sector.y;
  if (std::abs(sectorDistY) > mapSize.y / 2) {
    sectorDistY = -(sectorDistY / std::abs(sectorDistY)) * (mapSize.y - std::abs(other.sector.y));
  }

  return glm::vec3(sectorDistX * 2 * sectorFloatRadius + other.position.x - position.x, // + sectorFloatRadius cancelled out by subtraction
    other.position.y - position.y,
    sectorDistY * 2 * sectorFloatRadius + other.position.z - position.z); // + sectorFloatRadius cancelled out by subtraction
}

float GameObject::globalDistance(const GameObject& other) const {

  return glm::l2Norm(globalDistanceXYZ(other));

}

glm::vec3 GameObject::globalDistanceNormal(const GameObject& other) const {
  auto distance = globalDistanceXYZ(other);
  if (distance != glm::vec3(0.0f, 0.0f, 0.0f)) {
    return distance / glm::l2Norm(distance);
  }
  else {
    return glm::vec3(0.0f, 0.0f, 0.0f);
  }
}

float GameObject::tripleProdDistOrient(const GameObject& other) const{
  return dot(glm::vec3(0.0f, 1.0f, 0.0f), glm::cross(this->getOrientation(), this->globalDistanceNormal(other)));
}

float GameObject::dotDistOrient(const GameObject& other) const{
  return glm::dot(this->getOrientation(), this->globalDistanceNormal(other));
}

float GameObject::getTotalRotX() const {
  return totalRotX;
}
float GameObject::getTotalRotZ() const {
  return totalRotZ;
}

void GameObject::resetTotalRotX() {
  totalRotX = 0.0f;
}

void GameObject::resetTotalRotZ() {
  totalRotZ = 0.0f;
}