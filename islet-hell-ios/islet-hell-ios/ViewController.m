//
//  ViewController.m
//  islet-hell-ios
//
//  Created by me on 07/01/2022.
//

#import "ViewController.h"

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "interop.h"
#include "flags.h"
#include <small3d/Logger.hpp>

extern "C" {
#include "vkzos.h"
}

#include "Settings.hpp"
#include <small3d/Renderer.hpp>
#include <small3d/Model.hpp>
#include <small3d/SceneObject.hpp>
#include <small3d/Sound.hpp>
#include "GameLoopAndInput.hpp"
#include <exception>
#include <string>
#include <memory>
#include "OctPyramid.hpp"
#include "Map.hpp"
#include "GameObject.hpp"
#include "Enemy.hpp"
#include <glm/gtx/string_cast.hpp>
#include "Pyramid.hpp"
#include <random>
#include "Particle.hpp"

using namespace std;

void process(const KeyInput& keyInput);
void render();
void renderEnv();
void renderControls();
void write(std::string text, float elevation = 0.0f);
void genParticles(const glm::vec3& position,
                  const uint32_t life,
                  const glm::vec3& initShrapnelSpeed,
                  const uint32_t initShrapnelDensity,
                  const glm::vec3& speedModifier,
                  const glm::vec4& colour,
                  const bool alternate);
void explode(const glm::vec3& position, const glm::vec3& initSpeed);
void smoke(const glm::vec3& position);
bool splash(GameObject& obj);
void shoot(const glm::vec3& position, const glm::vec3& direction);

void initGame();

bool playing = false;
bool firstRound = true;
double timeToEnd = -1.0;
double prevTime = -1.0;

KeyInput keyInput;

GameLoopAndInput fs;
std::shared_ptr<Map> map;

float screenWidth = 0.0f, screenHeight = 0.0f;

std::vector<GameObject> planes;

OctPyramid islet(5.0f, 5.0f), sea(0.0f, 500.0f);

small3d::Model indicator;
small3d::Model radar;
small3d::Model spot;
small3d::Model button;

std::mt19937 gen;
std::uniform_int_distribution<> distrX;
std::uniform_int_distribution<> distrY;
std::uniform_int_distribution<> distrZ;
std::uniform_int_distribution<> distrEq;
std::vector<Particle> particles;

Pyramid particle(0.1f);

small3d::Renderer *r;
glm::vec3 rotation = glm::vec3(0.0f);
float sea_y = seaLevel;
float waveSpeed = -0.0001f;
float waveHeight = 0.03f;
std::unique_ptr<small3d::Sound> sndEngine; // Sometimes with VS this crashes if initialised in static memory here.
std::unique_ptr<small3d::Sound> sndGun;
std::unique_ptr <small3d::Sound> sndSplash;
std::unique_ptr <small3d::Sound> sndExplosion;

bool instantiated = false;

double currentTimeInSeconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  return (double) tv.tv_sec + tv.tv_usec / 1000000.0f;
}

void initGame() {
  for (auto& p : planes) {
    p.dead = false;
    p.position = glm::vec3(0.0f, 0.0f, 0.0f);
  }

  planes[0].setSector(glm::ivec2(4, 4));
  planes[1].setSector(glm::ivec2(3, 3));
  planes[2].setSector(glm::ivec2(3, 4));
  planes[3].setSector(glm::ivec2(3, 5));

  sndEngine->play(true);

}

