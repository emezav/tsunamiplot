/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Tsunami plot utilities
 * @copyright Copyright (c) 2025
 */

#include "tsunamiplot.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cmd.h"
#include "gauges.h"
#include "geo.h"
#include "grid.h"
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
        "0/102/0",
        "0/0/102",
        "170/85/68",
        "0/136/204",
        "255/17/255",
        "255/0/119",
        "136/255/153",
        "136/102/255",
        "85/136/119",
        "102/0/68",
        "221/255/0",
        "255/136/0",
        "119/102/0",
        "187/0/153",
        "153/119/153",
        "136/170/0",
        "34/51/102",
        "34/51/0",
        "170/187/119",
        "0/187/34",
        "255/85/85",
        "255/170/119",
        "68/17/0",
        "255/153/170",
        "0/187/136",
        "153/153/255",
        "136/221/255",
        "0/17/187",
        "153/0/0",
        "119/68/153",
        "187/255/221",
        "187/153/136",
        "170/17/68",
        "85/68/68",
        "153/17/238",
        "221/255/119",
        "170/85/0",
        "0/85/102",
        "221/204/255",
        "153/255/68",
        "0/68/187",
        "187/85/136",
        "221/102/238",
        "255/221/0",
        "119/0/153",
        "255/221/170",
        "0/255/119",
        "204/153/0",
        "85/170/68",
        "51/187/187",
        "34/0/34",
        "153/170/187",
        "0/255/204",
        "238/255/170",
        "255/0/204",
        "221/68/17",
        "119/85/51",
        "255/85/170",
        "187/153/85",
        "0/85/170",
        "0/85/51",
        "255/102/136",
        "102/119/68",
        "136/68/85",
        "0/153/187",
        "187/0/204",
        "68/0/85",
        "136/187/255",
        "119/119/187",
        "136/221/85",
        "0/136/255",
        "170/0/102",
        "102/0/34",
        "187/119/221",
        "238/119/68",
        "0/204/102",
        "187/204/187",
        "238/187/204",
        "51/136/85",
        "136/221/153",
        "85/119/153",
        "102/136/34",
        "85/51/85",
        "119/51/0",
        "119",
        "85/68/255",
        "119/187/153",
        "0/34/17",
        "68/85/0",
        "170/221/34",
        "221/153/204",
        "136/17/119",
        "204/204/34",
        "0/0/68",
        "170/221/119",
        "204/136/51",
        "255/136/119",
        "255/187/170",
        "170/170/68",
        "255/0/85",
        "0/68/221",
        "0/34/51",
        "51/34/0",
        "187/85/255",
        "136/238/238",
        "255/0/51",
        "0/68/0",
        "51/17/153",
        "238/102/204",
        "170/102/170",
        "187/119/119",
        "119/68/187",
        "68/17/34",
        "119/17/17",
        "255/170/0",
        "68/85/68",
        "102/102/204",
        "255/17/170",
        "51/221/17",
        "255/187/85",
        "255/255/221",
        "119/34/204",
        "85/170/0",
        "51/51/136",
        "85/68/0",
        "221/170/255",
        "0/255/170",
        "0/136/0",
        "153/102/34",
        "204/0/51",
        "255/255/85",
        "204/238/255",
        "187/85/102",
        "0/187/255",
        "0/85/136",
        "204/136/102",
        "255/136/187",
        "85/119/255",
        "102/238/204",
        "68/0/221",
        "221/204/119",
        "255/85/255",
        "170/68/187",
        "255/153/68",
        "221/51/119",
        "255/102/0",
        "136/153/119",
        "187/68/153",
        "153/136/0",
        "0/17/51",
        "119/85/136",
        "187/68/51",
        "0/221/255",
        "153/153/204",
        "187/119/255",
        "102/238/85",
        "102/204/0",
        "85/34/136",
        "0/153/136",
        "187/221/170",
        "102/153/238",
        "119/255/0",
        "85/85/119",
        "136/0/68",
        "238/187/0",
        "51/34/51",
        "68/17/68",
        "136/119/68",
        "119/170/102",
        "204/187/187",
        "136/187/187",
        "255/85/221",
        "204/170/68",
        "187/255/170",
        "0/119/119",
        "221/136/0",
        "119/51/34",
        "238/187/119",
        "204/0/17",
        "119/85/102",
        "187/153/170",
        "204/187/153",
        "255/255/136",
        "0/136/51",
        "221/102/17",
        "136/187/68",
        "187/170/17",
        "204/119/68",
        "119/51/102",
        "187/204/238",
        "153/51/0",
        "17/221/153",
        "17/51/51",
        "204/68/85",
        "34/0/0",
        "238/51/102",
        "153/0/34",
        "204/0/136",
        "187/204/85",
        "51/68/85",
        "68/68/34",
        "204/51/255",
        "221/255/238",
        "187/255/102",
        "102/51/51",
        "51/85/34",
        "102/153/204",
        "0/255/85",
        "68/119/51",
        "153/119/119",
        "221/119/119",
        "136/136/51",
        "221/119/187",
        "170/136/204",
        "68/170/102",
        "238/0/136",
        "85/68/136",
        "119/255/187",
        "85/204/119",
        "170/102/136",
        "255/136/255",
        "17/68/51",
        "119/0/255",
        "yellow",
        "17/119/204",
        "170/0/170",
        "255/102/68",
        "187/255/34",
        "85/85/221",
        "187/255/136",
        "170/136/255",
        "0/170/51",
        "153/102/85",
        "119/102/85",
        "221/255/85",
        "85/51/17",
        "102/153/170",
        "0/51/85",
        "51/119/136",
        "0/34/102",
        "0/204/187",
        "85/238/119",
        "34/170/255",
        "255/170/204",
        "221/34/204",
        "0/34/136",
        "255/221/68",
        "0/51/17",
        "68/102/102"};
  }

  /**
   * @brief Get the palette color sequence
   * @return Palette of RGB colors for lines in wave graph
   */
  vector<string> getLinePalette()
  {

    // Return the same palette as lines and points must match
    return getPointPalette();
  }

  /**
   * @brief Create a Max Palette File object
   * @return fs::path Path to the created palette file
   */
  // Build a max-wave CPT with fixed color sequence darkblue->blue->yellow->orange->red->darkred
  // scaled to [0, maxZ].  Breakpoint fractions: 0 / 10% / 20% / 40% / 60% / 100%.
  // Default maxZ = 1.0 (near-field N2).  Far-field F1 scenarios set palette_max_z = 7.
  fs::path createMaxPaletteFile(float maxZ = 1.0f)
  {
    fs::path palettePath = std::filesystem::temp_directory_path() / "max.cpt";

    std::ofstream ofs(palettePath.string());

    float b1 = maxZ * 0.10f;
    float b2 = maxZ * 0.20f;
    float b3 = maxZ * 0.40f;
    float b4 = maxZ * 0.60f;

    ofs << "# Max CPT 0-" << maxZ << "m -- white(0)->darkblue->blue->yellow->orange->red->darkred\n"
        << "0 white 0.01 darkblue L\n"
        << "0.01 darkblue " << b1 << " blue L\n"
        << b1 << " blue "   << b2 << " yellow L\n"
        << b2 << " yellow " << b3 << " orange L\n"
        << b3 << " orange " << b4 << " red L\n"
        << b4 << " red "    << maxZ << " darkred B\n"
        << "B darkblue\n"
        << "F darkred\n"
        << "N white\n";

    ofs.close();

    return palettePath;
  }

  /**
   * @brief Create a Inundation Palette File object
   * @return fs::path Path to the created palette file
   */
  fs::path createInundationPaletteFile()
  {
    fs::path palettePath = std::filesystem::temp_directory_path() / "inundation.cpt";

    std::ofstream ofs(palettePath.string());

  //  ofs << "# Inundation CPT file 0-7m\n"
  //          "# z-value  R  G  B    z-value   R  G  B\n"
  //          "0	0/0/127	0.1	0/0/241	L\n"
  //          "0.1	0/0/241	0.2	99/255/100	L\n"
  //          "0.2	99/255/100	0.3	0/212/255	L\n"
  //          "0.3	0/212/255	0.4	255/255/219	L\n"
  //          "0.4	255/255/219	0.6	255/255/163	L\n"
  //          "0.6	255/255/163	1.0	255/213/0	L\n"
  //          "1.0	255/213/0	2.0	255/9/0	L\n"
  //          "2.0	255/9/0	5.0	241/0/0	L\n"
  //          "5.0	241/0/0	7.0	127/0/0	B\n"
  //          "# Background color\n"
  //          "B	0/0/127\n"
  //          "# Foreground color\n"
  //          "F	255/255/255\n"
  //          "# NaN color\n"
  //          "N 255/255/255\n";

  // Standad  0 - 1 m inundation palette
   ofs << "# Inundation CPT file 0-1m\n"
          "# z-value  R  G  B    z-value   R  G  B\n"
          "0 white 0.01 lightblue L\n"
          "0.01 lightblue 0.1 blue L\n"
          "0.1 blue 0.2 yellow L\n"
          "0.2 yellow 0.4 orange L\n"
          "0.4 orange 0.6 red L\n"
          "0.6 red 1.0 darkred B\n"
          "B white\n" // Background color
          "F white\n" // Foreground color
          "N white\n"; // NaN color

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

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

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
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
#endif

    // Create palettes on the temporary dir
    fs::path deformPalettePath = fs::temp_directory_path() / "deform_palette.cpt";
    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";

    fs::path deformFilenameWithoutExtension(deformFilename);
    deformFilenameWithoutExtension.replace_extension("");

    // Create CPT from the deformation grid: polar (blue-white-red), continuous, scaled to data range.
    // No alpha in the CPT (-A flag omitted); transparency is handled per-layer in grdimage (-t).
    scriptOfs << "gmt grd2cpt \"" << deformFilename.string() << "\" -Cpolar -Z -D > \"" << deformPalettePath.string() << "\"" << std::endl;

    // Create a CPT file for bathymetry using the globe base palette
    // Bathymetry grid must be positive for land and negative for sea, so only invert the palette if requested
    if (Strings::tolower(options.get("bathy_convention")) == "tunami")
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
    if (lang == "us")
    {
      title = "Deformation";
    }

    string source = options.get("source");
    if (source.length())
    {
      title += " - " + source;
    }

    string heightStr = "Altura (m)";
    // If lang option is set to "us", change heightStr to "Height (m)"
    if (lang == "us")
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

    // Mask near-zero deformation values (|z| < 0.1 m) to NaN so the white halo around the
    // Okada footprint disappears. Values that small are not meaningful for hazard visualization.
    fs::path clippedDeformPath = fs::temp_directory_path() / (fs::path(deformFilename).stem().string() + "_clip.nc");
    scriptOfs << "gmt grdclip \"" << deformFilename.string() << "\" -G\"" << clippedDeformPath.string() << "\" -Si-0.1/0.1/NaN -Vq" << std::endl;

    // Overlay clipped deformation grid; -Q makes NaN cells transparent.
    scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << clippedDeformPath.string() << "\" -C\"" << deformPalettePath.string() << "\" -Q -t20 --FONT_TITLE=12p -Vq" << std::endl;

    // Draw coast lines
    // -W shorelines, -N1 country borders
    if (plotCoast)
    {
      scriptOfs << "gmt coast -JM15c -R" << extentStr << " -D" << coastRes << " -N1/0.01p,gray77 -W1/0.01p,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

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

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
#endif

// Remove palette file
#ifndef _DEBUG
#ifdef WIN32
    scriptOfs << "del \"" << deformPalettePath.string() << "\"" << std::endl;
    scriptOfs << "del \"" << clippedDeformPath.string() << "\"" << std::endl;
#elif __linux__
    scriptOfs << "rm \"" << deformPalettePath.string() << "\"" << std::endl;
    scriptOfs << "rm \"" << clippedDeformPath.string() << "\"" << std::endl;
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
   * @brief Create a bathymetry plot script
   * @param bathymetryPath Path to bathymetry ESRI grid (.bil)
   * @param options Geo options
   * @return fs::path Path to generated script
   */
  fs::path createBathyPlotScript(string bathymetryPath, geo::Options &options)
  {
    string lang = Strings::tolower(options.get("lang"));
    if (lang.length() == 0) lang = "es";
    string langUpper = Strings::toupper(static_cast<string>(lang));

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    fs::path scriptPath = fs::temp_directory_path() / (fs::path(bathymetryPath).stem().string() + "_bathy" + fileExt);

    std::ofstream scriptOfs(scriptPath.string());

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
#endif

    // Load bathymetry grid to get extent and resolution parameters
    Grid grid;
    fs::path bathymetryGridPath(bathymetryPath);
    if (loadGrid(grid, bathymetryGridPath, options) == false)
    {
      cerr << "Unable to load grid from " << bathymetryGridPath.string() << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }

    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";

    bool bathyCptFit = Strings::tolower(options.get("bathy_cpt_fit")) == "true";
    bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";

    // bathy_cpt selects the GMT palette name.
    // Default: "geo" when fitting to data range (avoids deep-ocean purple from globe),
    //          "globe" when using the fixed global scale.
    string bathyCpt = options.get("bathy_cpt");
    if (bathyCpt.empty())
    {
      bathyCpt = bathyCptFit ? "geo" : "globe";
    }

    // For TUNAMI convention (positive = depth), negate grid values so that standard
    // GMT palettes (geo/globe expect negative = ocean depth) apply without -I inversion.
    // Land near z=0 maps to coastal green instead of the "mountain white" end of an
    // inverted palette, and the colorbar shows the conventional negative-depth scale.
    fs::path plotGridPath(bathymetryPath);
    fs::path negatedGridPath;
    bool negatedGridCreated = false;
    if (bathyInvert)
    {
      float nd = static_cast<float>(grid.noDataValue());
      for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
          if (grid(i, j) != nd)
            grid(i, j) = -grid(i, j);

      negatedGridPath = fs::temp_directory_path() / (fs::path(bathymetryPath).stem().string() + "_neg.bil");
      if (geo::SaveGrid(grid, negatedGridPath.string(), GridFormat::ESRI_FLOAT) == geo::geoStatus::SUCCESS)
      {
        plotGridPath = negatedGridPath;
        negatedGridCreated = true;
      }
    }

    // Paths for fitted-mode temp grids and CPTs (defined here so cleanup section can access them)
    fs::path oceanOnlyPath = fs::temp_directory_path() / (fs::path(bathymetryPath).stem().string() + "_ocean.grd");
    fs::path landOnlyPath  = fs::temp_directory_path() / (fs::path(bathymetryPath).stem().string() + "_land.grd");
    fs::path landCptPath   = fs::temp_directory_path() / (fs::path(bathymetryPath).stem().string() + "_land.cpt");

    if (bathyCptFit)
    {
      // Ocean: clip to z<=0, fit palette with -Z to the actual ocean depth range.
      scriptOfs << "gmt grdclip \"" << plotGridPath.string() << "\" -Sa0/NaN -G\"" << oceanOnlyPath.string() << "\"" << std::endl;
      scriptOfs << "gmt grd2cpt \"" << oceanOnlyPath.string() << "\" -C" << bathyCpt << " -Z -D"
                << " > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      // Land: clip to 0<z<=8850 (above sea level, below any realistic nodata flag).
      // TUNAMI grids often store dry land as -9999 (-> +9999 after negation); removing
      // those cells prevents grd2cpt from pulling the palette white to low elevations.
      // Both layers use the same base CPT so colors join smoothly at z=0.
      scriptOfs << "gmt grdclip \"" << plotGridPath.string() << "\" -Sb0/NaN -Sa8850/NaN -G\"" << landOnlyPath.string() << "\"" << std::endl;
      scriptOfs << "gmt grd2cpt \"" << landOnlyPath.string() << "\" -C" << bathyCpt << " -D"
                << " > \"" << landCptPath.string() << "\"" << std::endl;
    }
    else
    {
      scriptOfs << "gmt makecpt -C" << bathyCpt << " -D"
                << " > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
    }

    string title = "Batimetria";
    if (lang == "us") title = "Bathymetry";

    string source = options.get("source");
    if (source.length()) title += " - " + source;

    string depthStr = "Profundidad/Altura (m)";
    if (lang == "us") depthStr = "Depth/Elevation (m)";

    // Output PNG: same directory as bathymetry grid, stem + _bathy suffix
    fs::path outputBase = fs::path(bathymetryPath).parent_path() / (fs::path(bathymetryPath).stem().string() + "_bathy");

    scriptOfs << "gmt begin \"" << outputBase.string() << "\" png E600" << std::endl;
    scriptOfs << "gmt set MAP_FRAME_TYPE fancy+" << std::endl;
    scriptOfs << "gmt set MAP_FRAME_WIDTH 2.5p" << std::endl;
    scriptOfs << "gmt set GMT_LANGUAGE " << langUpper << std::endl;

    if (bathyCptFit)
    {
      // Two-layer grdimage with -Q (NaN transparent): ocean-only first, land-only on top.
      // Land/ocean boundary follows the grid itself, not the coarse GSHHG coastline database,
      // giving pixel-perfect separation at any grid resolution.
      scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << oceanOnlyPath.string() << "\" -C\"" << bathymetryPalettePath.string() << "\" -Q -Bafg -BWSen+t\"" << title << "\" --FONT_TITLE=12p --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;
      scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << landOnlyPath.string() << "\" -C\"" << landCptPath.string() << "\" -Q -Vq" << std::endl;
    }
    else
    {
      scriptOfs << "gmt grdimage -JM15c -R" << extentStr << " \"" << plotGridPath.string() << "\" -C\"" << bathymetryPalettePath.string() << "\" -Bafg -BWSen+t\"" << title << "\" --FONT_TITLE=12p --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;
    }

    if (plotCoast)
    {
      // Coast draws borders and coastline lines only; land fill is handled by the grid layers.
      scriptOfs << "gmt coast -JM15c -R" << extentStr << " -D" << coastRes << " -N1/0.01p,gray77 -W1/0.01p,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

    double gridLengthX = columns * dxM;
    double gridLengthY = rows * dyM;
    double gridLengthKm = std::min(gridLengthX, gridLengthY) / 1000.0;

    float scaleLengthKm = trunc(gridLengthKm / 5.0f);
    if (scaleLengthKm < 1) scaleLengthKm = 0.1f;
    if (scaleLengthKm < 1)
      scaleLengthKm = (scaleLengthKm < 0.5f) ? 0.5f : 1.0f;
    else if (scaleLengthKm < 10)
      scaleLengthKm = round(scaleLengthKm);
    else if (scaleLengthKm < 100)
      scaleLengthKm = round(scaleLengthKm / 5.0f) * 5.0f;
    else if (scaleLengthKm < 1000)
      scaleLengthKm = round(scaleLengthKm / 50.0f) * 50.0f;
    else
      scaleLengthKm = round(scaleLengthKm / 500.0f) * 500.0f;

    scriptOfs << "gmt basemap -JM15c -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=6p,Helvetica --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=8p,Helvetica -Vq" << std::endl;

    if (!bathyCptFit)
      scriptOfs << "gmt colorbar -JM15c -R" << extentStr << " -DjMR+w6c/0.5c+o-3c/0c+v -C\"" << bathymetryPalettePath.string() << "\" -Bafg -Baf+l\"" << depthStr << "\" -B+u\" m.\" --FONT_ANNOT_PRIMARY=6p --FONT_LABEL=6p -Vq" << std::endl;

    scriptOfs << "gmt end" << std::endl;

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
#endif

#ifndef _DEBUG
#ifdef WIN32
    scriptOfs << "del \"" << bathymetryPalettePath.string() << "\"" << std::endl;
    if (negatedGridCreated)
    {
      fs::path negatedHdrPath = negatedGridPath; negatedHdrPath.replace_extension(".hdr");
      fs::path negatedPrjPath = negatedGridPath; negatedPrjPath.replace_extension(".prj");
      scriptOfs << "if exist \"" << negatedGridPath.string() << "\" del \"" << negatedGridPath.string() << "\"" << std::endl;
      scriptOfs << "if exist \"" << negatedHdrPath.string() << "\" del \"" << negatedHdrPath.string() << "\"" << std::endl;
      scriptOfs << "if exist \"" << negatedPrjPath.string() << "\" del \"" << negatedPrjPath.string() << "\"" << std::endl;
    }
    if (bathyCptFit)
    {
      scriptOfs << "if exist \"" << oceanOnlyPath.string() << "\" del \"" << oceanOnlyPath.string() << "\"" << std::endl;
      scriptOfs << "if exist \"" << landOnlyPath.string() << "\" del \"" << landOnlyPath.string() << "\"" << std::endl;
      scriptOfs << "if exist \"" << landCptPath.string() << "\" del \"" << landCptPath.string() << "\"" << std::endl;
    }
#elif __linux__
    scriptOfs << "rm \"" << bathymetryPalettePath.string() << "\"" << std::endl;
    if (negatedGridCreated)
    {
      fs::path negatedHdrPath = negatedGridPath; negatedHdrPath.replace_extension(".hdr");
      fs::path negatedPrjPath = negatedGridPath; negatedPrjPath.replace_extension(".prj");
      scriptOfs << "rm -f \"" << negatedGridPath.string() << "\" \"" << negatedHdrPath.string() << "\" \"" << negatedPrjPath.string() << "\"" << std::endl;
    }
    if (bathyCptFit)
      scriptOfs << "rm -f \"" << oceanOnlyPath.string() << "\" \"" << landOnlyPath.string() << "\" \"" << landCptPath.string() << "\"" << std::endl;
#endif
#endif

    scriptOfs.close();

#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
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
    float paletteMaxZ = 1.0f;
    if (!options.get("palette_max_z").empty())
    {
      try { paletteMaxZ = std::stof(options.get("palette_max_z")); }
      catch (...) { cerr << "Warning: invalid palette_max_z value, using 1.0\n"; }
    }
    fs::path palettePath = createMaxPaletteFile(paletteMaxZ);

    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length())
    {
      lang = "es";
    }

    string langUpper = Strings::toupper(static_cast<string>(lang));

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

    string title = "Altura estimada de Tsunami";

    if (lang == "us")
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

      cout << "Replacing " << noDataValue << " with nodata" << endl;
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

    // Create batch file
    std::ofstream scriptOfs(scriptPath.string());

    bool hasWaveData = !waveDataPath.empty() && fs::exists(fs::path(waveDataPath));
    bool hasGauges = !gaugePath.empty() && fs::exists(fs::path(gaugePath));

    vector<WavePoint> firstWave;
    vector<WavePoint> maxHeight;
    string timeUnits = "min";
    float timeResFactor = 1.0f;
    string eventType = Strings::tolower(options.get("event_type"));

    if (hasWaveData)
    {
      Timeseries pointData(waveDataPath);

      bool timeAdjusted = false;
      string timeRes = Strings::tolower(options.get("time_res"));

      if (timeRes == "h" || timeRes == "hours" || eventType == "far")
      {
        timeResFactor = 1.0f / 60.0f;
        pointData = pointData.applyTimeFactor(timeResFactor);
        fs::path tempWaveDataPath = fs::temp_directory_path() / (fs::path(waveDataPath).stem().string() + "_time_adjusted.dat");
        pointData.save(tempWaveDataPath.string());
        waveDataPath = tempWaveDataPath.string();
        timeAdjusted = true;
        timeUnits = "hrs";
      }

      float hFirstWave = 0.01f;
      float deltaFirstWave = 1.0f * timeResFactor;
      if (options.contains("h_first_wave")) { hFirstWave = options.getFloat("h_first_wave"); }
      if (options.contains("delta_first_wave")) { deltaFirstWave = options.getFloat("delta_first_wave") * timeResFactor; }

      WaveData waveData(pointData, hFirstWave, deltaFirstWave);
      firstWave = waveData.firstWave();
      maxHeight = waveData.maxHeight();
    }

    // Pre-load gauges so chunk count is known before writing any GMT commands
    Gauges gauges;
    vector<string> gaugeColors = getPointPalette();
    if (hasGauges && hasWaveData)
    {
      if (loadGauges(gaugePath, gauges, x0, y0, rows, columns, dxDeg, dyDeg) != true)
      {
        cerr << "Unable to load gauges from " << gaugePath << ", skipping gauge overlay." << endl;
        hasGauges = false;
      }
      else if (gauges.size() != firstWave.size())
      {
        cerr << "Warning! number of gauges (" << gauges.size() << ") does not match number of wave data series (" << firstWave.size() << ")" << endl;
        cerr << "Maybe some new gauges were added after the wave data was generated." << endl;
      }
    }

    // Layout: split gauges into sub-tables of N_PER_CHUNK placed side by side.
    //
    // Map width comes from the grid's geographic aspect ratio targeting TARGET_MAP_H height.
    // Sub-table width is then derived from the map width (map_w / n_chunks), so the tables
    // always align with the map.  A minimum of MIN_CHUNK_W enforces legibility; when that
    // minimum forces the map wider than the AR-based value, map_w expands only modestly.
    const int   N_PER_CHUNK  = 25;
    const float TARGET_MAP_H = 20.0f;  // cm: target Mercator map height
    const int   MIN_MAP_W    = 15;     // cm: absolute minimum map width
    const int   MIN_CHUNK_W  = 8;      // cm: minimum sub-table column width

    int n_gauges = (hasGauges && hasWaveData) ? (int)std::min(gauges.size(), firstWave.size()) : 0;
    int n_chunks = (n_gauges > 0) ? (n_gauges + N_PER_CHUNK - 1) / N_PER_CHUNK : 1;

    float lon_span = (float)(columns * dxDeg);
    float lat_span = (float)(rows    * dyDeg);

    // Map width from aspect ratio (near-equator Mercator approximation)
    int map_w_i = (lat_span > 0.0f)
                    ? std::max(MIN_MAP_W, (int)std::round(TARGET_MAP_H * (lon_span / lat_span)))
                    : MIN_MAP_W;

    // Sub-table width = map_w / n_chunks, enforcing MIN_CHUNK_W.
    // If rounding pushes map_w slightly wider, accept it (keeps table columns equal).
    int chunk_w_i = map_w_i;  // fallback when no gauges
    if (n_gauges > 0)
    {
      chunk_w_i = std::max(MIN_CHUNK_W, (int)std::round((float)map_w_i / (float)n_chunks));
      map_w_i   = n_chunks * chunk_w_i;  // realign (may expand by < MIN_CHUNK_W total)
    }

    string map_w_str   = std::to_string(map_w_i) + "c";
    string chunk_w_str = std::to_string(chunk_w_i) + "c";
    float legend_h = 22.0f;  // cm: generous for up to N_PER_CHUNK rows + header
    int heightPrecision = (eventType == "near") ? 2 : 4;

    // Write one legend file per chunk before the GMT script is written
    vector<fs::path> chunkLegendFiles;
    if (hasGauges && hasWaveData && n_gauges > 0)
    {
      for (int chunk = 0; chunk < n_chunks; chunk++)
      {
        int g_start = chunk * N_PER_CHUNK;
        int g_end = (int)std::min(g_start + N_PER_CHUNK, n_gauges);

        fs::path chunkFile = fs::temp_directory_path() / ("max_legend_chunk_" + std::to_string(chunk) + ".txt");
        std::ofstream chunkOfs(chunkFile.string());

        if (lang == "us") chunkOfs << "H - Time and Height from Waveform" << endl;
        else              chunkOfs << "H - Tiempo y altura por mareograma" << endl;
        chunkOfs << "N 0.10 0.40 0.125 0.125 0.125 0.125" << endl;
        chunkOfs << "G 4p" << endl;
        chunkOfs << "D 0.5p 0.5" << endl;

        if (lang == "us")
        {
          chunkOfs << "L - C Gauge" << endl;
          chunkOfs << "L - L Location" << endl;
          chunkOfs << "L - C T. First (" << timeUnits << ")" << endl;
          chunkOfs << "L - C H. First (m)" << endl;
          chunkOfs << "L - C T. Max (" << timeUnits << ")" << endl;
          chunkOfs << "L - C H. Max (m)" << endl;
        }
        else
        {
          chunkOfs << "L - C Punto" << endl;
          chunkOfs << "L - L Ubicacion" << endl;
          chunkOfs << "L - C T. Primero (" << timeUnits << ")" << endl;
          chunkOfs << "L - C H. Primero (m)" << endl;
          chunkOfs << "L - C T. Max (" << timeUnits << ")" << endl;
          chunkOfs << "L - C H. Max (m)" << endl;
        }
        chunkOfs << "G 4p" << endl;
        chunkOfs << "D 0.5p 0.5" << endl;

        for (int i = g_start; i < g_end; i++)
        {
          string color = gaugeColors[i % gaugeColors.size()];
          float h_first = firstWave[i].height;
          float t_first = firstWave[i].time;
          float h_max   = maxHeight[i].height;
          float t_max   = maxHeight[i].time;

          chunkOfs << "S 0.5c c 0.2c " << color << " 0/0/0" << endl;
          chunkOfs << "L - L " << Strings::replaceAll(gauges.name[i], "_", " ") << endl;

          if (t_first > 0.0f || t_max > 0.0f)
          {
            chunkOfs << "L - C " << std::fixed << std::setprecision(1) << t_first << endl;
            chunkOfs << "L - C " << std::fixed << std::setprecision(heightPrecision) << h_first << endl;
            chunkOfs << "L - C " << std::fixed << std::setprecision(1) << t_max << endl;
            chunkOfs << "L - C " << std::fixed << std::setprecision(heightPrecision) << h_max << endl;
          }
          else
          {
            chunkOfs << "L - C --" << endl;
            chunkOfs << "L - C --" << endl;
            chunkOfs << "L - C --" << endl;
            chunkOfs << "L - C --" << endl;
          }

          chunkOfs << "G 4p" << endl;
          chunkOfs << "D 0.5p 0.5" << endl;
        }

        chunkOfs.close();
        chunkLegendFiles.push_back(chunkFile);
      }
    }

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
    scriptOfs << "#!/bin/bash" << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
    // AppArmor workaround: the gs profile blocks ~/.gmt/sessions/gmt_0.ps- (dash extension).
    // Redirect the GMT user dir to /tmp/ (covered by the user-tmp abstraction, owner /tmp/** rwkl)
    // so that gmt_0.ps- lands under /tmp/ where gs can read it.
    // The directory must exist before GMT starts or it falls back to ~/.gmt/.
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
#endif

    // Resolve satellite filename relative to the grid directory (same as outline logic).
    string satelliteFilename = options.get("satellite");
    string satellitePath;
    if (!satelliteFilename.empty())
    {
        fs::path satResolved(gridPath);
        satResolved.replace_filename(satelliteFilename);
        if (fs::exists(satResolved))
            satellitePath = fs::canonical(satResolved).string();
        else
            std::cerr << "Warning: satellite image not found: " << satResolved.string() << std::endl;
    }
    bool showBathy = Strings::tolower(options.get("show_bathy")) == "true";
    int waveTrans = 0;
    try { if (!options.get("wave_transparency").empty()) waveTrans = std::stoi(options.get("wave_transparency")); } catch (...) {}
    string waveTransArg = (waveTrans > 0) ? " -t" + std::to_string(waveTrans) : "";

    if (showBathy && satellitePath.empty())
    {
      string bathyCpt = options.get("bathy_cpt");
      if (bathyCpt.empty() || !std::all_of(bathyCpt.begin(), bathyCpt.end(),
          [](char c){ return std::isalnum((unsigned char)c) || c == '_' || c == '-'; }))
        bathyCpt = "gray";
      if (bathyCpt == "gray")
      {
        // Scale gray palette to the local data range; invert if requested so deep = dark.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt grd2cpt \"" << gridPath << "\" -Cgray"
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
      else
      {
        // Physical palettes (globe, geo, topo, ...) must NOT be rescaled to the local
        // data range or the depth-to-colour mapping is destroyed.  Use makecpt so the
        // standard anchoring (0 = sea level) is preserved.
        // When bathy_convention=tunami the grid stores positive=ocean, negative=land,
        // which is the opposite of what globe/geo expect.  Invert the entire palette
        // (-I flips both the z-axis and the colour sequence) so that positive values
        // get ocean colours and negative values get land colours.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt makecpt -C" << bathyCpt
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
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
    scriptOfs << "gmt set PS_LINE_CAP round" << std::endl;

    // set PS_LINE_JOIN to ROUND
    scriptOfs << "gmt set PS_LINE_JOIN round" << std::endl;

    // Map is drawn first; legend follows below to avoid Cartesian coordinate-system
    // interference with the subsequent geographic grdimage calls.
    bool legendActive = hasGauges && hasWaveData && !chunkLegendFiles.empty();
    int yUp = legendActive ? (int)(legend_h + 2.0f) : 0;

    // -- shift Y up to leave room for the legend below
    string yUpArg = legendActive ? (" -Y" + std::to_string(yUp) + "c") : "";

    bool backgroundRendered = !satellitePath.empty() || showBathy;
    // The background layer (satellite/bathy) shifts the origin with yUpArg.
    // Subsequent layers must NOT repeat the shift or the map renders twice as far up.
    string waveYUpArg = backgroundRendered ? "" : yUpArg;

    if (!satellitePath.empty())
    {
      scriptOfs << "gmt grdimage -JM" << map_w_str << " -R" << extentStr << " \"" << satellitePath << "\"" << yUpArg << " -Vq" << std::endl;
    }
    else if (showBathy)
    {
      scriptOfs << "gmt grdimage -JM" << map_w_str << " -R" << extentStr << " \"" << gridPath << "\" -C\"" << bathymetryPalettePath.string() << "\"" << yUpArg << " -t40 -Vq" << std::endl;
    }

    // Plot zmax grid
    scriptOfs << "gmt grdimage -JM" << map_w_str << "  -R" << extentStr << " \"" << zMaxPlotPath.string() << "\" " << waveYUpArg << " -C\"" << palettePath.string() << "\" -Qwhite" << waveTransArg << " -Vq" << std::endl;

    // Draw map frame and title
    scriptOfs << "gmt basemap -JM" << map_w_str << " -R" << extentStr << " -Baf -BWSen+t\"" << title << "\" --FONT_TITLE=14p,Helvetica --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;

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
    // If outline option is set, plot outline file instead of coastlines

    // Outline is expected to be in the same directory as the grid file.
    // The outline option defines the filename of the outline file. If the outline option is not set, the default filename is "contour.gmt".
    fs::path outlinePath(gridPath);

    string outlineFilename = options.get("outline");

    if (outlineFilename.empty())
    {
      outlineFilename = "contour.gmt";
    }

    outlinePath.replace_filename(outlineFilename);

    // Check if outline file exists
    if (fs::exists(outlinePath))
    {
        cout << "Using outline file: " << fs::canonical(outlinePath).string() << endl;
        // If file is binary plot with -bi2f
        if (Strings::tolower(outlinePath.extension().string()) == ".bin" || Strings::tolower(outlinePath.extension().string()) == ".bf2")
        {
          scriptOfs << "gmt plot \"" << fs::canonical(outlinePath).string() << "\" -bi2f -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
        else
        {
          // Plot as text file, add -J and -R to match the grid
          scriptOfs << "gmt plot -JM" << map_w_str << " -R" << extentStr << " \"" << fs::canonical(outlinePath).string() << "\" -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
    }
    else if (plotCoast)
    {
      // Fall back to coastlines and country borders
      // -W shorelines, -N1 country borders
      scriptOfs << "gmt coast -JM" << map_w_str << " -R" << extentStr << " -D" << coastRes << " -N1/0.01p,gray77 -Wthinnest,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

    // Draw directional rose and scalebar
    scriptOfs << "gmt basemap -JM" << map_w_str << " -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=8p,Helvetica --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_LABEL=8p,Helvetica -Vq" << std::endl;

    string heightStr = "Altura (m)";
    // If lang option is set to "us", change heightStr to "Height (m)"
    if (lang == "us")
    {
      heightStr = "Height (m)";
    }

    // Draw color bar (height = 70% of target map height)
    string cbar_h_str = std::to_string((int)std::round(TARGET_MAP_H * 0.70f)) + "c";
    scriptOfs << "gmt colorbar  -JM" << map_w_str << " -R" << extentStr << " -DJMR+w" << cbar_h_str << "/0.4c+o20p/0p+v -C\"" << palettePath.string() << "\" -Bafg -Baf+l\"" << heightStr << "\" -Bxa1f1 -B+u\" m.\" --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    std::stringstream removeFilesSs;

    // Gauge circles on the map (gauges already loaded above for layout computation)
    if (hasGauges && hasWaveData && n_gauges > 0)
    {
      string gaugeSize = options.get("gauge_size");
      if (gaugeSize.empty()) gaugeSize = "2p";

      for (int i = 0; i < n_gauges; i++)
      {
        if (i >= (int)gauges.size()) break;
        string color = gaugeColors[i % gaugeColors.size()];
        fs::path markFile = fs::temp_directory_path() / ("mark_" + std::to_string(i) + ".txt");
        std::ofstream markOfs(markFile.string());
        markOfs << gauges.longitude[i] << " " << gauges.latitude[i] << endl;
        markOfs.close();

        scriptOfs << "gmt plot \"" << markFile.string() << "\" -Sc" << gaugeSize << " -W0.2p,black -G" << color << " -Vq" << std::endl;

#ifdef WIN32
        removeFilesSs << "del \"" << markFile.string() << "\"" << std::endl;
#elif __linux__
        removeFilesSs << "rm \"" << markFile.string() << "\"" << std::endl;
#endif
      }

      // Cleanup chunk legend files
      for (auto& chunkFile : chunkLegendFiles)
      {
#ifdef WIN32
        removeFilesSs << "del \"" << chunkFile.string() << "\"" << std::endl;
#elif __linux__
        removeFilesSs << "rm \"" << chunkFile.string() << "\"" << std::endl;
#endif
      }
    }

    // Legend below map: move origin back down, draw chunks side by side in a Cartesian frame.
    // -Y-{yUp}c undoes the upward shift applied to the first grdimage.
    // Each chunk has a fixed width (LEGEND_CHUNK_W_CM) and the whole table is centered
    // under the map.  All chunks anchor their TL at the top of the frame so content
    // starts immediately below the map regardless of how many gauges there are.
    if (legendActive)
    {
      const int LEGEND_CHUNK_W_CM = 20;
      int fixed_chunk_w = std::min(LEGEND_CHUNK_W_CM, map_w_i / std::max(1, n_chunks));
      int x_offset_i    = std::max(0, (map_w_i - n_chunks * fixed_chunk_w) / 2);
      string fixed_chunk_w_str = std::to_string(fixed_chunk_w) + "c";
      string totalW    = std::to_string(map_w_i) + "c";
      string legendHStr = std::to_string((int)legend_h) + "c";
      scriptOfs << "gmt legend \"" << chunkLegendFiles[0].string() << "\" -Rd -JX" << totalW << "/" << legendHStr << " -Y-" << yUp << "c -Dx" << x_offset_i << "c/" << legendHStr << "+jTL+w" << fixed_chunk_w_str << " --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_TITLE=8p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;
      for (int c = 1; c < (int)chunkLegendFiles.size(); c++)
      {
        scriptOfs << "gmt legend \"" << chunkLegendFiles[c].string() << "\" -Dx" << (x_offset_i + c * fixed_chunk_w) << "c/" << legendHStr << "+jTL+w" << fixed_chunk_w_str << " --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_TITLE=8p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;
      }
    }

    // End GMT modern session
    scriptOfs << "gmt end" << std::endl;

#ifdef __linux__
    // Clean up the temporary GMT user dir used for the AppArmor workaround.
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
#endif

#ifndef _DEBUG
#ifdef WIN32
    if (tempZMaxGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "del \"" << zMaxPlotPath.string() << "\"" << std::endl;
    }
    // Remove palette file
    removeFilesSs << "del \"" << palettePath.string() << "\"" << std::endl;
#elif __linux__
    if (tempZMaxGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "rm \"" << zMaxPlotPath.string() << "\"" << std::endl;
    }
    // Remove palette file
    removeFilesSs << "rm \"" << palettePath.string() << "\"" << std::endl;
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

    if (lang == "us")
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
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
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
    if (lang == "us")
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
      if (lang == "us")
      {
        timeAxisLabel = "Time (hr.)";
      }
    }

    string waveHeightAxisLabel = "Altura (m)";
    if (lang == "us")
    {
      waveHeightAxisLabel = "Height (m)";
    }

    int seriesCount = pointData.series_count();

    // Get the wave data from the time series
    float hFirstWave = 0.01f;                    // Minimum height to consider a first wave in meters (1 centimeter)
    float deltaFirstWave = 1.0f * timeResFactor; // Minimum time (minutes) to consider a first wave

    if (options.contains("h_first_wave"))
    {
      hFirstWave = options.getFloat("h_first_wave");
    }

    if (options.contains("delta_first_wave"))
    {
      deltaFirstWave = options.getFloat("delta_first_wave") * timeResFactor;
    }

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

      string imageFilename = options.get("wave_filename");
      if (!imageFilename.length())
      {
        imageFilename = "wave";
      }

      if (imageCount > 1)
      {
        imageFilename += "_" + std::to_string(imageNumber);
      }

      // Start GMT session for this image
      // Save as wave_{imageNumber}.png relative to waveImagePath
      scriptOfs << "gmt begin " << (waveImagePath / imageFilename).string() << " png E600" << std::endl;

      // set PS_LINE_CAP to ROUND
      scriptOfs << "gmt set PS_LINE_CAP ROUND" << std::endl;

      // set PS_LINE_JOIN to ROUND
      scriptOfs << "gmt set PS_LINE_JOIN ROUND" << std::endl;

      // annotate W (left), E (right), S (bottom); tick-only N (top)
      scriptOfs << "gmt set MAP_FRAME_AXES WSEn" << std::endl;


      cout << (waveImagePath / imageFilename).string() << ".png" << endl;

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

          // string color = linePalette[s % linePalette.size()];
          string color = linePalette[seriesIndex % linePalette.size()];

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
          tileLegendSs << "S R - 10p 1p," << color << " 1p," << color << endl;

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

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
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
   * @brief Create an Inundation Plot Script object
   * @param inundPath Inundation grid path
   * @param gridPath Grid path
   * @param options Geo options
   * @return fs::path
   */
  fs::path createInundationPlotScript(string inundPath, string gridPath, geo::Options &options)
  {

    fs::path palettePath = createInundationPaletteFile();

    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length())
    {
      lang = "es";
    }

    string langUpper = Strings::toupper(static_cast<string>(lang));

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

    string title = "Areas inundadas";

    if (lang == "us")
    {
      title = "Inundation Areas";
    }

    if (options.contains("source"))
    {
      title += " - " + options.get("source");
    }

    // Load zMax grid to get parameters
    Grid grid;
    fs::path inundGridPath(inundPath);
    fs::path inundPlotPath = inundGridPath;

    // Load grid and convert to Esri format
    if (loadGrid(grid, inundGridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << inundGridPath.string() << endl;
      return fs::path();
    }

    // Get grid parameters
    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    bool tempInundGridCreated = false;
    // if inund_nodata option is set, create a temporary grid and  replace nodata values
    if (!options.get("inund_nodata").empty())
    {
      float noDataValue = std::stof(options.get("inund_nodata"));

      for (size_t i = 0; i < rows; i++)
      {
        for (size_t j = 0; j < columns; j++)
        {
          if (grid(i, j) == noDataValue || grid(i,j) <= 0.0f)
          {
            grid(i, j) = 100.0f; // Replace NoData with 100.0f
          }
        }
      }

      grid.setNoData(100.0f);

      cout << "Replacing " << noDataValue << " with nodata" << endl;
      // Save temporary grid to a new file
      fs::path tempInundGridPath = fs::temp_directory_path() / (inundGridPath.stem().string() + "_nodata_fixed.bil");
      if (geo::SaveGrid(grid, tempInundGridPath.string(), GridFormat::ESRI_FLOAT) == geo::geoStatus::FAILURE)
      {
        cerr << "Unable to save temporary grid to " << tempInundGridPath.string() << endl;
        return fs::path();
      }
      inundPlotPath = tempInundGridPath;
      tempInundGridCreated = true;
    }

#ifdef _DEBUG
    cout << "Creating inundation plot script for " << inundGridPath.string() << endl;
#endif

    // Set output path to gridPath with .png extension
    fs::path outputPath = inundGridPath.replace_extension(".png");

    // Set file extension
    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    // Use filename from gridPath to create a .bat file inside fs::temp_directory_path()
    fs::path scriptPath = fs::temp_directory_path() / (fs::path(inundPlotPath).stem().string() + fileExt);

    // Generate legend from gauges
    fs::path legendFile = fs::temp_directory_path() / "inund_legend.txt";

    // Create batch file
    std::ofstream scriptOfs(scriptPath.string());

    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";

    // Send the commands to the batch file

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << ":: Auto-generated batch file to plot inundation grid " << inundPlotPath << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
    // Set GMT_LANGUAGE
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    // Set GMT_VERBOSE=quiet
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    // Set GMT_END_SHOW=off
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    // Set GMT language
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
#endif

    // Resolve satellite filename relative to the grid directory (same as outline logic).
    string satelliteFilename = options.get("satellite");
    string satellitePath;
    if (!satelliteFilename.empty())
    {
        fs::path satResolved(gridPath);
        satResolved.replace_filename(satelliteFilename);
        if (fs::exists(satResolved))
            satellitePath = fs::canonical(satResolved).string();
        else
            std::cerr << "Warning: satellite image not found: " << satResolved.string() << std::endl;
    }
    bool showBathy = Strings::tolower(options.get("show_bathy")) == "true";
    int waveTrans = 0;
    try { if (!options.get("wave_transparency").empty()) waveTrans = std::stoi(options.get("wave_transparency")); } catch (...) {}
    string waveTransArg = (waveTrans > 0) ? " -t" + std::to_string(waveTrans) : "";

    if (showBathy && satellitePath.empty())
    {
      string bathyCpt = options.get("bathy_cpt");
      if (bathyCpt.empty() || !std::all_of(bathyCpt.begin(), bathyCpt.end(),
          [](char c){ return std::isalnum((unsigned char)c) || c == '_' || c == '-'; }))
        bathyCpt = "gray";
      if (bathyCpt == "gray")
      {
        // Scale gray palette to the local data range; invert if requested so deep = dark.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt grd2cpt \"" << gridPath << "\" -Cgray"
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
      else
      {
        // Physical palettes (globe, geo, topo, ...) must NOT be rescaled to the local
        // data range or the depth-to-colour mapping is destroyed.  Use makecpt so the
        // standard anchoring (0 = sea level) is preserved.
        // When bathy_convention=tunami the grid stores positive=ocean, negative=land,
        // which is the opposite of what globe/geo expect.  Invert the entire palette
        // (-I flips both the z-axis and the colour sequence) so that positive values
        // get ocean colours and negative values get land colours.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt makecpt -C" << bathyCpt
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
    }

    // Create extent string
    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

    // Start GMT modern session
    scriptOfs << "gmt begin \"" << inundGridPath.replace_extension("").string() << "\" png E600" << std::endl;

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
    scriptOfs << "gmt set PS_LINE_CAP round" << std::endl;

    // set PS_LINE_JOIN to ROUND
    scriptOfs << "gmt set PS_LINE_JOIN round" << std::endl;

    // plot the legend file
    scriptOfs << "gmt legend " << legendFile << " -Rd -JX12c/9c -DjLB+o0c/-2c -F --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_TITLE=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;


    if (!satellitePath.empty())
    {
      scriptOfs << "gmt grdimage -JM10c -R" << extentStr << " \"" << satellitePath << "\" -Vq" << std::endl;
    }
    else if (showBathy)
    {
      scriptOfs << "gmt grdimage -JM10c -R" << extentStr << " \"" << gridPath << "\" -C\"" << bathymetryPalettePath.string() << "\" -t40 -Vq" << std::endl;
    }

    // Plot inundation grid
    scriptOfs << "gmt grdimage -JM10c  -R" << extentStr << " \"" << inundPlotPath.string() << "\" -C\"" << palettePath.string() << "\" -Qwhite" << waveTransArg << " -Vq" << std::endl;

    // Draw map frame and title
    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -Baf -BWSen+t\"" << title << "\" --FONT_TITLE=14p,Helvetica --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;

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
    // If outline option is set, plot outline file instead of coastlines

    // Outline is expected to be in the same directory as the grid file.
    // The outline option defines the filename of the outline file. If the outline option is not set, the default filename is "contour.gmt".
    fs::path outlinePath(gridPath);

    string outlineFilename = options.get("outline");

    if (outlineFilename.empty())
    {
      outlineFilename = "contour.gmt";
    }

    outlinePath.replace_filename(outlineFilename);

    // Check if outline file exists
    if (fs::exists(outlinePath))
    {
        cout << "Using outline file: " << fs::canonical(outlinePath).string() << endl;
        // If file is binary plot with -bi2f
        if (Strings::tolower(outlinePath.extension().string()) == ".bin" || Strings::tolower(outlinePath.extension().string()) == ".bf2")
        {
          scriptOfs << "gmt plot \"" << fs::canonical(outlinePath).string() << "\" -bi2f -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
        else
        {
          // Plot as text file, add -J and -R to match the grid
          scriptOfs << "gmt plot -JM10c -R" << extentStr << " \"" << fs::canonical(outlinePath).string() << "\" -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
    }
    else if (plotCoast)
    {
      // Fall back to coastlines and country borders
      // -W shorelines, -N1 country borders
      scriptOfs << "gmt coast -JM10c -R" << extentStr << " -D" << coastRes << " -N1/0.01p,gray77 -Wthinnest,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

    // Draw directional rose and scalebar
    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=6p,Helvetica --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    string heightStr = "Altura (m)";
    // If lang option is set to "us", change heightStr to "Height (m)"
    if (lang == "us")
    {
      heightStr = "Height (m)";
    }

    // Draw color bar
    scriptOfs << "gmt colorbar  -JM10c -R" << extentStr << " -DJMR+w6c/0.25c+o20p/0p+v -C\"" << palettePath.string() << "\" -Bafg -Baf+l\"" << heightStr << "\" -Bxa1f1 -B+u\" m.\" --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    std::ofstream legendOfs(legendFile.string());

    if (legendOfs.is_open() == false)
    {
      cerr << "Unable to create legend file " << legendFile.string() << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }
    // Define 1 column
    legendOfs << "N 1" << endl;
    // Define legends for data ranges

    legendOfs << "L - L Altura (m)" << endl;
    if (lang == "us")
    {
      legendOfs << "L - L Height (m)" << endl;
    }
    legendOfs << "S 0.25c s 0.25c 230/0/0 thinnest 0.5c 0.6m - 1m" << endl; // red - dark red
    legendOfs << "S 0.25c s 0.25c 255/153/0 thinnest 0.5c 0.4m - 0.6m" << endl; // orange-red
    legendOfs << "S 0.25c s 0.25c 255/230/0 thinnest 0.5c 0.2m - 0.4m" << endl; // yellow-orange
    legendOfs << "S 0.25c s 0.25c 0/0/255 thinnest 0.5c 0.01m - 0.2m" << endl; // blue
    legendOfs.close();

    legendOfs.close();

    // End GMT modern session
    scriptOfs << "gmt end" << std::endl;

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
#endif

#ifndef _DEBUG
   std::stringstream removeFilesSs;
#ifdef WIN32
    if (tempInundGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "del \"" << inundPlotPath.string() << "\"" << std::endl;
    }
    // Remove palette file
    removeFilesSs << "del \"" << palettePath.string() << "\"" << std::endl;
    // Remove temporary legend file
    removeFilesSs << "del \"" << legendFile.string() << "\"" << std::endl;
#elif __linux__
    if (tempInundGridCreated)
    {
      // Remove temporary zmax grid
      removeFilesSs << "rm \"" << inundPlotPath.string() << "\"" << std::endl;
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
   * @brief Create a shell script to plot the maximum velocity grid.
   * The color palette is auto-scaled to the data range using gmt grd2cpt
   * with the same color progression as the max-wave palette.
   */
  fs::path createVmaxPlotScript(string vmaxPath, string gridPath, geo::Options &options)
  {
    // palette_max_z does not apply here: grd2cpt -Z -D auto-scales to the data range
    fs::path maxPalettePath = createMaxPaletteFile(1.0f);

    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length())
    {
      lang = "es";
    }

    string langUpper = Strings::toupper(static_cast<string>(lang));

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

    string title = "Velocidad maxima";
    if (lang == "us")
    {
      title = "Maximum velocity";
    }
    if (options.contains("source"))
    {
      title += " - " + options.get("source");
    }

    Grid grid;
    fs::path vmaxGridPath(vmaxPath);
    fs::path vmaxPlotPath = vmaxGridPath;

    if (loadGrid(grid, vmaxGridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << vmaxGridPath.string() << endl;
      return fs::path();
    }

    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    bool tempVmaxGridCreated = false;
    if (!options.get("vmax_nodata").empty())
    {
      float noDataValue = std::stof(options.get("vmax_nodata"));

      for (size_t i = 0; i < rows; i++)
      {
        for (size_t j = 0; j < columns; j++)
        {
          if (grid(i, j) == noDataValue || grid(i, j) <= 0.0f)
          {
            grid(i, j) = 100.0f;
          }
        }
      }

      grid.setNoData(100.0f);

      cout << "Replacing " << noDataValue << " with nodata" << endl;
      fs::path tempVmaxGridPath = fs::temp_directory_path() / (vmaxGridPath.stem().string() + "_nodata_fixed.bil");
      if (geo::SaveGrid(grid, tempVmaxGridPath.string(), GridFormat::ESRI_FLOAT) == geo::geoStatus::FAILURE)
      {
        cerr << "Unable to save temporary grid to " << tempVmaxGridPath.string() << endl;
        return fs::path();
      }
      vmaxPlotPath = tempVmaxGridPath;
      tempVmaxGridCreated = true;
    }

    fs::path outputPath = vmaxGridPath.replace_extension(".png");

    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    fs::path scriptPath = fs::temp_directory_path() / (fs::path(vmaxPlotPath).stem().string() + fileExt);
    fs::path legendFile = fs::temp_directory_path() / "vmax_legend.txt";
    fs::path bathymetryPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";
    fs::path vmaxPalettePath = fs::temp_directory_path() / "vmax_palette.cpt";

    std::ofstream scriptOfs(scriptPath.string());

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << ":: Auto-generated batch file to plot vmax grid " << vmaxPlotPath << std::endl;
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << std::endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << std::endl;
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << std::endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << std::endl;
#endif

    // Resolve satellite filename relative to the grid directory (same as outline logic).
    string satelliteFilename = options.get("satellite");
    string satellitePath;
    if (!satelliteFilename.empty())
    {
        fs::path satResolved(gridPath);
        satResolved.replace_filename(satelliteFilename);
        if (fs::exists(satResolved))
            satellitePath = fs::canonical(satResolved).string();
        else
            std::cerr << "Warning: satellite image not found: " << satResolved.string() << std::endl;
    }
    bool showBathy = Strings::tolower(options.get("show_bathy")) == "true";
    int waveTrans = 0;
    try { if (!options.get("wave_transparency").empty()) waveTrans = std::stoi(options.get("wave_transparency")); } catch (...) {}
    string waveTransArg = (waveTrans > 0) ? " -t" + std::to_string(waveTrans) : "";

    if (showBathy && satellitePath.empty())
    {
      string bathyCpt = options.get("bathy_cpt");
      if (bathyCpt.empty() || !std::all_of(bathyCpt.begin(), bathyCpt.end(),
          [](char c){ return std::isalnum((unsigned char)c) || c == '_' || c == '-'; }))
        bathyCpt = "gray";
      if (bathyCpt == "gray")
      {
        // Scale gray palette to the local data range; invert if requested so deep = dark.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt grd2cpt \"" << gridPath << "\" -Cgray"
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
      else
      {
        // Physical palettes (globe, geo, topo, ...) must NOT be rescaled to the local
        // data range or the depth-to-colour mapping is destroyed.  Use makecpt so the
        // standard anchoring (0 = sea level) is preserved.
        // When bathy_convention=tunami the grid stores positive=ocean, negative=land,
        // which is the opposite of what globe/geo expect.  Invert the entire palette
        // (-I flips both the z-axis and the colour sequence) so that positive values
        // get ocean colours and negative values get land colours.
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt makecpt -C" << bathyCpt
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathymetryPalettePath.string() << "\"" << std::endl;
      }
    }

    // Auto-scale the max-wave color progression to the vmax data range
    scriptOfs << "gmt grd2cpt \"" << vmaxPlotPath.string() << "\" -C\"" << maxPalettePath.string() << "\" -Z > \"" << vmaxPalettePath.string() << "\"" << std::endl;

    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

    scriptOfs << "gmt begin \"" << vmaxGridPath.replace_extension("").string() << "\" png E600" << std::endl;

    scriptOfs << "gmt set MAP_FRAME_TYPE fancy+" << std::endl;
    scriptOfs << "gmt set MAP_FRAME_WIDTH 2.5p" << std::endl;
    scriptOfs << "gmt set GMT_LANGUAGE " << langUpper << std::endl;
    scriptOfs << "gmt set PS_LINE_CAP round" << std::endl;
    scriptOfs << "gmt set PS_LINE_JOIN round" << std::endl;

    // Skip legend
    // scriptOfs << "gmt legend " << legendFile << " -Rd -JX12c/9c -DjLT -F --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_TITLE=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    if (!satellitePath.empty())
    {
      scriptOfs << "gmt grdimage -JM10c -R" << extentStr << " \"" << satellitePath << "\" -Vq" << std::endl;
    }
    else if (showBathy)
    {
      scriptOfs << "gmt grdimage -JM10c -R" << extentStr << " \"" << gridPath << "\" -C\"" << bathymetryPalettePath.string() << "\" -t40 -Vq" << std::endl;
    }

    scriptOfs << "gmt grdimage -JM10c  -R" << extentStr << " \"" << vmaxPlotPath.string() << "\" -C\"" << vmaxPalettePath.string() << "\" -Qwhite" << waveTransArg << " -Vq" << std::endl;

    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -Baf -BWSen+t\"" << title << "\" --FONT_TITLE=14p,Helvetica --FONT_ANNOT=6p,Helvetica -Vq" << std::endl;

    double gridLengthX = columns * dxM;
    double gridLengthY = rows * dyM;
    double gridLengthKm = std::min(gridLengthX, gridLengthY) / 1000.0;

    float scaleLengthKm = trunc(gridLengthKm / 5.0f);
    if (scaleLengthKm < 1)
    {
      scaleLengthKm = 0.1;
    }
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

    // Draw coast lines
    // If outline option is set, plot outline file instead of coastlines

    // Outline is expected to be in the same directory as the grid file.
    // The outline option defines the filename of the outline file. If the outline option is not set, the default filename is "contour.gmt".
    fs::path outlinePath(gridPath);

    string outlineFilename = options.get("outline");

    if (outlineFilename.empty())
    {
      outlineFilename = "contour.gmt";
    }

    outlinePath.replace_filename(outlineFilename);

    // Check if outline file exists
    if (fs::exists(outlinePath))
    {
        cout << "Using outline file: " << fs::canonical(outlinePath).string() << endl;
        // If file is binary plot with -bi2f
        if (Strings::tolower(outlinePath.extension().string()) == ".bin" || Strings::tolower(outlinePath.extension().string()) == ".bf2")
        {
          scriptOfs << "gmt plot \"" << fs::canonical(outlinePath).string() << "\" -bi2f -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
        else
        {
          // Plot as text file, add -J and -R to match the grid
          scriptOfs << "gmt plot -JM10c -R" << extentStr << " \"" << fs::canonical(outlinePath).string() << "\" -Wthin,dimgray --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
        }
    }
    else if (plotCoast)
    {
      // Fall back to coastlines and country borders
      // -W shorelines, -N1 country borders
      scriptOfs << "gmt coast -JM10c -R" << extentStr << " -D" << coastRes << " -N1/0.01p,gray77 -Wthinnest,dimgray  --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << std::endl;
    }

    scriptOfs << "gmt basemap -JM10c -R" << extentStr << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0 << "+l+f+w" << scaleLengthKm << "k+o0p/30p --FONT_TITLE=6p,Helvetica --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    string velStr = "Vel. (m/s)";
    if (lang == "us")
    {
      velStr = "Vel. (m/s)";
    }

    scriptOfs << "gmt colorbar  -JM10c -R" << extentStr << " -DJMR+w6c/0.25c+o20p/0p+v -C\"" << vmaxPalettePath.string() << "\" -Bafg -Baf+l\"" << velStr << "\" -Bxa1f1 -B+u\" m/s\" --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << std::endl;

    std::ofstream legendOfs(legendFile.string());
    if (legendOfs.is_open() == false)
    {
      cerr << "Unable to create legend file " << legendFile.string() << endl;
      scriptOfs.close();
      fs::remove(scriptPath);
      return fs::path();
    }
    legendOfs << "N 1" << endl;
    legendOfs << "L - L Velocidad max. (m/s)" << endl;
    if (lang == "us")
    {
      legendOfs << "L - L Max. velocity (m/s)" << endl;
    }
    legendOfs.close();

    scriptOfs << "gmt end" << std::endl;

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << std::endl;
#endif

#ifndef _DEBUG
    std::stringstream removeFilesSs;
#ifdef WIN32
    if (tempVmaxGridCreated)
    {
      removeFilesSs << "del \"" << vmaxPlotPath.string() << "\"" << std::endl;
    }
    removeFilesSs << "del \"" << vmaxPalettePath.string() << "\"" << std::endl;
    removeFilesSs << "del \"" << maxPalettePath.string() << "\"" << std::endl;
    removeFilesSs << "del \"" << legendFile.string() << "\"" << std::endl;
#elif __linux__
    if (tempVmaxGridCreated)
    {
      removeFilesSs << "rm \"" << vmaxPlotPath.string() << "\"" << std::endl;
    }
    removeFilesSs << "rm \"" << vmaxPalettePath.string() << "\"" << std::endl;
    removeFilesSs << "rm \"" << maxPalettePath.string() << "\"" << std::endl;
    removeFilesSs << "rm \"" << legendFile.string() << "\"" << std::endl;
#endif
    scriptOfs << removeFilesSs.str();
#endif

    scriptOfs.close();

#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
#endif

#ifdef _DEBUG
    cout << scriptPath.string() << endl;
#endif

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

  void plotBathy(geo::Options &options)
  {
    auto [inputPath, outputPath] = getPaths(options);

    fs::path bathymetryPath = fs::path(options.get("grid"));

    Grid bathymetryGrid;
    if (loadGrid(bathymetryGrid, bathymetryPath, options, true) == false)
    {
      cerr << "Unable to load bathymetry grid from " << options.get("grid") << endl;
      return;
    }

    cout << "Plotting bathymetry grid " << bathymetryPath.string() << " ..." << std::endl;

    // loadGrid with convertToEsri=true replaces the extension to .bil
    bathymetryPath = bathymetryPath.replace_extension(".bil");

    fs::path scriptPath = createBathyPlotScript(bathymetryPath.string(), options);
    if (scriptPath.empty()) return;

    int exitCode = executeCommand(scriptPath.string(), true);

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

    fs::path pngPath = bathymetryPath.parent_path() / (bathymetryPath.stem().string() + "_bathy.png");
    cout << "Bathy " << pngPath.string() << " created" << std::endl;
  }

  void plotZMax(geo::Options &options)
  {
    using geo::GridFormat;

    fs::path gridPath;
    fs::path zMaxPath;
    fs::path gaugePath;

    auto [inputPath, outputPath] = getPaths(options);

    // If output path is relative, set relative to inputPath
    if (!inputPath.empty() && !outputPath.empty() && outputPath.is_relative())
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

    if (!waveDataPath.empty())
    {
      if (!fs::exists(waveDataPath) && fs::exists(outputPath / waveDataPath))
      {
        waveDataPath = (outputPath / waveDataPath).make_preferred();
      }
      if (!fs::exists(waveDataPath))
      {
        cerr << "Wave data file " << waveDataPath.string() << " not found, plotting without gauge table." << endl;
        waveDataPath = fs::path();
      }
      else
      {
        waveDataPath = fs::canonical(waveDataPath);
      }
    }

    // Get gauge path -- optional; grids without outpoints produce a map with no gauge markers
    if (options.contains("gauge") && !options.get("gauge").empty())
    {
      gaugePath = fs::path(options.get("gauge")).make_preferred();
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
        cerr << "Gauges file " << gaugePath.string() << " not found, plotting without gauges." << endl;
        gaugePath = fs::path();
      }
      else
      {
        gaugePath = fs::canonical(gaugePath);
      }
    }

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

  void plotInundation(geo::Options &options)
  {
    using geo::GridFormat;

    fs::path gridPath;
    fs::path inundationPath;

    auto [inputPath, outputPath] = getPaths(options);

    // If output path is relative, set relative to inputPath
    if (!inputPath.empty() && !outputPath.empty() && outputPath.is_relative())
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
    inundationPath = fs::path("inund" + geo::gridExtension(gridOutputFormat));
    if (options.contains("inund"))
    {
      inundationPath = options.get("inund");
    }

    if (!fs::exists(inundationPath) && fs::exists(outputPath / inundationPath))
    {
      inundationPath = (outputPath / inundationPath).make_preferred();
    }

    if (!fs::exists(inundationPath))
    {
      cerr << "Inundation grid " << inundationPath.string() << " does not exist." << endl;
      return;
    }

    // Convert inundationPath to canonical
    inundationPath = fs::canonical(inundationPath);

    cout << "Plotting inundation grid " << inundationPath.string() << " ..." << std::endl;

    // Get inundation grid
    Grid inundationGrid;

    if (loadGrid(inundationGrid, inundationPath, options, true) == false)
    {
      cerr << "Unable to load inundation grid from " << inundationPath.string() << endl;
      return;
    }

    // Create bat or sh file depending on OS
    fs::path scriptPath = createInundationPlotScript(inundationPath.string(), gridPath.string(), options);

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
    cout << "Inundation " << inundationPath.replace_extension("png").string() << std::endl;
  }

  void plotVmax(geo::Options &options)
  {
    using geo::GridFormat;

    fs::path gridPath;
    fs::path vmaxPath;

    auto [inputPath, outputPath] = getPaths(options);

    if (!inputPath.empty() && !outputPath.empty() && outputPath.is_relative())
    {
      outputPath = (inputPath / outputPath).make_preferred();
    }

    GridFormat gridOutputFormat{GridFormat::ESRI_FLOAT};
    if (options.contains("format"))
    {
      GridFormat suggestedFormat = geo::getFormat(options.get("format"));
      if (suggestedFormat != GridFormat::UNKNOWN)
      {
        gridOutputFormat = suggestedFormat;
      }
    }

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

    gridPath = fs::canonical(gridPath);

    Grid grid;
    if (loadGrid(grid, gridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << gridPath.string() << endl;
      return;
    }

    vmaxPath = fs::path("vmax" + geo::gridExtension(gridOutputFormat));
    if (options.contains("vmax"))
    {
      vmaxPath = options.get("vmax");
    }

    if (!fs::exists(vmaxPath) && fs::exists(outputPath / vmaxPath))
    {
      vmaxPath = (outputPath / vmaxPath).make_preferred();
    }

    if (!fs::exists(vmaxPath))
    {
      cerr << "Vmax grid " << vmaxPath.string() << " does not exist." << endl;
      return;
    }

    vmaxPath = fs::canonical(vmaxPath);

    cout << "Plotting vmax grid " << vmaxPath.string() << " ..." << std::endl;

    Grid vmaxGrid;
    if (loadGrid(vmaxGrid, vmaxPath, options, true) == false)
    {
      cerr << "Unable to load vmax grid from " << vmaxPath.string() << endl;
      return;
    }

    fs::path scriptPath = createVmaxPlotScript(vmaxPath.string(), gridPath.string(), options);

    int exitCode = executeCommand(scriptPath.string(), true);

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
    cout << "Vmax " << vmaxPath.replace_extension("png").string() << std::endl;
  }

  fs::path createPropagationPlotScript(string gridPath, string propPath, geo::Options &options)
  {
    string lang = Strings::tolower(options.get("lang"));
    if (!lang.length()) lang = "es";
    string langUpper = Strings::toupper(static_cast<string>(lang));

    bool plotCoast = Strings::tolower(options.get("plot_coast")) != "false";
    string coastRes = options.get("coast_resolution");
    if (coastRes.size() != 1) coastRes = "f";

    string title = (lang == "us") ? "Tsunami Travel Time" : "Tiempo de Propagacion del Tsunami";
    if (options.contains("source"))
      title += " - " + options.get("source");

    int maxTime = 40;
    if (!options.get("time").empty())
    {
      try { maxTime = std::stoi(options.get("time")); }
      catch (...) {}
    }

    Grid grid;
    fs::path propGridPath(propPath);
    fs::path propPlotPath = propGridPath;

    if (loadGrid(grid, propGridPath, options, true) == false)
    {
      cerr << "Unable to load propagation grid from " << propGridPath.string() << endl;
      return fs::path();
    }

    auto [x0, y0, xMax, yMax] = grid.extents();
    auto [dxDeg, dyDeg] = grid.resolutionDegrees();
    auto [dxM, dyM] = grid.resolutionMeters();
    auto [rows, columns] = grid.dimensions();

    const float TARGET_MAP_H = 20.0f;
    const int MIN_MAP_W = 15;
    float lon_span = (float)(columns * dxDeg);
    float lat_span = (float)(rows * dyDeg);
    int map_w_i = (lat_span > 0.0f)
                    ? std::max(MIN_MAP_W, (int)std::round(TARGET_MAP_H * (lon_span / lat_span)))
                    : MIN_MAP_W;
    string map_w_str = std::to_string(map_w_i) + "c";

    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif
    fs::path scriptPath = fs::temp_directory_path() / (fs::path(propPath).stem().string() + fileExt);
    std::ofstream scriptOfs(scriptPath.string());

    fs::path propPalettePath = fs::temp_directory_path() / "prop.cpt";

    string extentStr = std::to_string(x0) + "/" + std::to_string(xMax) + "/" + std::to_string(y0) + "/" + std::to_string(yMax);

#ifdef WIN32
    scriptOfs << "@echo off" << endl;
    scriptOfs << "set \"GMT_END_SHOW=off\"" << endl;
    scriptOfs << "set \"GMT_LANGUAGE=" << langUpper << "\"" << endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << endl;
    scriptOfs << "export GMT_VERBOSE=quiet" << endl;
    scriptOfs << "export GMT_END_SHOW=off" << endl;
    scriptOfs << "export GMT_LANGUAGE=" << langUpper << endl;
    scriptOfs << "mkdir -p \"/tmp/gmt_user_$$\"" << endl;
    scriptOfs << "export GMT_USERDIR=\"/tmp/gmt_user_$$\"" << endl;
#endif

    // Resolve satellite and bathy options
    string satFilename = options.get("satellite");
    string satPath;
    if (!satFilename.empty())
    {
      fs::path satResolved(gridPath);
      satResolved.replace_filename(satFilename);
      if (fs::exists(satResolved))
        satPath = fs::canonical(satResolved).string();
      else
        cerr << "Warning: satellite image not found: " << satResolved.string() << endl;
    }
    bool showBathy = Strings::tolower(options.get("show_bathy")) == "true";
    bool hasBackground = !satPath.empty() || showBathy;

    fs::path bathyPalettePath = fs::temp_directory_path() / "bathymetry_palette.cpt";
    if (showBathy && satPath.empty())
    {
      string bathyCpt = options.get("bathy_cpt");
      if (bathyCpt.empty() || !std::all_of(bathyCpt.begin(), bathyCpt.end(),
          [](char c){ return std::isalnum((unsigned char)c) || c == '_' || c == '-'; }))
        bathyCpt = "gray";
      if (bathyCpt == "gray")
      {
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt grd2cpt \"" << gridPath << "\" -Cgray"
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathyPalettePath.string() << "\"" << endl;
      }
      else
      {
        bool bathyInvert = Strings::tolower(options.get("bathy_convention")) == "tunami";
        scriptOfs << "gmt makecpt -C" << bathyCpt
                  << (bathyInvert ? " -I" : "") << " -D > \"" << bathyPalettePath.string() << "\"" << endl;
      }
    }

    // CPT: jet inverted (red=near/source, blue=far), scaled 0..maxTime hours, 1h steps.
    // When a background layer is present, NaN (land/unreached ocean) is made transparent
    // via -Q so the background shows through.  Without a background, COLOR_NAN=lightgray
    // gives a distinct third visual category between reached-ocean and land.
    string colorNanArg = hasBackground ? "" : " --COLOR_NAN=lightgray";
    scriptOfs << "gmt makecpt -Cjet -I -T0/" << maxTime << "/1 -Z" << colorNanArg << " > \"" << propPalettePath.string() << "\"" << endl;

    scriptOfs << "gmt begin \"" << propGridPath.replace_extension("").string() << "\" png E600" << endl;

    scriptOfs << "gmt set MAP_FRAME_TYPE fancy+" << endl;
    scriptOfs << "gmt set MAP_FRAME_WIDTH 2.5p" << endl;
    scriptOfs << "gmt set GMT_LANGUAGE " << langUpper << endl;
    scriptOfs << "gmt set PS_LINE_CAP round" << endl;
    scriptOfs << "gmt set PS_LINE_JOIN round" << endl;

    // Background layer (satellite or bathymetry) rendered before propagation raster
    if (!satPath.empty())
    {
      scriptOfs << "gmt grdimage -JM" << map_w_str << " -R" << extentStr
                << " \"" << satPath << "\" -Vq" << endl;
    }
    else if (showBathy)
    {
      scriptOfs << "gmt grdimage -JM" << map_w_str << " -R" << extentStr
                << " \"" << gridPath << "\" -C\"" << bathyPalettePath.string() << "\" -t40 -Vq" << endl;
    }

    // Propagation time raster; -Q makes NaN transparent when a background is present.
    string propQArg = hasBackground ? " -Q" : " -Qwhite";
    scriptOfs << "gmt grdimage -JM" << map_w_str << " -R" << extentStr
              << " \"" << propPlotPath.string() << "\" -C\"" << propPalettePath.string()
              << "\"" << propQArg << " -Vq" << endl;

    // Scale bar length
    double gridLengthKm = std::min((double)(columns * dxM), (double)(rows * dyM)) / 1000.0;
    float scaleLengthKm = trunc(gridLengthKm / 5.0f);
    if (scaleLengthKm < 1) scaleLengthKm = 0.1f;
    if (scaleLengthKm < 1)      scaleLengthKm = (scaleLengthKm < 0.5f) ? 0.5f : 1.0f;
    else if (scaleLengthKm < 10)   scaleLengthKm = round(scaleLengthKm);
    else if (scaleLengthKm < 100)  scaleLengthKm = round(scaleLengthKm / 5.0f) * 5.0f;
    else if (scaleLengthKm < 1000) scaleLengthKm = round(scaleLengthKm / 50.0f) * 50.0f;
    else                           scaleLengthKm = round(scaleLengthKm / 500.0f) * 500.0f;

    // Coast or outline drawn after raster.  Without a background layer, -Gwhite fills
    // land to cover nodata cells.  With a background, land is already visible through
    // the transparent NaN propagation cells, so -Gwhite is omitted and only coastlines
    // and borders are drawn.
    fs::path outlinePath(gridPath);
    string outlineFilename = options.get("outline");
    if (outlineFilename.empty()) outlineFilename = "contour.gmt";
    outlinePath.replace_filename(outlineFilename);

    if (fs::exists(outlinePath))
    {
      if (Strings::tolower(outlinePath.extension().string()) == ".bin" ||
          Strings::tolower(outlinePath.extension().string()) == ".bf2")
        scriptOfs << "gmt plot \"" << fs::canonical(outlinePath).string() << "\" -bi2f -Wthin,dimgray -Vq" << endl;
      else
        scriptOfs << "gmt plot -JM" << map_w_str << " -R" << extentStr
                  << " \"" << fs::canonical(outlinePath).string() << "\" -Wthin,dimgray -Vq" << endl;
    }
    else if (plotCoast)
    {
      string landFill = hasBackground ? "" : " -Gwhite";
      scriptOfs << "gmt coast -JM" << map_w_str << " -R" << extentStr
                << " -D" << coastRes << landFill << " -N1/0.01p,gray77 -Wthinnest,dimgray"
                << " --FONT_ANNOT_PRIMARY=8p,Helvetica -Vq" << endl;
    }

    // Isolines every 2h, labeled every 4h with "h" suffix and bordered white box.
    // Dashed pen for minor contours, slightly thicker for annotated ones.
    scriptOfs << "gmt grdcontour \"" << propPlotPath.string() << "\" -JM" << map_w_str
              << " -R" << extentStr
              << " -C2 -A4+f8p,black+gwhite+p0.3p,gray40+uh -Wcthin,gray30,- -Wa0.8p,gray20,- -Vq" << endl;

    // Source star marker: parsed from "fault = single;lon lat depth ..."
    {
      string faultStr = options.get("fault");
      size_t sep = faultStr.find(';');
      if (sep != string::npos)
      {
        std::istringstream iss(faultStr.substr(sep + 1));
        float faultLon, faultLat;
        if (iss >> faultLon >> faultLat)
        {
          scriptOfs << "echo " << faultLon << " " << faultLat
                    << " | gmt plot -Sa0.7c -Gyellow -W1.5p,black -Vq" << endl;
        }
      }
    }

    // Frame and title
    scriptOfs << "gmt basemap -JM" << map_w_str << " -R" << extentStr
              << " -Baf -BWSen+t\"" << title << "\" --FONT_TITLE=14p,Helvetica --FONT_ANNOT=6p,Helvetica -Vq" << endl;

    // Directional rose and scale bar
    scriptOfs << "gmt basemap -JM" << map_w_str << " -R" << extentStr
              << " -TdjLT+w30p+f2+l,,,,+o5p/5p -LJBC+c" << y0
              << "+l+f+w" << scaleLengthKm << "k+o0p/30p"
              << " --FONT_TITLE=8p,Helvetica --FONT_ANNOT_PRIMARY=8p,Helvetica --FONT_LABEL=8p,Helvetica -Vq" << endl;

    // Colorbar: travel time in hours
    string timeLabel = (lang == "us") ? "Travel time (h)" : "Tiempo de llegada (h)";
    string cbar_h_str = std::to_string((int)std::round(TARGET_MAP_H * 0.70f)) + "c";
    scriptOfs << "gmt colorbar -JM" << map_w_str << " -R" << extentStr
              << " -DJMR+w" << cbar_h_str << "/0.4c+o20p/0p+v -C\"" << propPalettePath.string()
              << "\" -Baf+l\"" << timeLabel << "\" --FONT_ANNOT_PRIMARY=6p,Helvetica --FONT_LABEL=6p,Helvetica -Vq" << endl;

    scriptOfs << "gmt end" << endl;

#ifdef __linux__
    scriptOfs << "rm -rf \"/tmp/gmt_user_$$\"" << endl;
    scriptOfs << "rm -f \"" << propPalettePath.string() << "\"" << endl;
#endif

    scriptOfs.close();

#ifdef __linux__
    fs::permissions(scriptPath, fs::perms::owner_exec, fs::perm_options::add);
#endif

    return scriptPath;
  }

  void plotPropagation(geo::Options &options)
  {
    using geo::GridFormat;

    fs::path gridPath;
    fs::path propPath;

    auto [inputPath, outputPath] = getPaths(options);

    if (!inputPath.empty() && !outputPath.empty() && outputPath.is_relative())
      outputPath = (inputPath / outputPath).make_preferred();

    GridFormat gridOutputFormat{GridFormat::ESRI_FLOAT};
    if (options.contains("format"))
    {
      GridFormat suggestedFormat = geo::getFormat(options.get("format"));
      if (suggestedFormat != GridFormat::UNKNOWN)
        gridOutputFormat = suggestedFormat;
    }

    gridPath = fs::path(options.get("grid"));
    if (!fs::exists(gridPath) && fs::exists(inputPath / gridPath))
      gridPath = (inputPath / gridPath).make_preferred();
    else if (!fs::exists(gridPath) && fs::exists(outputPath / gridPath))
      gridPath = (outputPath / gridPath).make_preferred();

    if (!fs::exists(gridPath))
    {
      cerr << "Grid " << gridPath.string() << " does not exist." << endl;
      return;
    }
    gridPath = fs::canonical(gridPath);

    Grid grid;
    if (loadGrid(grid, gridPath, options, true) == false)
    {
      cerr << "Unable to load grid from " << gridPath.string() << endl;
      return;
    }

    propPath = fs::path("prop" + geo::gridExtension(gridOutputFormat));
    if (options.contains("prop"))
      propPath = options.get("prop");

    if (!fs::exists(propPath) && fs::exists(outputPath / propPath))
      propPath = (outputPath / propPath).make_preferred();

    if (!fs::exists(propPath))
    {
      cerr << "Propagation grid " << propPath.string() << " does not exist." << endl;
      return;
    }
    propPath = fs::canonical(propPath);

    cout << "Plotting propagation grid " << propPath.string() << " ..." << endl;

    Grid propGrid;
    if (loadGrid(propGrid, propPath, options, true) == false)
    {
      cerr << "Unable to load propagation grid from " << propPath.string() << endl;
      return;
    }

    fs::path scriptPath = createPropagationPlotScript(gridPath.string(), propPath.string(), options);

    int exitCode = executeCommand(scriptPath.string(), true);

#ifndef _DEBUG
    try { fs::remove(scriptPath); }
    catch (fs::filesystem_error &e) { cerr << "Error removing temporary script file: " << e.what() << endl; }
#endif

    if (exitCode != 0)
    {
      cerr << "Plot script failed with exit code " << exitCode << endl;
      return;
    }
    cout << "Propagation " << propPath.replace_extension("png").string() << endl;
  }

  /**
   * @brief Find elevation snapshot grids in a directory
   * @param dir Directory to search
   * @param prefix Filename prefix (e.g. "elev" or "elr3")
   * @param digits Number of zero-padded digits in the frame index
   * @return vector<fs::path> Sorted list of matching frame file paths
   */
  vector<fs::path> findElevationFrames(const fs::path &dir, const string &prefix, int digits)
  {
    vector<fs::path> frames;

    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
      return frames;
    }

    for (auto &entry : fs::directory_iterator(dir))
    {
      if (!entry.is_regular_file())
      {
        continue;
      }

      string fname = entry.path().filename().string();
      if (fname.size() <= prefix.size() + static_cast<size_t>(digits))
      {
        continue;
      }
      if (fname.compare(0, prefix.size(), prefix) != 0)
      {
        continue;
      }

      bool numericOk = true;
      for (int i = 0; i < digits; i++)
      {
        if (!std::isdigit(static_cast<unsigned char>(fname[prefix.size() + i])))
        {
          numericOk = false;
          break;
        }
      }
      if (!numericOk)
      {
        continue;
      }

      // Only accept known georeferenced grid extensions -- skips sibling
      // .hdr/.prj/.png files and the original (non-georeferenced) elr .dat.
      string ext = fname.substr(prefix.size() + digits);
      if (ext != ".bil" && ext != ".asc" && ext != ".flt" && ext != ".grd")
      {
        continue;
      }

      frames.push_back(entry.path());
    }

    std::sort(frames.begin(), frames.end());

    return frames;
  }

  /**
   * @brief Extract the zero-padded numeric frame index from a snapshot filename
   * @param filename File name (e.g. "elev00042.bil")
   * @param prefix Filename prefix
   * @param digits Number of digits in the index
   * @return int Frame index, or -1 if the filename does not match
   */
  int parseFrameIndex(const string &filename, const string &prefix, int digits)
  {
    if (filename.size() <= prefix.size() + static_cast<size_t>(digits))
    {
      return -1;
    }
    if (filename.compare(0, prefix.size(), prefix) != 0)
    {
      return -1;
    }

    try
    {
      return std::stoi(filename.substr(prefix.size(), digits));
    }
    catch (...)
    {
      return -1;
    }
  }

  /**
   * @brief Format an elapsed-time subtitle "+HH:MM:00" from a frame index
   * @param frameIndex Frame index (0-based)
   * @param dt Simulation dt (seconds/step)
   * @param interval Steps between snapshots
   * @return string Formatted timestamp
   */
  string formatElevationTimestamp(int frameIndex, float dt, int interval)
  {
    // dt may be sub-second (near-field scenarios commonly use dt < 1s), so
    // the multiplication must stay in floating point -- truncating dt to an
    // int first would silently zero every timestamp for those scenarios.
    int seconds = static_cast<int>(static_cast<float>(frameIndex) * dt * static_cast<float>(interval));
    int hours = seconds / 3600;
    int minutes = (seconds / 60) % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "+%02d:%02d:00", hours, minutes);

    return string(buf);
  }

  /**
   * @brief Render a scenario's elevation snapshot sequence as a titled/timestamped
   * 2D flat-map (Mercator) video
   * @param options Geo options
   *
   * Required options: output (result directory), source (title), dt
   * (seconds/step), elev_interval (steps between snapshots).
   * Optional: elev_prefix (default "elev"), elev_digits (default 5),
   * palette_max_z (default 1.0), animate_format/animate_fps/
   * animate_speed_factor/animate_out.
   */
  void plotElevationAnimation2D(geo::Options &options)
  {
    auto [inputPath, outputPath] = getPaths(options);

    string title = options.get("source");

    float dt = options.contains("dt") ? options.getFloat("dt") : 1.0f;
    int interval = options.contains("elev_interval") ? options.getInt("elev_interval") : 1;
    string prefix = options.contains("elev_prefix") ? options.get("elev_prefix") : "elev";
    int digits = options.contains("elev_digits") ? options.getInt("elev_digits") : 5;

    vector<fs::path> frames = findElevationFrames(outputPath, prefix, digits);
    if (frames.empty())
    {
      cerr << "No " << prefix << " snapshot grids found in " << outputPath.string() << endl;
      return;
    }

    float paletteMaxZ = 1.0f;
    if (options.contains("palette_max_z"))
    {
      try { paletteMaxZ = std::stof(options.get("palette_max_z")); }
      catch (...) { cerr << "Warning: invalid palette_max_z value, using 1.0" << endl; }
    }
    fs::path palettePath = createMaxPaletteFile(paletteMaxZ);

    // createMaxPaletteFile's first breakpoint runs white(0) -> darkblue(1% of
    // maxZ): fine for zmax (never near zero once touched), but an
    // instantaneous elevation snapshot has tiny floating-point noise
    // straddling zero everywhere the wave hasn't meaningfully arrived, which
    // that narrow white/darkblue transition turns into visible speckle.
    // Clip |elevation| below this threshold to NaN (renders as the palette's
    // "N" color, same as land) so calm, untouched ocean reads as clean
    // background instead of noise.
    float maskThresh = options.contains("animate_mask_thresh") ? options.getFloat("animate_mask_thresh") : 0.03f;

    cout << "Plotting " << frames.size() << " elevation frames from " << outputPath.string() << " ..." << endl;

    int plotted = 0;
    for (auto &framePath : frames)
    {
      fs::path gridPath = framePath;
      Grid grid;
      if (loadGrid(grid, gridPath, options, true) == false)
      {
        cerr << "Unable to load elevation frame " << framePath.string() << ", skipping." << endl;
        continue;
      }

      fs::path maskedPath = fs::temp_directory_path() / (gridPath.stem().string() + "_masked.grd");
      {
        std::ostringstream clipCmd;
        clipCmd << "gmt grdclip \"" << gridPath.string() << "\" -Si-" << maskThresh << "/" << maskThresh
                << "/NaN -G\"" << maskedPath.string() << "\"";
        executeCommand(clipCmd.str(), false);
      }

      int frameIndex = parseFrameIndex(framePath.filename().string(), prefix, digits);
      string timestamp = formatElevationTimestamp(frameIndex, dt, interval);

      fs::path outBase = gridPath;
      outBase.replace_extension("");

      fs::path scriptPath = createGridPlotScript(maskedPath.string(), palettePath.string(), title, timestamp, outBase.string());

      int exitCode = executeCommand(scriptPath.string(), true);

#ifndef _DEBUG
      try { fs::remove(scriptPath); }
      catch (fs::filesystem_error &e) { cerr << "Error removing temporary script file: " << e.what() << endl; }
      try { fs::remove(maskedPath); }
      catch (fs::filesystem_error &e) { cerr << "Error removing temporary masked grid: " << e.what() << endl; }
#endif

      if (exitCode != 0)
      {
        cerr << "Plot script failed for " << framePath.string() << " with exit code " << exitCode << endl;
        continue;
      }
      plotted++;
    }

    if (plotted == 0)
    {
      cerr << "No elevation frames were plotted successfully." << endl;
      return;
    }

    string fps = options.contains("animate_fps") ? options.get("animate_fps") : "30";
    string speedFactor = options.contains("animate_speed_factor") ? options.get("animate_speed_factor") : "4.0";
    string format = options.contains("animate_format") ? options.get("animate_format") : "mp4";
    string outName = options.contains("animate_out") ? options.get("animate_out") : prefix;

    char digitFmt[16];
    snprintf(digitFmt, sizeof(digitFmt), "%%0%dd", digits);

    fs::path pattern = outputPath / (prefix + string(digitFmt) + ".png");
    fs::path videoPath = outputPath / (outName + "." + format);

    std::ostringstream ffmpegCmd;
    ffmpegCmd << "ffmpeg -y -i \"" << pattern.string() << "\""
              << " -c:v libx264 -crf 23 -r " << fps
              << " -c:a none -pix_fmt yuv420p"
              << " -filter:v \"pad=ceil(iw/2)*2:ceil(ih/2)*2,setpts=" << speedFactor << "*PTS\""
              << " \"" << videoPath.string() << "\"";

    cout << "Encoding " << plotted << " frames to " << videoPath.string() << " ..." << endl;

    int ffmpegExit = executeCommand(ffmpegCmd.str(), true);
    if (ffmpegExit != 0)
    {
      cerr << "ffmpeg failed with exit code " << ffmpegExit << endl;
      return;
    }

    cout << "Animation saved to " << videoPath.string() << endl;
  }

  /**
   * @brief Create a 3D perspective elevation-frame plot script: bathymetry
   * relief base (grdview) plus a masked, vertically exaggerated wave-height
   * surface on top. The wave surface's shape comes from the exaggerated
   * grid, but its color comes from the true (unscaled) values via -G drape.
   * @param bathyPath Bathymetry grid path
   * @param elevPath Elevation snapshot grid path for this frame
   * @param bathyCptPath Bathymetry color palette path
   * @param waveCptPath Wave-height color palette path
   * @param region GMT -R string: "west/east/south/north/zmin/zmax"
   * @param proj GMT projection string: "-JM<width> -JZ<height>"
   * @param persp GMT perspective string: "-p<azimuth>/<elevation>"
   * @param maskThresh Mask |elevation| below this value (meters) to transparent
   * @param waveExag Vertical exaggeration applied to the wave surface only
   * @param title Plot title
   * @param subtitle Plot subtitle (timestamp)
   * @param outputPath Output PNG path, no extension
   * @return fs::path Path to the created script file
   */
  fs::path createElevationSurfaceScript(
      string bathyPath, string elevPath,
      string bathyCptPath, string waveCptPath,
      string region, string proj, string persp,
      float maskThresh, float waveExag,
      string title, string subtitle,
      string outputPath)
  {
    string fileExt = ".bat";
#ifdef __linux__
    fileExt = ".sh";
#endif

    fs::path scriptPath = fs::temp_directory_path() / (fs::path(outputPath).stem().string() + "_surf" + fileExt);
    std::ofstream scriptOfs(scriptPath.string());

#ifdef WIN32
    scriptOfs << "@echo off" << std::endl;
    scriptOfs << "set \"GMT_VERBOSE=quiet\"" << std::endl;
    scriptOfs << "set \"GMT_END_SHOW=off\"" << std::endl;
#elif __linux__
    scriptOfs << "#!/bin/bash" << std::endl;
    scriptOfs << "export GMT_VERBOSE=quiet" << std::endl;
    scriptOfs << "export GMT_END_SHOW=off" << std::endl;
#endif

    fs::path maskedPath = fs::temp_directory_path() / (fs::path(elevPath).stem().string() + "_masked.grd");
    fs::path exagPath = fs::temp_directory_path() / (fs::path(elevPath).stem().string() + "_exag.grd");

    // Mask near-zero elevation to NaN so the bathymetry relief shows through
    // wherever the wave has no real signal yet (same trick as plotDeform).
    scriptOfs << "gmt grdclip \"" << elevPath << "\" -Si-" << maskThresh << "/" << maskThresh << "/NaN -G\"" << maskedPath.string() << "\" -Vq" << std::endl;
    scriptOfs << "gmt grdmath \"" << maskedPath.string() << "\" " << waveExag << " MUL = \"" << exagPath.string() << "\"" << std::endl;

    scriptOfs << "gmt begin \"" << outputPath << "\" png E600" << std::endl;

    scriptOfs << "gmt grdview \"" << bathyPath << "\" -R" << region << " " << proj << " " << persp
              << " -Qs -C\"" << bathyCptPath << "\" -Wfaint,white -X0 -Y0 -Vq" << std::endl;

    scriptOfs << "gmt grdview \"" << exagPath.string() << "\" -G\"" << maskedPath.string() << "\" -R" << region << " " << proj << " " << persp
              << " -Qi100 -C\"" << waveCptPath << "\" -Vq" << std::endl;

    // -Baf is required for the title (+t) to actually render -- GMT silently
    // drops +t (but not +s) when no other frame annotation is requested.
    scriptOfs << "gmt basemap -R" << region << " " << proj << " " << persp << " -Baf";
    if (title.length())
    {
      scriptOfs << " -B+t\"" << title << "\"";
      if (subtitle.length())
      {
        scriptOfs << "+s\"" << subtitle << "\"";
      }
    }
    scriptOfs << " --FONT_TITLE=14p --FONT_SUBTITLE=10p -Vq" << std::endl;

    scriptOfs << "gmt colorbar -DjBR+w6c/0.4c+o1c/1c -C\"" << waveCptPath << "\" -Baf+l\"Altura (m)\" -Vq" << std::endl;

    scriptOfs << "gmt end" << std::endl;

#ifndef _DEBUG
#ifdef WIN32
    scriptOfs << "del \"" << maskedPath.string() << "\"" << std::endl;
    scriptOfs << "del \"" << exagPath.string() << "\"" << std::endl;
#elif __linux__
    scriptOfs << "rm -f \"" << maskedPath.string() << "\" \"" << exagPath.string() << "\"" << std::endl;
#endif
#endif

    scriptOfs.close();

    return scriptPath;
  }

  /**
   * @brief Render a scenario's elevation snapshot sequence as a 3D oblique
   * relief block video (bathymetry base, exaggerated wave surface on top)
   * @param options Geo options
   *
   * Required options: output, source (title), dt, elev_interval, grid
   * (bathymetry). Optional: elev_prefix/elev_digits (as in the 2D version),
   * palette_max_z, bathy_cpt, animate_z_min/animate_z_max (default
   * -10000/6000), animate_jz (default "6c"), animate_az/animate_el (default
   * 210/25), animate_wave_exag (default 300), animate_mask_thresh (default
   * 0.03), animate_format/animate_fps/animate_speed_factor/animate_out.
   */
  void plotElevationAnimation3D(geo::Options &options)
  {
    auto [inputPath, outputPath] = getPaths(options);

    string title = options.get("source");

    float dt = options.contains("dt") ? options.getFloat("dt") : 1.0f;
    int interval = options.contains("elev_interval") ? options.getInt("elev_interval") : 1;
    string prefix = options.contains("elev_prefix") ? options.get("elev_prefix") : "elev";
    int digits = options.contains("elev_digits") ? options.getInt("elev_digits") : 5;

    vector<fs::path> frames = findElevationFrames(outputPath, prefix, digits);
    if (frames.empty())
    {
      cerr << "No " << prefix << " snapshot grids found in " << outputPath.string() << endl;
      return;
    }

    fs::path bathyPath = fs::path(options.get("grid"));
    if (!fs::exists(bathyPath) && fs::exists(inputPath / bathyPath))
    {
      bathyPath = (inputPath / bathyPath).make_preferred();
    }
    if (!fs::exists(bathyPath))
    {
      cerr << "Bathymetry grid " << bathyPath.string() << " does not exist." << endl;
      return;
    }
    bathyPath = fs::canonical(bathyPath);

    Grid bathyGrid;
    if (loadGrid(bathyGrid, bathyPath, options, true) == false)
    {
      cerr << "Unable to load bathymetry grid from " << bathyPath.string() << endl;
      return;
    }

    auto [x0, y0, xMax, yMax] = bathyGrid.extents();

    float zMin = options.contains("animate_z_min") ? options.getFloat("animate_z_min") : -10000.0f;
    float zMax = options.contains("animate_z_max") ? options.getFloat("animate_z_max") : 6000.0f;
    string jz = options.contains("animate_jz") ? options.get("animate_jz") : "6c";
    string az = options.contains("animate_az") ? options.get("animate_az") : "210";
    string el = options.contains("animate_el") ? options.get("animate_el") : "25";
    float waveExag = options.contains("animate_wave_exag") ? options.getFloat("animate_wave_exag") : 300.0f;
    float maskThresh = options.contains("animate_mask_thresh") ? options.getFloat("animate_mask_thresh") : 0.03f;

    string bathyCpt = options.get("bathy_cpt");
    if (bathyCpt.empty())
    {
      bathyCpt = "globe";
    }

    std::ostringstream regionOss;
    regionOss << x0 << "/" << xMax << "/" << y0 << "/" << yMax << "/" << zMin << "/" << zMax;
    string region = regionOss.str();

    string proj = "-JM10c -JZ" + jz;
    string persp = "-p" + az + "/" + el;

    // Build the bathymetry CPT once, shared by all frames.
    fs::path bathyCptPath = fs::temp_directory_path() / "animate_bathy.cpt";
    {
      std::ostringstream cptCmd;
      cptCmd << "gmt makecpt -C" << bathyCpt << " -D > \"" << bathyCptPath.string() << "\"";
      executeCommand(cptCmd.str(), false);
    }

    float paletteMaxZ = 1.0f;
    if (options.contains("palette_max_z"))
    {
      try { paletteMaxZ = std::stof(options.get("palette_max_z")); }
      catch (...) { cerr << "Warning: invalid palette_max_z value, using 1.0" << endl; }
    }
    fs::path wavePalettePath = createMaxPaletteFile(paletteMaxZ);

    cout << "Plotting " << frames.size() << " 3D elevation frames from " << outputPath.string() << " ..." << endl;

    int plotted = 0;
    for (auto &framePath : frames)
    {
      fs::path gridPath = framePath;
      Grid grid;
      if (loadGrid(grid, gridPath, options, true) == false)
      {
        cerr << "Unable to load elevation frame " << framePath.string() << ", skipping." << endl;
        continue;
      }

      int frameIndex = parseFrameIndex(framePath.filename().string(), prefix, digits);
      string timestamp = formatElevationTimestamp(frameIndex, dt, interval);

      fs::path outBase = gridPath;
      outBase.replace_extension("");

      fs::path scriptPath = createElevationSurfaceScript(
          bathyPath.string(), gridPath.string(),
          bathyCptPath.string(), wavePalettePath.string(),
          region, proj, persp,
          maskThresh, waveExag,
          title, timestamp,
          outBase.string());

      int exitCode = executeCommand(scriptPath.string(), true);

#ifndef _DEBUG
      try { fs::remove(scriptPath); }
      catch (fs::filesystem_error &e) { cerr << "Error removing temporary script file: " << e.what() << endl; }
#endif

      if (exitCode != 0)
      {
        cerr << "Plot script failed for " << framePath.string() << " with exit code " << exitCode << endl;
        continue;
      }
      plotted++;
    }

    if (plotted == 0)
    {
      cerr << "No elevation frames were plotted successfully." << endl;
      return;
    }

    string fps = options.contains("animate_fps") ? options.get("animate_fps") : "30";
    string speedFactor = options.contains("animate_speed_factor") ? options.get("animate_speed_factor") : "4.0";
    string format = options.contains("animate_format") ? options.get("animate_format") : "mp4";
    string outName = (options.contains("animate_out") ? options.get("animate_out") : prefix) + "_3d";

    char digitFmt[16];
    snprintf(digitFmt, sizeof(digitFmt), "%%0%dd", digits);

    fs::path pattern = outputPath / (prefix + string(digitFmt) + ".png");
    fs::path videoPath = outputPath / (outName + "." + format);

    std::ostringstream ffmpegCmd;
    ffmpegCmd << "ffmpeg -y -i \"" << pattern.string() << "\""
              << " -c:v libx264 -crf 23 -r " << fps
              << " -c:a none -pix_fmt yuv420p"
              << " -filter:v \"pad=ceil(iw/2)*2:ceil(ih/2)*2,setpts=" << speedFactor << "*PTS\""
              << " \"" << videoPath.string() << "\"";

    cout << "Encoding " << plotted << " frames to " << videoPath.string() << " ..." << endl;

    int ffmpegExit = executeCommand(ffmpegCmd.str(), true);
    if (ffmpegExit != 0)
    {
      cerr << "ffmpeg failed with exit code " << ffmpegExit << endl;
      return;
    }

    cout << "Animation saved to " << videoPath.string() << endl;
  }

}

