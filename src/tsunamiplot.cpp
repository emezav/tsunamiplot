/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Tsunami plot utilities
 * @copyright Copyright (c) 2025
 */

#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

#include "cmd.h"
#include "gauges.h"
#include "geo.h"
#include "grid.h"
#include "tsunamiplot.h"
#include "Timeseries.h"
#include "WaveData.h"

namespace TsunamiPlot
{

  namespace fs = std::filesystem;
  using geo::geoStatus;
  using geo::Grid;
  using geo::GridFormat;
  using geo::Strings;
  using std::string;
  using std::vector;

  /**
   * @brief Operation status
   *
   */
  enum class status : int
  {
    SUCCESS = 0,  /*!< OK */
    FAILURE = -1, /*!< Operation was not successful. */
  };

  // Forward declarations
  /**
   * @brief Get the Paths object
   * @param options Geo options
   * @param createOutput If true, create output directory if it does not exist
   * @return std::tuple<fs::path, fs::path> Input and ouput paths
   */
  std::tuple<fs::path, fs::path> getPaths(geo::Options &options, bool createOutput = false);

  /**
   * @brief Get the Point Palette
   * @return vector<string> Palette of RGB colors for points
   */
  vector<string> getPointPalette()
  {
    return {
        "255/0/0",     // Red
        "0/0/255",     // Blue
        "0/255/0",     // Green
        "255/255/0",   // Yellow
        "0/255/255",   // Cyan
        "255/0/255",   // Magenta
        "255/165/0",   // Orange
        "128/0/128",   // Purple
        "165/42/42",   // Brown
        "255/192/203", // Pink
        "50/205/50",   // Lime
        "0/128/128",   // Teal
        "173/216/230", // Light Navy
        "128/128/0",   // Olive
        "128/0/0"      // Maroon
    };
  }

  /**
   * @brief Get the palette color sequence
   * @return Palette of RGB colors for lines in wave graph
   */
  vector<string> getLinePalette()
  {
    return {
        "3/57/108",    // Blue
        "97/208/79",   // Green
        "223/83/107",  // Red
        "169/169/169", // Dark gray
        "252/145/58",  // Magenta
    };
  }

  /**
   * @brief Create a Max Palette File object
   * @return fs::path Path to the created palette file
   */
  fs::path createMaxPaletteFile()
  {
    fs::path palettePath = std::filesystem::temp_directory_path() / "max.cpt";

    std::ofstream ofs(palettePath.string());

    ofs << "# Max CPT file 0-7m\n"
           "# z-value  R  G  B    z-value   R  G  B\n"
           "0	0/0/127	0.1	0/0/241	L\n"
           "0.1	0/0/241	0.2	99/255/100	L\n"
           "0.2	99/255/100	0.3	0/212/255	L\n"
           "0.3	0/212/255	0.4	255/255/219	L\n"
           "0.4	255/255/219	0.6	255/255/163	L\n"
           "0.6	255/255/163	1.0	255/213/0	L\n"
           "1.0	255/213/0	2.0	255/9/0	L\n"
           "2.0	255/9/0	5.0	241/0/0	L\n"
           "5.0	241/0/0	7.0	127/0/0	B\n"
           "# Background color\n"
           "B	0/0/127\n"
           "# Foreground color\n"
           "F	255/255/255\n"
           "# NaN color\n"
           "N 255/255/255\n";

    ofs.close();

    return palettePath;
  }

