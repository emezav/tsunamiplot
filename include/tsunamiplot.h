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

  /**
   * @brief Plot inundation data
   * @param options Geo options
   */
  void plotInundation(geo::Options &options);

  /**
   * @brief Plot maximum velocity data with auto-scaled color palette
   * @param options Geo options
   */
  void plotVmax(geo::Options &options);

  /**
   * @brief Plot tsunami travel-time propagation grid with isolines every 2h
   * @param options Geo options
   */
  void plotPropagation(geo::Options &options);

  /**
   * @brief Plot bathymetry grid
   * @param options Geo options
   */
  void plotBathy(geo::Options &options);

  /**
   * @brief Render a scenario's elevation snapshot sequence (elevNNNNN /
   * elr<level>NNNN grids) as a titled/timestamped 2D flat-map video
   * @param options Geo options
   */
  void plotElevationAnimation2D(geo::Options &options);
}

#endif // TSUNAMIPLOT_H