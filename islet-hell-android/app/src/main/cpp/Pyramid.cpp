/*
 * Pyramid.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */
#include "Pyramid.hpp"

Pyramid::Pyramid(const float edgeLength) {

  // vertex 0 (top)
  this->vertexData.insert(vertexData.end(), { 0.0f, edgeLength / 2, 0.0f, 1.0f });

  // vertex 1
  glm::vec3 pos(edgeLength / 2, 0.0f, edgeLength / 4);
  this->vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z, 1.0f });

  glm::vec3 dir(0.0f, 0.0f, -1.0f);

  // vertex 2
  dir = glm::vec3(glm::rotate(glm::mat4x4(1.0f), 3.14159f / 6, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(dir, 1.0f));
  pos += edgeLength * dir;
  this->vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z, 1.0f });

  // vertex 3
  dir = glm::vec3(glm::rotate(glm::mat4x4(1.0f), 2 * 3.14159f / 3 , glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(dir, 1.0f));
  pos += edgeLength * dir;
  this->vertexData.insert(vertexData.end(), { pos.x, pos.y, pos.z, 1.0f });

  this->indexData.insert(indexData.end(), 
    { 0, 1, 2,
     0, 2, 3,
     0, 3, 1,
     3, 2, 1 });

  this->vertexDataByteSize = static_cast<uint32_t>(this->vertexData.size() * sizeof(float));
  this->indexDataByteSize = static_cast<uint32_t>(this->indexData.size() * sizeof(float));

}
