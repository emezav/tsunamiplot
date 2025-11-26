#include "gauges.h"
#include "geo.h"

using geo::Grid;
using geo::Strings;

bool loadGauges(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg)
{
  // First try loading gauges using lat/lon coordinates
  if (loadGaugesLatLon(path, gauges, x0, y0, rows, columns, dxDeg, dyDeg) == true)
  {
    return true;
  }

  // If that fails, try loading gauges using column/row coordinates
  if (loadGaugesColRow(path, gauges, x0, y0, rows, columns, dxDeg, dyDeg) == true)
  {
    return true;
  }

  return false;
}

bool loadGaugesLatLon(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg)
{
  string data = geo::Strings::loadFile(path);
  // Empty file?
  if (!data.length())
  {
    return false;
  }

  vector<string> lines = geo::Strings::splitLines(data);
  for (auto &line : lines)
  {
    line = geo::Strings::trim(line);
    if (!line.length() || line[0] == '#')
    {
      continue;
    }
    std::istringstream iss(line);
    vector<string> gaugeData = geo::Strings::split(line);
    if (gaugeData.size() >= 3)
    {
      double lon = atof(gaugeData[0].c_str());
      double lat = atof(gaugeData[1].c_str());
      auto [col, row] = Grid::position(lon, lat, x0, y0, rows, columns, dxDeg, dyDeg);
      if (col != -1 && row != -1)
      {
        gauges.column.push_back(col);
        gauges.row.push_back(row);
        gauges.longitude.push_back(lon);
        gauges.latitude.push_back(lat);
        gauges.name.push_back(gaugeData[2]);
      }
      else
      {
#ifdef _DEBUG
        cerr << "Warning: Gauge " << gaugeData[2] << " at (" << lon << ", " << lat << ") is out of grid bounds and will be ignored." << endl;
#endif
      }
    }
  }

  return gauges.size() ? true : false;
}

bool loadGaugesColRow(string path, Gauges &gauges, double x0, double y0, int rows, int columns, double dxDeg, double dyDeg)
{
  string data = geo::Strings::loadFile(path);
  // Empty file?
  if (!data.length())
  {
    return false;
  }

  vector<string> lines = geo::Strings::splitLines(data);
  for (auto &line : lines)
  {
    line = geo::Strings::trim(line);
    if (!line.length() || line[0] == '#')
    {
      continue;
    }
    std::istringstream iss(line);
    vector<string> gaugeData = geo::Strings::split(line);
    if (gaugeData.size() >= 3)
    {
      int col = atoi(gaugeData[0].c_str());
      int row = atoi(gaugeData[1].c_str());
      if (col >= 0 && col <= columns && row >= 0 && row <= rows)
      {
        auto [lon, lat] = Grid::coordinates(col, row, x0, y0, dxDeg, dyDeg);
        gauges.column.push_back(col);
        gauges.row.push_back(row);
        gauges.longitude.push_back(lon);
        gauges.latitude.push_back(lat);
        gauges.name.push_back(gaugeData[2]);
      }
      else
      {
#ifdef _DEBUG
        cerr << "Warning: Gauge " << gaugeData[2] << " at (" << col << ", " << row << ") is out of grid bounds and will be ignored." << endl;
#endif
      }
    }
  }

  return true;
}
