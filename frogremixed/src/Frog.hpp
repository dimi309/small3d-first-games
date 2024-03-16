/*
 * Frog.hpp
 *
 *  Created on: 2018/11/10
 *      Author: Dimitri Kourkoulis
 *     License: BSD 3-Clause License (see LICENSE file)
 */

#include <small3d/SceneObject.hpp>
#include <small3d/Model.hpp>

enum FrogState {STILL, JUMPING};

class Frog : public small3d::SceneObject {
 private:
  FrogState State;
  
 public:  
  Frog(std::vector<std::shared_ptr<small3d::Model>> froganim);
  ~Frog();
  void startJumping();
  void stopJumping();
  bool isJumping();
};
