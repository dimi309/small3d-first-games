/*
 * Map.cpp
 *
 * Created on: 12 Oct 2019
 *     Author: Dimitri Kourkoulis
 *    License: BSD 3-Clause License (see LICENSE file)
 */

#include "Map.hpp"
#include <fstream>
#include <small3d/Logger.hpp>
#include <small3d/BasePath.hpp>

using namespace small3d;

#ifdef __ANDROID__
#include <streambuf>
#include <istream>
#include <android_native_app_glue.h>
struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
      this->setg(begin, begin, end);
    }
};
#endif

Map::Map(std::string filepath, ANativeActivity* activity) {

  region = new char[(1 + 2 * static_cast<uint64_t>(maxRegionRadius)) *
		    (1 + 2 * static_cast<uint64_t>(maxRegionRadius))];
  mapData.clear();
  std::string line;
#ifdef __ANDROID__
  AAsset* asset = AAssetManager_open(activity->assetManager,
                                     filepath.c_str(),
                                     AASSET_MODE_STREAMING);
  if (!asset) throw std::runtime_error("Opening asset " + filepath +
                                       " has failed!");
  off_t length;
  length = AAsset_getLength(asset);
  const void* buffer = AAsset_getBuffer(asset);
  membuf sbuf((char*)buffer, (char*)buffer + sizeof(char) * length);
  std::istream in(&sbuf);
  if (in) {
    while (std::getline(in, line)) {
#else

      std::ifstream mapfile(getBasePath() + filepath);

    if (file.is_open()) {
      while (std::getline(file, line)) {
#endif

        mapData.push_back(line);
        LOGDEBUG("Loaded: " + line);
      }
      LOGDEBUG("Loaded map.");
#ifndef __ANDROID__
      mapfile.close();
#endif
      xsize = static_cast<int>(mapData[0].size());
      ysize = static_cast<int>(mapData.size());
    }
  else {
    LOGERROR("Could not open map file.");
  }
  
}

Map::~Map() {
  if (region) delete[] region;
}


const char* Map::getRegion(int coordx, int coordy, uint32_t radius) {
  if (radius > maxRegionRadius) {
    throw std::runtime_error("getRegion given region radius exceeded maximum possible value.");
  }

  int diameter = 2 * radius + 1;

  for (int y = 0; y < diameter; ++y) {
    for (int x = 0; x < diameter; ++x) {
      int yout = coordy - radius + y;
      int xout = coordx - radius + x;
      if (yout < 0) {
        yout = static_cast<int>(mapData.size()) + yout;
      } 
      else if (yout >= mapData.size()) {
        yout = yout % static_cast<int>(mapData.size());
      }

      if (xout < 0) {
        xout = static_cast<int>(mapData[0].size()) + xout;
      }
      else if (xout >= mapData[0].size()) {
        xout = xout % static_cast<int>(mapData[0].size());
      }
      region[y * diameter + x] = mapData[yout][xout];
    }
  }

  return region;
}

char Map::getLocation(int coordx, int coordy) {
  if (coordy < 0) return ' ';
  if (coordx < 0) return ' ';
  if (coordy >= ysize) return ' ';
  if (coordx >= xsize) return ' ';
  return mapData[coordy][coordx];
}

int Map::getXsize() {
  return xsize;
}

int Map::getYsize() {
  return ysize;
}
