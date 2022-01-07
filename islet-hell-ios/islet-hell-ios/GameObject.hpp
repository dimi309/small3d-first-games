/*
 * GameObject.hpp
 *
 *  Created on: 2 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */
#pragma once
#include "Settings.hpp"
#include "Map.hpp"
#include <small3d/SceneObject.hpp>
class GameObject : public small3d::SceneObject {

private:
  float totalRotX = 0.0f;
  float totalRotZ = 0.0f;
  glm::ivec2 sector;
  glm::ivec2 mapSize;
  
public:
  bool dead = false;
  bool sectorChanged = false;
  GameObject(const std::string name, const std::string modelPath, const std::string& modelMeshName, 
    const glm::ivec2& sector, const glm::ivec2& mapSize);
  GameObject(GameObject &obj, const glm::ivec2& sector);
  void positionOnMap(Map& map);
  glm::vec3 getGlobalPosition() const;
  glm::ivec2 getSector() const;
  void setSector(glm::ivec2);
  void rotateXZ(float x, float z);
  glm::vec3 globalDistanceXYZ(const GameObject&) const;
  float globalDistance(const GameObject&) const;
  glm::vec3 globalDistanceNormal(const GameObject&) const;
  float tripleProdDistOrient(const GameObject&) const;
  float dotDistOrient(const GameObject&) const;
  float getTotalRotX() const;
  float getTotalRotZ() const;
  void resetTotalRotX();
  void resetTotalRotZ();
};