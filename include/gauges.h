/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Gauge utilities header
 * @copyright Copyright (c) 2025
 */

#ifndef GAUGE_H
#define GAUGE_H

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * @brief Waveform gauge array
 */
struct Gauges
{
  vector<int> row;          /*!< Gauge location row*/
  vector<int> column;       /*!< Gauge location column */
  vector<double> longitude; /*!< Gauge location - longitude */
  vector<double> latitude;  /*!< Gauge location - latitude */
  vector<string> name;      /*!< Gauge name */

  /**
   * @brief Count of defined gauges
   * @return size_t Count of defined gauges
   */
  inline size_t size()
  {
    return row.size();
  }
};

/**
   * @brief Load gauges from a file
   *
   * @param path Path to the gauges file
   * @param gauges Output gauges object
   * @param x0 Lower left corner X coordinate for the grid
   * @param y0 Lower left corner Y coordinate for the grid
   * @param rows Number of rows in the grid
   * @param columns Number of columns in the grid
   * @param dxDeg Grid cell size in degrees (longitude)
   * @param dyDeg Grid cell size in degrees (latitude)
   * @return status true if gauges were loaded, false otherwise.
   */
  bool loadGauges(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg);

  /**
 * @brief Load gauges from a file
 *
 * @param path Path to the gauges file
 * @param gauges Output gauges object
 * @param x0 Lower left corner X coordinate for the grid
 * @param y0 Lower left corner Y coordinate for the grid
 * @param rows Number of rows in the grid
 * @param columns Number of columns in the grid
 * @param dxDeg Grid cell size in degrees (longitude)
 * @param dyDeg Grid cell size in degrees (latitude)
 * @return status true if gauges were loaded, false otherwise.
 */
bool loadGaugesLatLon(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg);

/**
 * @brief Load gauges from a file using column and row coordinates
 *
 * @param path Path to the gauges file
 * @param gauges Output gauges object
 * @param x0 Lower left corner X coordinate for the grid
 * @param y0 Lower left corner Y coordinate for the grid
 * @param rows Number of rows in the grid
 * @param columns Number of columns in the grid
 * @param dxDeg Grid cell size in degrees (longitude)
 * @param dyDeg Grid cell size in degrees (latitude)
 * @return status true if gauges were loaded, false otherwise.
 */
bool loadGaugesColRow(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg);

#endif