void process(const KeyInput& keyInput) {

  if (keyInput.esc) {
    fs.stop();
    return;
  }

  if (!playing) {

    if (keyInput.enter) {

      playing = true;
      timeToEnd = -1.0;
      prevTime = -1.0;
      initGame();
      firstRound = false;
    }
    return;
  }

  if (timeToEnd != -1.0) {
    if (prevTime != -1.0) {
      timeToEnd -= currentTimeInSeconds() - prevTime;
    }
    prevTime = currentTimeInSeconds();

    if (timeToEnd < 0) {
      playing = false;
      timeToEnd = -1.0;
      prevTime = -1.0;
      particles.clear();
      sndEngine->stop();
    }

  }


  glm::vec3 rotation(0);

  if (keyInput.down) {
    rotation.x = planeTurnSpeed;
  }

  if (keyInput.up) {
    rotation.x = -planeTurnSpeed;
  }

  if (keyInput.left) {
    rotation.z = planeTurnSpeed;
  }

  if (keyInput.right) {
    rotation.z = -planeTurnSpeed;
  }

  if (keyInput.space) {
    sndGun->play();
    shoot(planes[0].position, planes[0].getOrientation());
  }
  else {
    sndGun->stop();
  }

  planes[0].position += planes[0].getOrientation() * planeSpeed;

  planes[0].positionOnMap(*map);

  if (planes[0].sectorChanged) {
    // Destroy all particles when changing sector,
    // otherwise they appear in the wrong place.
    for (auto& p : particles) {
      p.kill();
    }
  }

  planes[0].rotateXZ(rotation.x, rotation.z);

  sea_y += waveSpeed;
  if (sea_y > seaLevel + waveHeight || sea_y < seaLevel) waveSpeed = -waveSpeed;

  r->cameraPosition = planes[0].position;
  r->cameraPosition.x -= planes[0].getOrientation().x * 3.2f;
  r->cameraPosition.z -= planes[0].getOrientation().z * 3.2f;
  r->cameraPosition.y -= planes[0].getOrientation().y * 3.2f;
  r->setCameraRotation(planes[0].getRotation());
  r->cameraPosition.y += 0.9f;
  if (r->cameraPosition.y < seaLevel + 0.9f) {
    r->cameraPosition.y = seaLevel + 0.9f;
  }

  if (splash(planes[0])) {
    sndSplash->play();
  }
  else {
    sndSplash->stop();
  }

  bool foundOneAlive = false;

  for (auto plit = planes.begin() + 1; plit != planes.end(); ++plit) {

    if (!plit->dead) {
      foundOneAlive = true;
      static_cast<Enemy*>(&(*plit))->act(planes[0]);
      auto dot = plit->dotDistOrient(planes[0]);
      if (dot > 0.8f && plit->globalDistance(planes[0]) < 100.0f && plit->getSector() == planes[0].getSector()) { // cross-sector shooting for enemies
        // is not possible unless shot particles
        // become sector-aware
        shoot(plit->position, plit->getOrientation());
      }
    }

    plit->positionOnMap(*map);

    auto sectDiff = planes[0].getSector() - plit->getSector();
    plit->position -= glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);

    if (plit->dead && plit->position.y > seaLevel) {
      plit->position.y -= 0.1f;
    }

    if (planes[0].globalDistance(*plit) < 100.0f && planes[0].dotDistOrient(*plit) > 0.0f) {

      if (plit->dead) {
        smoke(plit->position);
      }
      else {
        splash(*plit);
      }
    }

    plit->position += glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);
  }

  if (!foundOneAlive && timeToEnd == -1.0f) {
    timeToEnd = endAfterSeconds;
  }

  auto region = map->getRegion(planes[0].getSector().x * 2 * sectorTileRadius + 1, planes[0].getSector().y * 2 * sectorTileRadius + 1, visibleTileRadius);
  for (int y = 0; y < visibleTileDiameter; ++y) {
    for (int x = 0; x < visibleTileDiameter; ++x) {
      if (region[y * visibleTileDiameter + x] == 'I') {
        glm::vec3 isletPos(-visibleTileRadius * tileSize + x * tileSize, seaLevel - 0.01f, -visibleTileRadius * tileSize + y * tileSize);

        bool exploded = false;
        while (glm::length(isletPos - planes[0].position) < isletTouchDistance) {
          if (!exploded && !planes[0].dead) explode(planes[0].position, planeSpeed * planes[0].getOrientation());
          exploded = true;
          planes[0].position.y += 0.001f;
          planes[0].dead = true;
          if (timeToEnd == -1.0f) {
            timeToEnd = endAfterSeconds;
          }
        }

        exploded = false;


        for (auto plit = planes.begin() + 1; plit != planes.end(); ++plit) {
          auto sectDiff = planes[0].getSector() - plit->getSector();
          plit->position -= glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);

          while (glm::length(isletPos - plit->position) < isletTouchDistance) {
            if (!plit->dead) {
              if (!exploded) explode(plit->position, enemySpeed * plit->getOrientation());
              exploded = true;
              plit->position.y += 0.001f;
              plit->dead = true;
            }
            else {
              plit->position -= 0.01f * plit->getOrientation();
            }
          }
          plit->position += glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);
        }
      }
    }
  }

  for (auto& p : particles) {
    p.process();
  }

  auto it = particles.begin();

  while (it != particles.end()) {
    if (it->isDead()) {
      it = particles.erase(it);
    }
    else {
      ++it;
    }
  }
}