  /**
   * @brief Create a Deform Plot Script
   * @param deformPath Path to deform grid
   * @param bathymetryPath Path to bathymetry grid
   * @param options Geo options
   * @return fs::path
   */
  fs::path createDeformPlotScript(string deformPath, string bathymetryPath, geo::Options &options)
  {

    // Get lang from options
    string lang = Strings::tolower(options.get("lang"));

    if (lang.length() == 0)
    {
      lang = "es";
    }

    string langUpper = Strings::toupper(static_cast<string>(lang));

    // Get deform filename
    fs::path deformFilename(deformPath);

#ifdef _DEBUG
    cout << "Creating deform plot script for " << deformFilename.string() << endl;
#endif

    // Set output path to deformPath with .png extension
    fs::path outputPath = fs::path(deformPath).replace_extension(".png");

    // Use filename from deformPath to create a script file inside fs::temp_directory_path()
    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    fs::path scriptPath = fs::temp_directory_path() / (fs::path(deformPath).stem().string() + "_deform" + fileExt);

    // Create batch file
    std::ofstream scriptOfs(scriptPath.string());
    // Send the commands to the batch file

    // Load bathymetry grid to get parameters
    Grid grid;
    fs::path bathymetryGridPath(bathymetryPath);

    if (loadGrid(grid, bathymetryGridPath, options) == false)
    {
      cerr << "Unable to load grid from " << bathymetryGridPath.string() << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    // Get grid parameters
    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    // Create extent string
    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

#ifdef WIN32
    // Set GMT_VERBOSE=quiet
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
    // Set GMT language
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;

#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
#endif

    // Create palettes on the temporary dir
    fs::path deformPalettePath = fs::temp_directory_path() / "deform_palette.cpt";
    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";

    fs::path deformFilenameWithoutExtension(deformFilename);
    deformFilenameWithoutExtension.replace_extension("");

    // Create a CPT file from the data on the deformation grid, -z for continuous, -D to match background and foreground for lowest and highest values
    scriptOfs << "gmt grd2cpt \"" << deformFilename.string() << "\"  -Cpolar -Z -D -A20+a > \"" << deformPalettePath.string() << "\" " << std::endl;

    // Create a CPT file for bathymetry using the globe base palette
    // Bathymetry grid must be positive for land and negative for sea, so only invert the palette if requested
    if (Strings::tolower(options.get("plot_invbat")) == "true" || options.get("plot_invbat") == "1")
    {
      // Invert the globe palette for bathymetry
      scriptOfs << "gmt makecpt -Cglobe -I -D > \"" << bathymetryPalettePath.string() << "\" " << std::endl;
    }
    else
    {
      scriptOfs << "gmt makecpt -Cglobe -D > \"" << bathymetryPalettePath.string() << "\" " << std::endl;
    }

    // If lang option is en, change title to English
    string title = "Deformacion";
    if (lang == "en")
    {
      title = "Deformation";
    }

    string source = options.get("source");
    if (source.length())
    {
      title += " - " + source;
    }

    string heightStr = "Altura (m)";
    // If lang option is set to "en", change heightStr to "Height (m)"
    if (lang == "en")
    {
      heightStr = "Height (m)";
    }

    // Start GMT session
    scriptOfs << "gmt begin \"" << deformFilenameWithoutExtension.string() << "\" png E600" << std::endl;

    // Set GMT option MAP_FRAME_TYPE to fancy+ for this session
    scriptOfs << "gmt set MAP_FRAME_TYPE fancy+" << std::endl;

    // Set GMT option MAP_FRAME_WIDTH for this session
    scriptOfs << "gmt set MAP_FRAME_WIDTH 2.5p" << std::endl;

    // Set GMT option GMT_LANGUAGE
    scriptOfs << "gmt set GMT_LANGUAGE " << langUpper << std::endl;

    // Plot earth relief as background (commented out, plot bathymetry grid instead)
    // scriptOfs << "gmt grdimage @earth_relief_01m -Cglobe -R\"" << deformFilename.string() << "\" -JM15 -Vq" << std::endl;

    // Plot bathymetry, framed, with title, deform grid overlayed, coastlines, and colorbar
    scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << bathymetryPath << "\"  -C\"" << bathymetryPalettePath.string() << "\" -Bafg -BWSen+t\"" << title << "\" --FONT_TITLE=12p --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;

    // Overlay deformation grid
    // -Q+z0 to make values of zero transparent
    scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << deformFilename.string() << "\"  -C\"" << deformPalettePath.string() << "\" -Q+z0 -t20 --FONT_TITLE=12p -Vq" << std::endl;

    // Draw coast lines
    // -W shorelines, -N1 country borders
    scriptOfs << "gmt coast -JM15c -R" << extentStr << " -Df -N1/0.01p,gray77 -W1/0.01p,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;

    // Calculate grid length in meters
    double gridLengthX = columns * dxM;
    double gridLengthY = rows * dyM;

    // Calculate the minimum grid length
    double gridLengthKm = std::min(gridLengthX, gridLengthY) / 1000.0;

#ifdef _DEBUG
    cout << "Grid length (km): " << gridLengthKm << endl;
#endif

    // Determine GMT scale string based on grid length. Scale must be approx, 1/5 of the minimum grid length
    float scaleLengthKm = trunc(gridLengthKm / 5.0f);
    if (scaleLengthKm < 1)
    {
      scaleLengthKm = 0.1;
    }

    // Round scaleLengthKm to 0.5, 5, 50, 500, 5000
    if (scaleLengthKm < 1)
    {
      scaleLengthKm = (scaleLengthKm < 0.5f) ? 0.5f : 1.0f;
    }
    else if (scaleLengthKm < 10)
    {
      scaleLengthKm = round(scaleLengthKm);
    }
    else if (scaleLengthKm < 100)
    {
      scaleLengthKm = round(scaleLengthKm / 5.0f) * 5.0f;
    }
    else if (scaleLengthKm < 1000)
    {
      scaleLengthKm = round(scaleLengthKm / 50.0f) * 50.0f;
    }
    else
    {
      scaleLengthKm = round(scaleLengthKm / 500.0f) * 500.0f;
    }

#ifdef _DEBUG
    cout << "Scale length (km): " << scaleLengthKm << endl;
#endif

    // Draw directional rose and scalebar
    scriptOfs << "gmt basemap -JM15c -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=6p,Helvetica --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=8p,Helvetica -Vq" << std::endl;

    // Draw color bar
    scriptOfs << "gmt colorbar  -JM15c -R" << extentStr << " -DjMR+w6c/0.5c+o-3c/0c+v -C\"" << deformPalettePath.string() << "\" -Bafg -Baf+l\"" << heightStr << "\" -Bxa1f1 -B+u\" m.\" --FONT_ANNOT_PRIMARY=6p --FONT_LABEL=6p -Vq" << std::endl;

    // End GMT session
    scriptOfs << "gmt end" << std::endl;

// Remove palette file
#ifndef _DEBUG
#ifdef WIN32
    scriptOfs << "del \"" << deformPalettePath.string() << "\"" << std::endl;
#elif __linux__
    scriptOfs << "rm \"" << deformPalettePath.string() << "\"" << std::endl;
#endif
#endif

#ifdef _DEBUG
    cout << "Palette path " << deformPalettePath.string() << endl;
#endif

    // Close batch script
    scriptOfs.close();

    // If linux, use fs::permissions to set the script as executable (only for the owner)
#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
#endif

#ifdef _DEBUG
    cout << "Script: " << scriptPath.string() << endl;
#endif

    return scriptPath;
  }

  /**
   * @brief Create a Max Plot Script object
   * @param zMaxPath Max Z grid path
   * @param waveDataPath Wave data path
   * @param gaugePath Gauge path
   * @param options Geo options
   * @return fs::path
   */
  fs::path createMaxPlotScript(string gridPath, string zMaxPath, string waveDataPath, string gaugePath, geo::Options &options)
  {

    fs::path palettePath = createMaxPaletteFile();

    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length())
    {
      lang = "es";
    }

    string langUpper = Strings::toupper(static_cast<string>(lang));

    string title = "Altura estimada de Tsunami";

    if (lang == "en")
    {
      title = "Tsunami Height Forecast";
    }

    if (options.contains("source"))
    {
      title += " - " + options.get("source");
    }

    // Load zMax grid to get parameters
    Grid grid;
    fs::path zMaxGridPath(zMaxPath);
    fs::path zMaxPlotPath = zMaxGridPath;

    // Load grid and convert to Esri format
    if (loadGrid(grid, zMaxGridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << zMaxGridPath.string() << endl;
      return fs::path();
    }

    // Get grid parameters
    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    bool tempZMaxGridCreated = false;
    // if zmax_nodata option is set, create a temporary grid and  replace nodata values
    if (!options.get("zmax_nodata").empty())
    {
      float noDataValue = std::stof(options.get("zmax_nodata"));

      for (size_t i = 0; i < rows; i++)
      {
        for (size_t j = 0; j < columns; j++)
        {
          if (grid(i, j) == noDataValue)
          {
            grid(i, j) = 100.0; // Replace NoData with 100m
          }
        }
      }

      grid.setNoData(100.0f);

      cout << "Replacing NoData in zmax grid with " << noDataValue << endl;
      // Save temporary grid to a new file
      fs::path tempZMaxGridPath = fs::temp_directory_path() / (zMaxGridPath.stem().string() + "_nodata_fixed.bil");
      if (geo::SaveGrid(grid, tempZMaxGridPath.string(), GridFormat::ESRI_FLOAT) == geo::geoStatus::FAILURE)
      {
        cerr << "Unable to save temporary grid to " << tempZMaxGridPath.string() << endl;
        return fs::path();
      }
      zMaxPlotPath = tempZMaxGridPath;
      tempZMaxGridCreated = true;
    }

#ifdef _DEBUG
    cout << "Creating zmax plot script for " << zMaxGridPath.string() << endl;
#endif

    // Set output path to gridPath with .png extension
    fs::path outputPath = zMaxGridPath.replace_extension(".png");

    // Set file extension
    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    // Use filename from gridPath to create a .bat file inside fs::temp_directory_path()
    fs::path scriptPath = fs::temp_directory_path() / (fs::path(zMaxPath).stem().string() + fileExt);

    // Generate legend from gauges
    fs::path legendFile = fs::temp_directory_path() / "max_legend.txt";

    // Create batch file
    std::ofstream scriptOfs(scriptPath.string());

    // Get wave data as Timeseries
    Timeseries pointData(waveDataPath);

    // Set to true if time adjustment is applied
    bool timeAdjusted = false;

    // Apply time_res factor to wave data. m=minutes (factor = 1), h=hours (factor=1/60)
    float timeResFactor = 1.0f;

    // Get time resolution from options (minutes or hours)
    string timeRes = Strings::tolower(options.get("time_res"));

    // Get plot mode from options (far or near)
    string eventType = Strings::tolower(options.get("event_type"));

    string timeUnits = "min";
    // Change resolution to hours if requested or if plot mode is far
    if (timeRes == "h" || timeRes == "hours" || eventType == "far")
    {
      timeResFactor = 1.0f / 60.0f;
      pointData = pointData.applyTimeFactor(timeResFactor);
      // Create a temporary wave data file with the adjusted time
      fs::path tempWaveDataPath = fs::temp_directory_path() / (fs::path(waveDataPath).stem().string() + "_time_adjusted.dat");
      // Save the adjusted wave data to the temporary file
      pointData.save(tempWaveDataPath.string());
      // Use the temporary wave data file for plotting
      waveDataPath = tempWaveDataPath.string();
      timeAdjusted = true;

      timeUnits = "hrs";
    }

    // Get the wave data from the time series
    // Use 0 for h_first_wave and delta_first_wave to get all waves
    WaveData waveData(pointData, 0.0, 0.0);

    // Get first wave and max height points
    vector<WavePoint> firstWave = waveData.firstWave();
    vector<WavePoint> maxHeight = waveData.maxHeight();

    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";

    // Send the commands to the batch file

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << ":: Auto-generated batch file to plot max grid " << zMaxPath << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
    // Set GMT_LANGUAGE
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;
#elif __linux__
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
#endif

    // Create a CPT file for bathymetry using the globe base palette
    // Bathymetry grid must be positive for land and negative for sea, so only invert the palette if requested
    if (Strings::tolower(options.get("plot_invbat")) == "true" || options.get("plot_invbat") == "1")
    {
      // Invert the globe palette for bathymetry
      scriptOfs << "gmt makecpt -Cglobe -I -D > \"" << bathymetryPalettePath.string() << "\" " << std::endl;
    }
    else
    {
      scriptOfs << "gmt makecpt -Cglobe -D > \"" << bathymetryPalettePath.string() << "\" " << std::endl;
    }

    // Create extent string
    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

    // Start GMT modern session
    scriptOfs << "gmt begin \"" << zMaxGridPath.replace_extension("").string() << "\" png E600" << std::endl;

    // If show_extent option is set to true, output the extent.
    if (Strings::tolower(options.get("show_extent")) == "true" || options.get("show_extent") == "1")
    {
      cout << "Plot extent: " << extentStr << endl;
    }

    // Set GMT option MAP_FRAME_TYPE to fancy+ for this session
    scriptOfs << "gmt set MAP_FRAME_TYPE fancy+" << std::endl;

    // Set GMT option MAP_FRAME_WIDTH for this session
    scriptOfs << "gmt set MAP_FRAME_WIDTH 2.5p" << std::endl;

    // Set GMT option GMT_LANGUAGE
    scriptOfs << "gmt set GMT_LANGUAGE " << langUpper << std::endl;

    // set PS_LINE_CAP to ROUND
    scriptOfs << "gmt set PS_LINE_CAP ROUND" << std::endl;

    // set PS_LINE_JOIN to ROUND
    scriptOfs << "gmt set PS_LINE_JOIN ROUND" << std::endl;

    // plot the legend file
    scriptOfs << "gmt legend " << legendFile << " -Rd -JX12c/9c -DjLT+w10c --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_TITLE=8p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    // Plot the grid - Offset Y by 12c to leave space for the legend
    // scriptOfs << "gmt grdimage -JM10c  -R\"" << gridPath << "\" \"" << gridPath << "\" -Cglobe -Y12c -Vq" << std::endl;

    // Plot bathymetry - Offset Y by 12C to leave space for the legend, override -R to match grid. Set transparency to 70 percent
    scriptOfs << "gmt grdimage -JM10c -R" << extentStr << " \"" << gridPath << "\"  -C\"" << bathymetryPalettePath.string() << "\" -Y11c -t70 -Vq" << std::endl;

    // Plot zmax grid
    scriptOfs << "gmt grdimage -JM10c  -R" << extentStr << " \"" << zMaxPlotPath.string() << "\" -C\"" << palettePath.string() << "\" -Qwhite -Vq" << std::endl;

    // Draw map frame and title
    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -Baf -B+t\"" << title << "\" --FONT_TITLE=14p,Helvetica --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;

    // Calculate grid length in meters
    double gridLengthX = columns * dxM;
    double gridLengthY = rows * dyM;

    // Calculate the minimum grid length
    double gridLengthKm = std::min(gridLengthX, gridLengthY) / 1000.0;

#ifdef _DEBUG
    cout << "Grid length (km): " << gridLengthKm << endl;
#endif

    // Determine GMT scale string based on grid length. Scale must be approx, 1/5 of the minimum grid length
    float scaleLengthKm = trunc(gridLengthKm / 5.0f);
    if (scaleLengthKm < 1)
    {
      scaleLengthKm = 0.1;
    }

    // Round scaleLengthKm to 0.5, 5, 50, 500, 5000
    if (scaleLengthKm < 1)
    {
      scaleLengthKm = (scaleLengthKm < 0.5f) ? 0.5f : 1.0f;
    }
    else if (scaleLengthKm < 10)
    {
      scaleLengthKm = round(scaleLengthKm);
    }
    else if (scaleLengthKm < 100)
    {
      scaleLengthKm = round(scaleLengthKm / 5.0f) * 5.0f;
    }
    else if (scaleLengthKm < 1000)
    {
      scaleLengthKm = round(scaleLengthKm / 50.0f) * 50.0f;
    }
    else
    {
      scaleLengthKm = round(scaleLengthKm / 500.0f) * 500.0f;
    }

#ifdef _DEBUG
    cout << "Scale length (km): " << scaleLengthKm << endl;
#endif

    // Draw coast lines
    // If outline option is set to true, plot outline file instead of coastlines
    if (options.get("outline").length() > 0)
    {
      fs::path outlinePath(options.get("outline"));
      if (!fs::exists(outlinePath))
      {
        cerr << "Warning! outline file " << outlinePath.string() << " does not exist." << endl;
      }
      else
      {
        // If file is binary plot with -bi2f
        if (Strings::tolower(outlinePath.extension().string()) == ".bin" || Strings::tolower(outlinePath.extension().string()) == ".bf2")
        {
          scriptOfs << "gmt plot \"" << fs::canonical(outlinePath).string() << "\" -bi2f -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
        else
        {
          // Plot as text file, add -J and -R to match the grid
          scriptOfs << "gmt plot -JM10c -R" << extentStr << " \"" << fs::canonical(outlinePath).string() << "\" -Wthinnest,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
      }
    }
    else
    {
      // Fall back to coastlines and country borders
      // -W shorelines, -N1 country borders
      scriptOfs << "gmt coast -JM10c -R" << extentStr << " -Df -N1/0.01p,gray77 -Wthinnest,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

    // Draw directional rose and scalebar
    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=6p,Helvetica --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    string heightStr = "Altura (m)";
    // If lang option is set to "en", change heightStr to "Height (m)"
    if (lang == "en")
    {
      heightStr = "Height (m)";
    }

    // Draw color bar
    scriptOfs << "gmt colorbar  -JM10c -R" << extentStr << " -DjMR+w6c/0.5c+o-3c/0c+v -C\"" << palettePath.string() << "\" -Bafg -Baf+l\"" << heightStr << "\" -Bxa1f1 -B+u\" m.\" --FONT_ANNOT_PRIMARY=6p --FONT_LABEL=8p -Vq" << std::endl;

    // Attempt to load gauges
    Gauges gauges;
    if (loadGauges(gaugePath, gauges, x0, y0, rows, columns, dxDeg, dyDeg) != true)
    {
      cerr << "Unable to load gauges from " << gaugePath << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    // Check if number of gauges matches number of wave data series
    if (gauges.size() != firstWave.size())
    {
      cerr << "Warning! number of gauges (" << gauges.size() << ") does not match number of wave data series (" << firstWave.size() << ")" << endl;
      cerr << "Maybe some new gauges were added after the wave data was generated." << endl;
    }

    std::ofstream legendOfs(legendFile.string());

    // Generate GMT table for gauges with the point palette
    vector<string> colors = getPointPalette();

    // Legend header
    if (lang == "en")
    {
      legendOfs << "H - Time and Height from Waveform" << endl;
    }
    else
    {
      legendOfs << "H - Tiempo y altura por mareograma" << endl;
    }
    // Define 6 columns
    legendOfs << "N 6" << endl;
    // Define a gap of 4 points between rows
    legendOfs << "G 4p" << endl;
    legendOfs << "D 0.5p 0.5" << endl;

    if (lang == "en")
    {
      legendOfs << "L - C Gauge" << endl;
      legendOfs << "L - L Location" << endl;
      legendOfs << "L - C T. First (" << timeUnits << ")" << endl;
      legendOfs << "L - C H. First (m)" << endl;
      legendOfs << "L - C T. Max (" << timeUnits << ")" << endl;
      legendOfs << "L - C H. Max (m)" << endl;
    }
    else
    {
      legendOfs << "L - C Punto" << endl;
      legendOfs << "L - L Ubicacion" << endl;
      legendOfs << "L - C T. Primero (" << timeUnits << ")" << endl;
      legendOfs << "L - C H. Primero (m)" << endl;
      legendOfs << "L - C T. Max (" << timeUnits << ")" << endl;
      legendOfs << "L - C H. Max (m)" << endl;
    }

    legendOfs << "G 4p" << endl;
    legendOfs << "D 0.5p 0.5" << endl;

    std::stringstream removeFilesSs;

    for (size_t i = 0; i < firstWave.size(); i++)
    {
      if (i >= gauges.size())
      {
        cerr << "Warning! Not enough gauges for wave data series index " << i << endl;
        break;
      }

      string color = colors[i % colors.size()];

      // Get first wave data
      float h_first_wave = firstWave[i].height;
      float t_first_wave = firstWave[i].time * timeResFactor;

      // Get max height data
      float h_max_wave = maxHeight[i].height;
      float t_max_wave = maxHeight[i].time * timeResFactor;

      // Output legend row for the gauge with its first wave data
      legendOfs << "S 0.5c c 0.2c " << color << " 0/0/0" << endl;
      legendOfs << "L - L " << Strings::replaceAll(gauges.name[i], "_", " ") << endl;
      // Plot t_first_wave and h_first_wave in any case

      legendOfs << "L - C " << std::fixed << std::setprecision(1) << t_first_wave << endl;
      legendOfs << "L - C " << std::fixed << std::setprecision(4) << h_first_wave << endl;
      legendOfs << "L - C " << std::fixed << std::setprecision(1) << t_max_wave << endl;
      legendOfs << "L - C " << std::fixed << std::setprecision(4) << h_max_wave << endl;

      // cout << "Gauge " << gauges.name[i] << ": First wave at " << t_first_wave << " " << timeUnits << " with height " << h_first_wave << " m. Max wave at " << t_max_wave << " " << timeUnits << " with height " << h_max_wave << " m." << endl;

      legendOfs << "G 4p" << endl;
      legendOfs << "D 0.5p 0.5" << endl;

      // Create a mark file for the gauge
      fs::path markFile = fs::temp_directory_path() / ("mark_" + std::to_string(i) + ".txt");
      std::ofstream markOfs(markFile.string());
      markOfs << gauges.longitude[i] << " " << gauges.latitude[i] << endl;
      markOfs.close();
      // cout << markFile << endl;

      // Line to plot the gauge mark
      // If event type is near, use bigger circles
      if (eventType == "near")
      {
        // Plot bigger circles
        scriptOfs << "gmt plot \"" << markFile.string() << "\" -Sc4p -W0.2p,black -G" << color << " -Vq" << std::endl;
      }
      else
      {
        // Plot smaller circles
        scriptOfs << "gmt plot \"" << markFile.string() << "\" -Sc0.8p -W0.1p,black -G" << color << " -Vq" << std::endl;
      }

#ifdef WIN32
      // Save the mark file to be removed later
      removeFilesSs << "del \"" << markFile.string() << "\"" << std::endl;
#elif __linux__
      // Save the mark file to be removed later
      removeFilesSs << "rm \"" << markFile.string() << "\"" << std::endl;
#endif
    }

    legendOfs.close();

    // End GMT modern session
    scriptOfs << "gmt end" << std::endl;

#ifndef _DEBUG
#ifdef WIN32
    if (tempZMaxGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "del \"" << zMaxPlotPath.string() << "\"" << std::endl;
    }
    // Remove palette file
    removeFilesSs << "del \"" << palettePath.string() << "\"" << std::endl;
    // Remove temporary legend file
    removeFilesSs << "del \"" << legendFile.string() << "\"" << std::endl;
#elif __linux__
    if (tempZMaxGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "rm \"" << zMaxPlotPath.string() << "\"" << std::endl;
    }
    // Remove palette file
    removeFilesSs << "rm \"" << palettePath.string() << "\"" << std::endl;
    // Remove temporary legend file
    removeFilesSs << "rm \"" << legendFile.string() << "\"" << std::endl;
#endif

    // Append remove commands to the batch file
    scriptOfs << removeFilesSs.str();
#endif

    // Close the batch file
    scriptOfs.close();

    // If linux, use fs::permissions to set the script as executable (only for the owner)
#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
#endif

// Show script path in debug mode
#ifdef _DEBUG
    cout << scriptPath.string() << endl;
#endif

    return scriptPath;
  }

  /**
   * @brief Create a Wave Plot Script
   *
   * @param gridPath Path to the grid file
   * @param waveDataPath Path to the wave data file
   * @param gaugePath Path to the gauge file
   * @param options Geo options
   * @return fs::path Path to the created script
   */
  fs::path createWavePlotScript(string zMaxPath, string waveDataPath, string gaugePath, geo::Options &options)
  {

    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length())
    {
      lang = "es";
    }

    // Create a copy of lang and transform to uppercase
    string langUpper = Strings::toupper(static_cast<string>(lang));

    fs::path gridFilename(zMaxPath);

    int seriesPerPlot = 4; // Number of series per plot
    if (options.contains("series_per_plot"))
    {
      seriesPerPlot = options.getInt("series_per_plot");
      if (seriesPerPlot < 1)
      {
        seriesPerPlot = 1;
      }
      else if (seriesPerPlot > 10)
      {
        seriesPerPlot = 10;
      }
    }

    if (!fs::exists(gridFilename) || !fs::is_regular_file(gridFilename))
    {
      cerr << "Grid file " << gridFilename.string() << " does not exist." << endl;
      return fs::path();
    }

    // Wave plot title
    string title = "Mareogramas";

    if (lang == "en")
    {
      title = "Wave profiles";
    }

    if (options.contains("source"))
    {
      title += " - " + options.get("source");
    }

    // Tile dimension: 12c/9c -XJ12c/9c
    // Legend width: 5c
    // Offset is negative from top left corner

    // Tile offsets for different number of tiles on the plot (vertical layout: 1, 2, 3 and 4 tiles)
    std::map<int, vector<string>> verticalLayout{
        {1, vector<string>{""}},                                    // No offset
        {2, vector<string>{"-Yc-13c", "-Yc"}},                      // 2 tiles
        {3, vector<string>{"-Yc-26c", "-Yc-13c", "-Yc"}},           // 3 tiles
        {4, vector<string>{"-Yc-39c", "-Yc-26c", "-Yc-13c", "-Yc"}} // 4 tiles
    };

    // Tile offsets for different number of tiles on the plot (horizontal layout: 1, 2, 3 and 4 tiles)
    std::map<int, vector<string>> horizontalLayout{
        {1, vector<string>{""}},                                    // No offset
        {2, vector<string>{"-Xc-16c", "-Xc"}},                      // 2 tiles
        {3, vector<string>{"-Xc-32c", "-Xc-16c", "-Xc"}},           // 3 tiles
        {4, vector<string>{"-Xc-48c", "-Xc-32c", "-Xc-16c", "-Xc"}} // 4 tiles
    };

    // Tile offsets for different number of tiles on the plot (grid layout: 1x1, 2x2, 3x3 and 4x4 tiles)
    std::map<int, vector<string>> gridLayout{
        {1, vector<string>{""}},                   // No offset
        {2, vector<string>{"-Xc-16c -Yc", "-Xc"}}, // 2 tiles
        {3, vector<string>{
                "-Xc-16c -Yc",
                "-Xc",
                "-Xc-16c -Yc-13c",
            }},                                                                         // 3 tiles
        {4, vector<string>{"-Xc-16c -Yc", "-Xc -Yc", "-Xc-16c -Yc-13c", "-Xc -Yc-13c"}} // 4 tiles
    };

    if (seriesPerPlot < 1 || seriesPerPlot > 4)
    {
      seriesPerPlot = 4; // Default to 4 series per plot
    }

    float hFirstWave = 0.3f;      // Minimum height to consider a first wave
    float deltaFirstWave = 15.0f; // Minimum time (minutes) to consider a first wave

    std::map<int, vector<string>> tileOffsets = gridLayout;

    int maxTilesPerImage = tileOffsets.size(); // Maximum number of tiles per image

    // Wave image path directory relative to wave data path
    fs::path waveImagePath = fs::path(waveDataPath).parent_path();

    // Set output path to gridPath with .png extension
    fs::path outputPath = fs::path(zMaxPath).replace_extension(".png");

    // Set file extension
    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    // Use filename from outputPath to create a .bat file inside fs::temp_directory_path()
    fs::path scriptPath = fs::temp_directory_path() / (fs::path(waveImagePath).stem().string() + "_wave" + fileExt);

    // Create batch file
    std::ofstream scriptOfs(scriptPath.string());

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << "REM Auto-generated batch file to plot wave data from " << waveDataPath << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "set GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "set GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;

#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    scriptOfs << "# Auto-generated shell file to plot wave data from " << waveDataPath << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
#endif

    // Get wave data as Timeseries
    Timeseries pointData(waveDataPath);

    // Apply time_res factor to wave data. m=minutes (factor = 1), h=hours (factor=1/60)
    float timeResFactor = 1.0f;

    // Get time resolution from options (minutes or hours)
    string timeRes = Strings::tolower(options.get("time_res"));

    // Get event type from options (far or near)
    string eventType = Strings::tolower(options.get("event_type"));

    string timeAxisLabel = "Tiempo (min.)";
    if (lang == "en")
    {
      timeAxisLabel = "Time (min.)";
    }

    bool timeAdjusted = false;
    // Change time resolution to hours if requested of if plot mode is far
    if (timeRes == "h" || timeRes == "hours" || eventType == "far")
    {
      timeResFactor = 1.0f / 60.0f;
      pointData = pointData.applyTimeFactor(timeResFactor);
      // Create a temporary wave data file with the adjusted time
      fs::path tempWaveDataPath = fs::temp_directory_path() / (fs::path(waveDataPath).stem().string() + "_time_adjusted.dat");
      // Save the adjusted wave data to the temporary file
      pointData.save(tempWaveDataPath.string());
      // Use the temporary wave data file for plotting
      waveDataPath = tempWaveDataPath.string();
      timeAdjusted = true;

      timeAxisLabel = "Tiempo (hr.)";
      if (lang == "en")
      {
        timeAxisLabel = "Time (hr.)";
      }
    }

    string waveHeightAxisLabel = "Altura (m)";
    if (lang == "en")
    {
      waveHeightAxisLabel = "Height (m)";
    }

    int seriesCount = pointData.series_count();

    // Get the wave data from the time series
    WaveData waveData(pointData, hFirstWave, deltaFirstWave);

    // Get first wave and max height points
    vector<WavePoint> firstWave = waveData.firstWave();
    vector<WavePoint> maxHeight = waveData.maxHeight();

    // Load zMax grid to get parameters
    Grid grid;
    fs::path zMaxGridPath(zMaxPath);

    if (loadGrid(grid, zMaxGridPath, options) == false)
    {
      cerr << "Unable to load grid from " << zMaxGridPath.string() << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    // Get grid parameters
    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();

#ifdef _DEBUG
    cout << "Resolution in meters: dx=" << dxM << " dy=" << dyM << endl;
#endif

    auto [rows, columns] = grid.dimensions();

    // Attempt to load gauges
    Gauges gauges;
    if (loadGauges(gaugePath, gauges, x0, y0, rows, columns, dxDeg, dyDeg) != true)
    {
      cerr << "Unable to load gauges from " << gaugePath << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    if (seriesCount != gauges.size())
    {
      cerr << "Number of series in wave data (" << seriesCount << ") does not match number of gauges (" << gauges.size() << ")" << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    // Get min and max values from the wave data
    auto [minIndex, minStep, minTime, zMin] = pointData.min_val();
    auto [maxIndex, maxStep, maxTime, zMax] = pointData.max_val();
    float finalTime = pointData.max_t();

    // Calculate max extent for x axis
    float maxExtentX = finalTime;

    if (maxExtentX < 1.0f)
    {
      maxExtentX = 1.0f;
    }

    // Tomar la altura maxima y multiplicarla por el 20% para la grafica
    float maxExtentY = zMax * 1.2f;

    // Limit max Y extent to 5.0f
    if (maxExtentY > 5.0f)
    {
      maxExtentY = 5.0f;
    }

    if (maxExtentY < 1.0f)
    {
      maxExtentY = 1.1f;
    }

    // Calculate extent for y axis
    float minExtentY = zMin - (fabsf(zMin) * 0.2f);
    if (minExtentY >= maxExtentY)
    {
      minExtentY = maxExtentY - 1.0f;
    }

    if (minExtentY > -0.5f)
    {
      minExtentY = -0.5f;
    }

    // Calculate delta for y axis (10 intervals)
    float deltaY = (maxExtentY - minExtentY) / 10.0f;

    // Divide y on 5 intervals
    float eDy = (maxExtentY - minExtentY) / 5.0f;

    eDy -= fmodf(eDy, 0.1f);

    // Divide x on 5 intervals
    int eDx = (int)floorf(maxExtentX / 5.0f);

    if (eDx > 10)
    {
      eDx -= eDx % 10;
    }
    else
    {
      eDx = 5;
    }

    /* Create wave extent string */
    string plotExtent = " -R0/" + std::to_string(maxExtentX) + "/" + std::to_string(minExtentY) + "/" + std::to_string(maxExtentY);

    // Calculate total the number of tiles
    int tileCount = seriesCount / seriesPerPlot;

    if (tileCount * seriesPerPlot < seriesCount)
    {
      tileCount++;
    }

    // Calculate the number of documents (images)
    int imageCount = tileCount / maxTilesPerImage;

    if (imageCount * maxTilesPerImage < tileCount)
    {
      imageCount++;
    }
    // Get array of palette colors
    vector<string> linePalette = getLinePalette();

    // Vector to hold legend file names
    vector<string> legendFiles;

    for (int imageNumber = 1; imageNumber <= imageCount; imageNumber++)
    {

      // Calculate start and end tile for the image
      int startTile = (imageNumber - 1) * maxTilesPerImage;
      int endTile = startTile + maxTilesPerImage;
      if (endTile > tileCount)
      {
        endTile = tileCount;
      }

      // Calculate tile count on this image
      int numberOfTiles = endTile - startTile;
      // cout << "Number of tiles on this image: " << numberOfTiles << endl;

      // Start GMT session for this image
      // Save as wave_{imageNumber}.png relative to waveImagePath
      scriptOfs << "gmt begin " << (waveImagePath / ("wave_" + std::to_string(imageNumber))).string() << " png E600" << std::endl;

      // set PS_LINE_CAP to ROUND
      scriptOfs << "gmt set PS_LINE_CAP ROUND" << std::endl;

      // set PS_LINE_JOIN to ROUND
      scriptOfs << "gmt set PS_LINE_JOIN ROUND" << std::endl;

      cout << (waveImagePath / ("wave_" + std::to_string(imageNumber))).string() << ".png" << endl;

      int currentTile = startTile;
      int seriesPlotted = 0;

      // Start plotting current image tiles
      for (int tileIndex = 0; tileIndex < numberOfTiles; tileIndex++)
      {

        // cout << "Plotting image " << imageNumber << " tile " << (tileIndex + 1) << " of " << numberOfTiles << " (series plotted: " << seriesPlotted << ")" << endl;

        // Get layout offsets for the current tile count
        vector<string> layoutOffsets = tileOffsets[numberOfTiles];

        // Create legend file for the tile
        fs::path tileLegendPath = fs::temp_directory_path() / ("wave_legend_" + std::to_string(imageNumber) + "_" + std::to_string(tileIndex + 1) + ".txt");

        // Create legend output string stream
        std::ostringstream tileLegendSs;

        // Send header to legend output stream
        // Define 2 columns
        tileLegendSs << "N 2" << endl;
        tileLegendSs << "G 2p" << endl;

        // Plot basemap for this tile with the title
        scriptOfs << "gmt basemap " << plotExtent << " -JX12c/9c "
                  << "-Baf -B+t\"" << title << "\" "
                  << "-Bx+l\"" << timeAxisLabel << "\" "
                  << "-By+l\"" << waveHeightAxisLabel << "\" "
                  << layoutOffsets[tileIndex]
                  << " --FONT_TITLE=12p,Helvetica --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_LABEL=8p,Helvetica  "
                  << " -Vq" << std::endl;

        // For each tile, plot up to seriesPerPlot series
        for (int s = 0; s < seriesPerPlot; s++)
        {
          int seriesIndex = (currentTile * seriesPerPlot) + s;

          if (seriesIndex >= seriesCount)
          {
            break;
          }

          string color = linePalette[s % linePalette.size()];

          // Line to plot the wave data series for the current seriesIndex
          scriptOfs << "gmt plot " << plotExtent << " \"" << waveDataPath << "\" "
                    << " -i0," << (seriesIndex + 1)
                    << " -JX12c/9c "
                    // Append +s for spline rounding
                    // << " -W1p," << color << "+s"
                    // Don't use spline rounding to show actual data
                    << " -W1p," << color
                    << " -Vq"
                    << std::endl;

          // Add legend with line name from seriesIndex
          // Use line symbol instead with color
          tileLegendSs << "S R - 0.5c " << color << " " << color << " 0.5p" << endl;

          // Add gauge name, take max. 18 chars
          string gaugeName = Strings::replaceAll(gauges.name[seriesIndex], "_", " ");
          if (gaugeName.length() > 20)
          {
            gaugeName = gaugeName.substr(0, 20);
          }
          tileLegendSs << "L - L " << gaugeName << endl;

          // Get first wave data

          float tFirstWave = firstWave[seriesIndex].time;
          float hFirstWave = firstWave[seriesIndex].height;

          seriesPlotted++;
        }

        // Write legend file for the tile
        std::ofstream tileLegendOfs(tileLegendPath.string());
        tileLegendOfs << tileLegendSs.str();
        tileLegendOfs.close();

        // Line to plot the legend file for the tile
        scriptOfs << "gmt legend " << tileLegendPath.string() << plotExtent
                  << " -JX12c/9c "
                  << " -DjRT+w5c --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_TITLE=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

        // Add current tile legend path to the list of legend files
        legendFiles.push_back(tileLegendPath.string());

        currentTile++;
      }

      // End GMT session for this image
      scriptOfs << "gmt end" << std::endl;
    }

    // Remove temporary legend files if _DEBUG is not defined
#ifndef _DEBUG
    for (const auto &legendFile : legendFiles)
    {
#ifdef WIN32
      scriptOfs << "del \"" << legendFile << "\"" << std::endl;
#elif __linux__
      scriptOfs << "rm \"" << legendFile << "\"" << std::endl;
#endif
    }
#endif

#ifndef _DEBUG
    // Remove temporary wave data file if time was adjusted
    if (timeAdjusted)
    {
#ifdef WIN32
      scriptOfs << "del \"" << waveDataPath << "\"" << std::endl;
#elif __linux__
      scriptOfs << "rm \"" << waveDataPath << "\"" << std::endl;
#endif
    }
#endif

    // Close the batch file
    scriptOfs.close();

    // If linux, use fs::permissions to set the script as executable (only for the owner)
#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
#endif

#ifdef _DEBUG
    // Show script path in debug mode
    cout << scriptPath.string() << endl;
#endif

    // Return the script path
    return scriptPath;
  }

  /**
   * @brief Create a Grid Plot Shell Script object
   * @param gridPath Input grid file path
   * @param palettePath Input color palette file path
   * @param title Input title for the plot
   * @param subtitle Input subtitle for the plot
   * @param outputPath Output file path
   * @return fs::path Path to the created shell script file, including filename.
   */
  fs::path createGridPlotScript(string gridPath, string palettePath, string title, string subtitle, string outputPath)
  {

    // Use filename from gridPath to create a .bat file inside fs::temp_directory_path()
    fs::path scriptPath = fs::temp_directory_path() / (fs::path(gridPath).stem().string() + ".bat");

    std::ofstream scriptOfs(scriptPath.string());

    // Create shell script depending on OS
#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << "REM Auto-generated batch file to plot grid " << gridPath << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "set GMT_END_SHOW=off" << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "set GMT_VERBOSE=quiet" << std::endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    scriptOfs << "# Auto-generated shell file to plot grid " << gridPath << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
#endif
    // Start GMT modern session
    scriptOfs << "gmt begin \"" << outputPath << "\" png E600" << std::endl;

    scriptOfs << "gmt grdimage \"" << gridPath << "\" -JM15C"
              << " -C\"" << palettePath << "\" -Bafg";

    if (title.length())
    {
      scriptOfs << " -BNSEW+t\"" << title << "\"";
      if (subtitle.length())
      {
        scriptOfs << "+s\"" << subtitle << "\"";
      }
    }
    scriptOfs << " --FONT_TITLE=12p -Vq";
    scriptOfs << std::endl;

    // Create colorbar
    scriptOfs << "gmt colorbar "
              << " -R\"" << gridPath << "\""
              << " -JM15c"
              << " -DjMR+w8c/0.5c+o-2.5c/0c+v"
              << " -C\"" << palettePath << "\" -Bafg"
              << " -Baf+l\"Altura (m)\" -Bxa1f1 -B+u\" m.\""
              << " --FONT_ANNOT_PRIMARY=8p --FONT_LABEL=10p"
              << std::endl;

    scriptOfs << "gmt end" << std::endl;

    scriptOfs.close();

    cout << scriptPath.string() << endl;

    return scriptPath;
  }

  void plotDeform(geo::Options &options)
  {

    auto [inputPath, outputPath] = getPaths(options);

    // Get deformation file path
    fs::path deformPath = options.get("deform");

    Grid deformGrid;
    // Load grid and convert to ESRI format
    if (loadGrid(deformGrid, deformPath, options, true) == false)
    {
      cerr << "Unable to load deform grid from " << deformPath << endl;
      return;
    }

    cout << "Plotting deformation grid " << deformPath.string() << " ..." << std::endl;

    // ESRI grid for deform must exist, so replace deformPath extension
    deformPath = deformPath.replace_extension(".bil");

    // Get bathymetry grid path
    fs::path bathymetryPath = fs::path(options.get("grid"));

    Grid bathymetryGrid;
    // Load bathymetry grid and convert to ESRI format
    if (loadGrid(bathymetryGrid, bathymetryPath, options, true) == false)
    {
      cerr << "Unable to load bathymetry grid from " << options.get("grid") << endl;
      return;
    }

    //  ESRI grid for bathymetry must exist, so replace grid extension
    bathymetryPath = bathymetryPath.replace_extension(".bil");

    // Create bat or sh file depending on OS
    fs::path scriptPath = createDeformPlotScript(deformPath.string(), bathymetryPath.string(), options);

    // cout << "Plotting deformation. Please wait while global map data is downloaded..." << endl;

    // Execute the script to create the plot using executeCommand
    int exitCode = executeCommand(scriptPath.string(), true);

#ifndef _DEBUG
    // Remove the script file
    try
    {
      fs::remove(scriptPath);
    }
    catch (fs::filesystem_error &e)
    {
      cerr << "Error removing temporary script file: " << e.what() << endl;
    }
#endif

    if (exitCode != 0)
    {
      cerr << "Plot script failed with exit code " << exitCode << endl;
      return;
    }

    cout << "Deform " << deformPath.replace_extension("png").string() << " created" << std::endl;
  }

  void plotZMax(geo::Options &options)
  {
    using geo::GridFormat;

    fs::path gridPath;
    fs::path zMaxPath;
    fs::path gaugePath;

    auto [inputPath, outputPath] = getPaths(options);

    // If output path is relative, set relative to inputPath
    if (outputPath.is_relative())
    {
      outputPath = (inputPath / outputPath).make_preferred();
    }

    // Check required parameters
    GridFormat gridOutputFormat{GridFormat::ESRI_FLOAT};
    if (options.contains("format"))
    {
      GridFormat suggestedFormat = geo::getFormat(options.get("format"));
      if (suggestedFormat == GridFormat::UNKNOWN)
      {
        cerr << "Warning! invalid output_format option: " << options.get("format") << ". Using ESRI (.bil) format" << endl;
      }
      else
      {
        gridOutputFormat = suggestedFormat;
      }
    }

    // Get grid path
    gridPath = fs::path(options.get("grid"));
    if (!fs::exists(gridPath) && fs::exists(inputPath / gridPath))
    {
      gridPath = (inputPath / gridPath).make_preferred();
    }
    else if (!fs::exists(gridPath) && fs::exists(outputPath / gridPath))
    {
      gridPath = (outputPath / gridPath).make_preferred();
    }

    if (!fs::exists(gridPath))
    {
      cerr << "Grid " << gridPath.string() << " does not exist." << endl;
      return;
    }

    // Get cannonical route to grid path
    gridPath = fs::canonical(gridPath);

    Grid grid;
    // Attempt to load grid and convert to esri
    if (loadGrid(grid, gridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << gridPath.string() << endl;
      return;
    }

    // Get zMax path
    zMaxPath = fs::path("zmax" + geo::gridExtension(gridOutputFormat));
    if (options.contains("zmax"))
    {
      zMaxPath = options.get("zmax");
    }

    if (!fs::exists(zMaxPath) && fs::exists(inputPath / zMaxPath))
    {
      zMaxPath = (inputPath / zMaxPath).make_preferred();
    }
    else if (!fs::exists(zMaxPath) && fs::exists(outputPath / zMaxPath))
    {
      zMaxPath = (outputPath / zMaxPath).make_preferred();
    }

    if (!fs::exists(zMaxPath))
    {
      cerr << "Zmax grid " << zMaxPath.string() << " does not exist." << endl;
      return;
    }

    // Convert zMaxPath to canonical
    zMaxPath = fs::canonical(zMaxPath);

    cout << "Plotting zmax grid " << zMaxPath.string() << " ..." << std::endl;

    // Get zmax grid

    Grid zMaxGrid;

    if (loadGrid(zMaxGrid, zMaxPath, options, true) == false)
    {
      cerr << "Unable to load zmax grid from " << zMaxPath.string() << endl;
      return;
    }

    // Get wave data path
    fs::path waveDataPath = "point.dat";
    if (options.contains("wave_data"))
    {
      waveDataPath = fs::path(options.get("wave_data")).make_preferred();
    }

    // Search for wave data in output path if not found in current path
    if (!fs::exists(waveDataPath) && fs::exists(outputPath / waveDataPath))
    {
      waveDataPath = (outputPath / waveDataPath).make_preferred();
    }

    if (!fs::exists(waveDataPath))
    {
      cerr << "Wave data file " << waveDataPath.string() << " does not exist." << endl;
      return;
    }

    // Get cannonical route to wave data path
    waveDataPath = fs::canonical(waveDataPath);

    // Get gauge path
    gaugePath = fs::path(options.get("gauge")).make_preferred();

    // Check gauge path existence
    if (!fs::exists(gaugePath) && fs::exists(inputPath / gaugePath))
    {
      gaugePath = (inputPath / gaugePath).make_preferred();
    }
    else if (!fs::exists(gaugePath) && fs::exists(outputPath / gaugePath))
    {
      gaugePath = (outputPath / gaugePath).make_preferred();
    }

    if (!fs::exists(gaugePath))
    {
      cerr << "Gauges file " << gaugePath.string() << " does not exist." << endl;
      return;
    }

    // Get cannonical route to gauge path
    gaugePath = fs::canonical(gaugePath);

    // Create bat or sh file depending on OS
    fs::path scriptPath = createMaxPlotScript(gridPath.string(), zMaxPath.string(), waveDataPath.string(), gaugePath.string(), options);

    // Execute the script to create the plot using executeCommand
    int exitCode = executeCommand(scriptPath.string(), true);

#ifndef _DEBUG
    // Remove the script file
    try
    {
      fs::remove(scriptPath);
    }
    catch (fs::filesystem_error &e)
    {
      cerr << "Error removing temporary script file: " << e.what() << endl;
    }
#endif

    if (exitCode != 0)
    {
      cerr << "Plot script failed with exit code " << exitCode << endl;
      return;
    }
    cout << "Zmax " << zMaxPath.replace_extension("png").string() << std::endl;
  }

  void plotWaveProfile(geo::Options &options)
  {

    fs::path inputPath = fs::path(options.get("input")).make_preferred();
    fs::path outputPath = fs::path(options.get("output")).make_preferred();

    // If output path is relative, set relative to inputPath
    if (outputPath.is_relative())
    {
      outputPath = (inputPath / outputPath).make_preferred();
    }

    // Check required parameters
    GridFormat gridOutputFormat{GridFormat::ESRI_FLOAT};
    if (options.contains("format"))
    {
      GridFormat suggestedFormat = geo::getFormat(options.get("format"));
      if (suggestedFormat == GridFormat::UNKNOWN)
      {
        cerr << "Warning! invalid format: " << options.get("format") << ". Using ESRI (.bil) format" << endl;
      }
      else
      {
        gridOutputFormat = suggestedFormat;
      }
    }

    // Get zMax path
    fs::path zMaxPath = fs::path("zmax" + geo::gridExtension(gridOutputFormat));
    if (options.contains("zmax"))
    {
      zMaxPath = options.get("zmax");
    }

    if (!fs::exists(zMaxPath) && fs::exists(inputPath / zMaxPath))
    {
      zMaxPath = (inputPath / zMaxPath).make_preferred();
    }
    else if (!fs::exists(zMaxPath) && fs::exists(outputPath / zMaxPath))
    {
      zMaxPath = (outputPath / zMaxPath).make_preferred();
    }

    if (!fs::exists(zMaxPath))
    {
      cerr << "Zmax grid " << zMaxPath.string() << " does not exist." << endl;
      return;
    }

    // Convert zMaxPath to canonical
    zMaxPath = fs::canonical(zMaxPath);

    fs::path waveDataPath = fs::path("point.dat").make_preferred();
    if (options.contains("wave_data"))
    {
      waveDataPath = fs::path(options.get("wave_data")).make_preferred();
    }

    // Search for wave data in output path if not found in current path
    if (!fs::exists(waveDataPath) && fs::exists(outputPath / waveDataPath))
    {
      waveDataPath = (outputPath / waveDataPath).make_preferred();
    }

    if (!fs::exists(waveDataPath))
    {
      cerr << "Plot wave: Wave data file " << waveDataPath.string() << " does not exist." << endl;
      return;
    }

    // Get cannonical route to wave data path
    waveDataPath = fs::canonical(waveDataPath);

    cout << "Plotting wave profiles from " << waveDataPath.string() << " ..." << std::endl;

    // Get gauge path
    fs::path gaugePath = fs::path(options.get("gauge")).make_preferred();

    if (!fs::exists(gaugePath) && fs::exists(inputPath / gaugePath))
    {
      gaugePath = (inputPath / gaugePath).make_preferred();
    }
    else if (!fs::exists(gaugePath) && fs::exists(outputPath / gaugePath))
    {
      gaugePath = (outputPath / gaugePath).make_preferred();
    }

    if (!fs::exists(gaugePath))
    {
      cerr << "Gauges file " << gaugePath.string() << " does not exist." << endl;
      return;
    }

    // Convert gaugePath to canonical
    gaugePath = fs::canonical(gaugePath);

    // Create bat or sh file depending on OS
    fs::path scriptPath = createWavePlotScript(zMaxPath.string(), waveDataPath.string(), gaugePath.string(), options);

    // Execute the script to create the plot using executeCommand
    int exitCode = executeCommand(scriptPath.string(), true);

    // Remove the script file
#ifndef _DEBUG
    try
    {
      fs::remove(scriptPath);
    }
    catch (fs::filesystem_error &e)
    {
      cerr << "Error removing temporary script file: " << e.what() << endl;
    }
#endif

    if (exitCode != 0)
    {
      cerr << "Plot script failed with exit code " << exitCode << endl;
      return;
    }
  }

  std::tuple<fs::path, fs::path> getPaths(geo::Options &options, bool createOutput)
  {

    // assume current path, unless options contains "input" option
    fs::path inputPath = fs::current_path();

    if (options.contains("input"))
    {
      inputPath = fs::path(options.get("input")).make_preferred();
    }

    // assume current path, unless options contains "output" option
    fs::path outputPath = fs::current_path();
    if (options.contains("output"))
    {
      outputPath = fs::path(options.get("output"));
    }

    if (outputPath.is_relative())
    {
      outputPath = (inputPath / outputPath).make_preferred();
    }

    // Create output directory if it does not exist
    if (createOutput && !fs::exists(outputPath))
    {
      fs::create_directories(outputPath);
    }

    return {inputPath, outputPath};
  }

}
