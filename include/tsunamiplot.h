/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief  Tsunami plot utilities header
 * @copyright Copyright (c) 2025
 */

#ifndef TSUNAMIPLOT_H
#define TSUNAMIPLOT_H

#include "geo.h"
namespace TsunamiPlot
{

  /**
   * @brief Plot deformation data
   * @param options Geo options
   */
  void plotDeform(geo::Options &options);

  /**
   * @brief Plot maximum wave height data
   * @param options Geo options
   */
  void plotZMax(geo::Options &options);

  /**
   * @brief Plot wave profile data
   * @param options Geo options
   */
  void plotWaveProfile(geo::Options &options);
}

#endif // TSUNAMIPLOT_H