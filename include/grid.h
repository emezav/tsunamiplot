/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief  Grid utilities
 * @copyright Copyright (c) 2025
 */

 #ifndef GRID_H
 #define GRID_H

#include <filesystem>
#include <string>

#include "geo.h"
using geo::Grid;
namespace fs = std::filesystem;

 /**
   * @brief Loads a grid to memory.
   *
   * @param g Output grid object
   * @param path Path to the grid
   * @param options Options
   * @param convertToEsri If true, convert grid to Esri format
   * @return true if grid was loaded, false otherwise.
   */
  bool loadGrid(Grid &g, fs::path &gridPath, geo::Options &options, bool convertToEsri = false);

 #endif