void render() {
  if (!playing) {
    write("   ISLET HELL   ", 1.0f);
    write("another budget production by dimi309", 0.7f);
    if (!firstRound) {
      if (planes[0].dead) {
        write("* you have been destroyed *", 0.4f);
      }
      else {
        write("* All enemies destroyed. YOU WIN! *", 0.4f);
      }
    }
    write("Press enter to start.");
    r->swapBuffers();
    return;
  }

  r->render(planes[0], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

  // Checking bullet particle collisions while rendering, so that
  // we don't loop through them twice
  for (auto& p : particles) {
    if (p.colour.x == 0.0f) {
      r->render(particle, p.position, p.rotation, p.colour);
      if (planes[0].contains(p.position) && !planes[0].dead) {
        explode(planes[0].position, planeSpeed * planes[0].getOrientation());
        planes[0].dead = true;
        if (timeToEnd == -1.0f) {
          timeToEnd = endAfterSeconds;
        }
      }
      for (auto& pln : planes) {
        if (pln.contains(p.position)) {
          explode(pln.position, enemySpeed * pln.getOrientation());
          pln.dead = true;
        }
      }
    }
    else {
      r->render(particle, p.position, p.rotation, p.colour);
    }
  }

  auto planesLeftInList = planes.size() - 1;

  for (auto plit = planes.begin() + 1; plit != planes.end(); ++plit) {
    --planesLeftInList;
    if (!planes[0].dead && planes[0].getSector() == plit->getSector() && plit->containsCorners(planes[0])) {
      explode(planes[0].position, planeSpeed * planes[0].getOrientation());
      plit->dead = true;
      planes[0].dead = true;
      if (timeToEnd == -1.0f) {
        timeToEnd = endAfterSeconds;
      }
    }

    // Checking for plane collisions while rendering so that we do not loop
    // through them one more time

    for (auto lidx = 1; lidx <= planesLeftInList; ++lidx) {
      if (!plit->dead && plit->getSector() == (plit + lidx)->getSector() && plit->containsCorners(*(plit + lidx))) {
        explode(plit->position, enemySpeed * plit->getOrientation());
        plit->dead = true;
        (plit + lidx)->dead = true;
      }
    }


    auto dist = planes[0].globalDistance(*plit);
    if (dist < 100.0f && (planes[0].dotDistOrient(*plit) > 0.0f || dist < 10.0f)) { // close and (in front or very close)

      auto sectDiff = planes[0].getSector() - plit->getSector();

      plit->position -= glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);

      r->render(*plit, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

      plit->position += glm::vec3(sectDiff.x * 2 * sectorFloatRadius, 0.0f, sectDiff.y * 2 * sectorFloatRadius);
    }
  }

  // Rendering the environment after the plane ensures the sea transparency works on some platforms,
  // like MacOS.
  renderEnv();
  renderControls();

  auto planeOrnt = planes[0].getOrientation();
  float radarRot = 0;

  if (planeOrnt.x >= 0) {
    if (planeOrnt.z < 0) {  // facing "north", positive angle
      radarRot += -1.57f;
    }
    else {
      radarRot += 4.71f; // facing "south", negative angle
    }
  }
  else {
    if (planeOrnt.z < 0) {
      radarRot += 1.57f; // facing "north", negative angle
    }
    else {
      radarRot += -4.71f; // facing "south", positive angle
    }
  }

  if (planeOrnt.x != 0) {
    radarRot += std::atan(planeOrnt.z / planeOrnt.x);
  }
  else {
    radarRot += std::atan(planeOrnt.z / 0.000001f);
  }

  r->render(radar, glm::vec3(0.75f, -0.75f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "radar", false);

  for (auto plit = planes.begin() + 1; plit != planes.end(); ++plit) {
    if (!plit->dead && planes[0].globalDistance(*plit) < visibleRadius) {
      auto distScaled = planes[0].globalDistanceXYZ(*plit) / (5 * visibleRadius);
      r->render(spot, glm::vec3(0.75f, -0.75f, -1.00f),
                glm::rotate(glm::mat4(1.0f), radarRot, glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::translate(glm::mat4(1.0f), glm::vec3(-distScaled.x, distScaled.z, 0.0f)),
                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "spot", false);
    }
  }
  r->swapBuffers();
}

void renderEnv() {
  if (!playing) {
    return;
  }

  int diameter = 2 * visibleTileRadius + 1;

  auto region = map->getRegion(planes[0].getSector().x * 2 * sectorTileRadius + 1, planes[0].getSector().y * 2 * sectorTileRadius + 1, visibleTileRadius);

  for (int y = 0; y < diameter; ++y) {
    for (int x = 0; x < diameter; ++x) {
      if (region[y * diameter + x] == 'I') {
        r->render(islet, glm::vec3(-visibleTileRadius * tileSize + x * tileSize, seaLevel - 0.01f, -visibleTileRadius * tileSize + y * tileSize),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.2f, 0.7f, 0.0f, 1.0f));
      }
    }
  }
  // Rendering the sea last ensures the sea transparency works on some platforms, like MacOS.
  r->render(sea, glm::vec3(r->cameraPosition.x, sea_y, r->cameraPosition.z), glm::vec3(0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.3f));
}

void renderControls() {
    // left
    r->render(button, glm::vec3(-0.8f, -0.8f, 0.0f), glm::vec3(0.0f), glm::vec4(0.0f),
              keyInput.left ? "buttonOn":"buttonOff", false);
    // down
    r->render(button, glm::vec3(-0.6f, -0.8f, 0.0f), glm::vec3(0.0f), glm::vec4(0.0f),
              keyInput.down ? "buttonOn":"buttonOff", false);
    // right
    r->render(button, glm::vec3(-0.4f, -0.8f, 0.0f), glm::vec3(0.0f), glm::vec4(0.0f),
              keyInput.right ? "buttonOn":"buttonOff", false);
    // up
    r->render(button, glm::vec3(-0.6f, -0.6f, 0.0f), glm::vec3(0.0f), glm::vec4(0.0f),
              keyInput.up ? "buttonOn":"buttonOff", false);

    r->render(button, glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec4(0.0f),
              keyInput.space ? "buttonOn":"buttonOff", false);
}

void write(std::string text, float elevation) {
  std::string textureName = "indication" + std::to_string(elevation);
  r->generateTexture(textureName, text, glm::vec3(1.0f, 0.5f, 0.5f), 24);
  r->render(indicator, glm::vec3(0.0f, elevation, 0.0f), glm::mat4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), textureName, false);
}

void genParticles(const glm::vec3& position,
                  const uint32_t life,
                  const glm::vec3& initShrapnelSpeed,
                  const uint32_t initShrapnelDensity,
                  const glm::vec3& speedModifier,
                  const glm::vec4& colour,
                  const bool alternate) {

  auto s = initShrapnelSpeed;
  for (uint32_t idx = 0; idx < initShrapnelDensity; ++idx) {
    if (alternate) {
      s.x *= -1;
      s.z *= -1;
    }
    Particle p(life, s, speedModifier, colour);
    p.position = position;
    particles.insert(particles.end(), p);
  }
}

void explode(const glm::vec3& position, const glm::vec3& initSpeed) {
  for (int idx = 0; idx < 5; ++idx) {
    genParticles(position, 120, initSpeed +
                                glm::vec3(0.001f * distrEq(gen), 0.001f * distrEq(gen), 0.001f * distrEq(gen)),
                 30, glm::vec3(0.0f, -0.0001f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), false);
    sndExplosion->play();
  }
}

void smoke(const glm::vec3& position) {
  genParticles(position, 130,
               glm::vec3(0.0001f * distrEq(gen), 0.0001f * distrEq(gen), 0.0001f * distrEq(gen)),
               4, glm::vec3(0.0f, 0.001f, 0.0f), glm::vec4(0.3f, 0.3f, 0.3f, 0.8f), true);
}

bool splash(GameObject& obj) {
  bool splashed = false;
  for (auto& ext : obj.getBoundingBoxSetExtremes()) {

    glm::vec4 exLeftBottom(ext.minX, ext.minY, ext.maxZ, 1.0f);

    glm::vec4 exRightBottom(ext.maxX, ext.minY, ext.maxZ, 1.0f);

    exLeftBottom = obj.getRotation() * exLeftBottom;

    exRightBottom = obj.getRotation() * exRightBottom;

    const float initFoamSpeed = 0.0008f;
    const float initFoamDist = 0.02f;
    const uint32_t initFoamDensity = 10;

    if (exLeftBottom.y + obj.position.y <= seaLevel) {
      for (int idx = 0; idx < initFoamDensity; ++idx) {
        Particle p(10, glm::vec3(initFoamSpeed * distrX(gen), initFoamSpeed * distrY(gen), initFoamSpeed * distrZ(gen)));
        p.position = obj.position + glm::vec3(exLeftBottom) + glm::vec3(initFoamDist * distrX(gen), initFoamDist * distrY(gen), initFoamDist * distrZ(gen));
        p.position.y = seaLevel;
        particles.insert(particles.end(), p);
      }
      splashed = true;
    }

    if (obj.position.y - 0.05f <= seaLevel) {
      for (int idx = 0; idx < initFoamDensity; ++idx) {
        Particle p(10, glm::vec3(initFoamSpeed * distrX(gen), initFoamSpeed * distrY(gen), initFoamSpeed * distrZ(gen)));
        p.position = obj.position + glm::vec3(initFoamDist * distrX(gen), initFoamDist * distrY(gen), initFoamDist * distrZ(gen));
        p.position.y = seaLevel;
        particles.insert(particles.end(), p);
      }
      splashed = true;
    }

    if (exRightBottom.y + obj.position.y <= seaLevel) {
      for (int idx = 0; idx < initFoamDensity; ++idx) {
        Particle p(10, glm::vec3(initFoamSpeed * distrX(gen), initFoamSpeed * distrY(gen), initFoamSpeed * distrZ(gen)));
        p.position = obj.position + glm::vec3(exRightBottom) + glm::vec3(initFoamDist * distrX(gen), initFoamDist * distrY(gen), initFoamDist * distrZ(gen));
        p.position.y = seaLevel;
        particles.insert(particles.end(), p);
      }
      splashed = true;
    }
  }
  return splashed;
}
void shoot(const glm::vec3& position, const glm::vec3& direction) {
  genParticles(position + 0.5f * direction, 100, bulletSpeed * direction, 1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), false);
}

void touched(int x, int y) {
  if (app_active) {
       
       if (screenWidth != 0 && screenHeight != 0) {
         float xval = static_cast<float>(x) / screenWidth;
         float yval = static_cast<float>(y) / screenHeight;

         if (yval > 0.9f) {
           if (xval < 0.1f) {
             keyInput.left = true;
           } else if (xval < 0.2f) {
             keyInput.down = true;
           } else if (xval < 0.3f) {
             keyInput.right = true;
           }
         } else if (yval > 0.8f) {
           if (xval > 0.1f && xval < 0.2f) {
             keyInput.up = true;
           }
         }

         if (yval > 0.45f && yval < 0.55f && xval > 0.9f) {
             keyInput.space = true;
         }
       }
}
}
@implementation ViewController {
  CADisplayLink* _displayLink;
}

- (void)viewDidLoad {
  [super viewDidLoad];
  
  
  // Do any additional setup after loading the view.
  app_window = (__bridge void*) self.view.layer;
  
  small3d::initLogger();
    

    try {
      map = make_shared<Map>("resources1/map.txt");
      sndEngine = std::make_unique<small3d::Sound>("resources1/engine.ogg");
      sndGun = std::make_unique<small3d::Sound>("resources1/gun.ogg");
      sndSplash = std::make_unique<small3d::Sound>("resources1/splash.ogg");
      sndExplosion = std::make_unique <small3d::Sound> ("resources1/explosion.ogg");

      planes.push_back(GameObject("plane", "resources1/plane.glb", "Sphere", glm::ivec2(4, 4),
                                  glm::ivec2(map->getXsize(), map->getYsize())));

      planes.push_back(Enemy(planes[0], glm::ivec2(3, 3)));
      planes.push_back(Enemy(planes[0], glm::ivec2(3, 4)));
      planes.push_back(Enemy(planes[0], glm::ivec2(3, 5)));

      std::random_device rd;
      gen = std::mt19937(rd());
      distrX = std::uniform_int_distribution<>(-10, 10);
      distrY = std::uniform_int_distribution<>(1, 100);
      distrZ = std::uniform_int_distribution<>(-10, 10);

      distrEq = std::uniform_int_distribution<>(-50, 50);

      planes[0].position = glm::vec3(-sectorFloatRadius, 0.0f, -sectorFloatRadius);
      
      
      r = &small3d::Renderer::getInstance("Islet Hell", 854, 480, 0.785f, 1.0f, 24.0f, "resources1/shaders/", 5000);

                r->lightDirection.z = 0.0f;
                r->setBackgroundColour(glm::vec4(0.3f, 0.3f, 7.0f, 1.0f));
                r->createRectangle(indicator, glm::vec3(-0.4f, -0.4f, 0.1f),
                                   glm::vec3(0.4f, -0.50f, 0.1f));
                r->createRectangle(radar, glm::vec3(-0.25f, 0.25f, 0.1f), glm::vec3(0.25f, -0.25f, 0.1f));
                r->generateTexture("radar", small3d::Image("resources1/radar.png"));
                r->createRectangle(spot, glm::vec3(-0.005f, 0.005f, 0.0f),
                                   glm::vec3(0.005f, -0.005f, 0.0f));
                r->generateTexture("spot", small3d::Image("resources1/spot.png"));
                r->generateTexture("buttonOn", small3d::Image("resources1/buttonOn.png"));
                r->generateTexture("buttonOff", small3d::Image("resources1/buttonOff.png"));
                r->createRectangle(button, glm::vec3(-0.1f, 0.1f, 0.0f),
                                   glm::vec3(0.1f, -0.1f, 0.0f));

                instantiated = true;

    }
      catch (std::exception& e) {
        LOGERROR(std::string(e.what()));
      }

  screenWidth = r->getScreenWidth();
  screenHeight = r->getScreenHeight();
  
  uint32_t fps = 60;
  _displayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(renderLoop)];
  [_displayLink setFrameInterval: 60 / fps];
  [_displayLink addToRunLoop: NSRunLoop.currentRunLoop forMode: NSDefaultRunLoopMode];
  
}

- (void) viewDidAppear:(BOOL)animated {
  
}

- (void) viewWillDisappear:(BOOL)animated {
  
}

-(void) renderLoop {
  if (app_active) {
    process(keyInput);
    render();
  }
}



- (void)touchesBegan:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
  UITouch *touch = [[event allTouches] anyObject];
  CGPoint touchPoint = [touch locationInView:self.view];
  keyInput.enter = true;
  touched(touchPoint.x, touchPoint.y);
  //LOGDEBUG("touch begin " + intToStr(touchPoint.x) + " - " + intToStr(touchPoint.y));
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
  UITouch *touch = [[event allTouches] anyObject];
  CGPoint touchPoint = [touch locationInView:self.view];
  
  keyInput.enter = true;
  touched(touchPoint.x, touchPoint.y);
  //LOGDEBUG("touch move " + intToStr(touchPoint.x) + " - " + intToStr(touchPoint.y));
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches
           withEvent:(UIEvent *)event {
  keyInput = {};
}


@end

@implementation View
+(Class) layerClass { return [CAMetalLayer class]; }
@end



