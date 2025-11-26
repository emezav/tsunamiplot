#include "grid.h"
#include "geo.h"

using geo::Grid;
using geo::GridFormat;
using geo::Strings;
using geo::geoStatus;

/**
   * @brief Loads a grid to memory.
   *
   * @param g Output grid object
   * @param path Path to the grid
   * @param options Options
   * @param convertToEsri If true, convert grid to Esri format
   * @return status SUCCESS if grid was loaded, FAILURE otherwise.
   */
  bool loadGrid(Grid &g, fs::path &gridPath, geo::Options &options, bool convertToEsri)
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

    // If gridPath does not exist, check inputPath / gridPath and outputPath / gridPath
    if (fs::exists(gridPath))
    {
      gridPath = fs::canonical(gridPath);
    }
    else if (fs::exists(inputPath / gridPath))
    {
      // Check inputPath / gridPath
      gridPath = fs::canonical(inputPath / gridPath);
    }
    else if (fs::exists(outputPath / gridPath))
    {
      // Check outputPath / gridPath
      gridPath = fs::canonical(outputPath / gridPath);
    }

    // Check if grid path exists
    if (!fs::exists(gridPath) || !fs::is_regular_file(gridPath))
    {
      cerr << "Unable to load grid file " << gridPath.string() << endl;
      return false;
    }

    // Get grid file extension
    string fileExtension = (gridPath.extension().string());
    fileExtension = Strings::tolower(fileExtension);

    // Get grid format from extension
    GridFormat gridFormat = geo::gridFormatFromExtension(fileExtension);
    if (gridFormat == GridFormat::UNKNOWN)
    {
      cerr << "Unsupported deform grid format: " << fileExtension << endl;
      return false;
    }

    // Switch to GridFormat::TEXT_REVERSE if required
    if (gridFormat == GridFormat::TEXT && Strings::tolower(options.get("format")) == "lrf")
    {
      gridFormat = GridFormat::TEXT_REVERSE;
    }

    double xMin, yMin, xMax, yMax;
    int rows, columns;
    double dxDeg, dyDeg;
    double dxM, dyM;

    // If grid format is not TEXT or TEXT_REVERSE, load grid normally
    if (gridFormat != GridFormat::TEXT && gridFormat != GridFormat::TEXT_REVERSE)
    {
      // Ignore dim arguments
      // Get information from the grid and set parameters
      geoStatus loadStatus = geo::LoadGrid(g, gridPath.string());

      if (loadStatus != geoStatus::SUCCESS)
      {
        cerr << "Unable to load bathymetry from " << gridPath.string() << endl;
        return false;
      }

      auto res = g.resolutionDegrees();

      // Set dxDeg, dyDeg
      dxDeg = std::get<0>(res);
      dyDeg = std::get<1>(res);

      auto extents = g.extents();

      // Set boundaries
      xMin = std::get<0>(extents);
      yMin = std::get<1>(extents);
      xMax = std::get<2>(extents);
      yMax = std::get<3>(extents);

      // Get cell size in meters.
      auto sizeMeters = geo::cellSizeMeters(yMin, dxDeg, dyDeg);

      // Set dxM, dyM
      dxM = std::get<0>(sizeMeters);
      dyM = std::get<1>(sizeMeters);

      // Set rows and columns
      auto dims = g.dimensions();
      rows = std::get<0>(dims);
      columns = std::get<1>(dims);
    }
    else
    {
      // grid format is text, origin, dimensions and resolution must have been passed as arguments.
      string gridInfo = options.get("dim");
      gridInfo = Strings::tolower(gridInfo);
      if (gridInfo.compare("auto") == 0)
      {
        cerr << "Grid origin, dimensions and resolution must be specified: x0 y0 rows columns" << endl;
        return false;
      }
      try
      {
        auto dim = options.getFloats("dim", ' ');

        if (dim.size() < 4)
        {
          cerr << "Dims must be defined for text grids" << endl;
          return false;
        }

        xMin = dim[0];
        yMin = dim[1];
        rows = static_cast<int>(dim[2]);
        columns = static_cast<int>(dim[3]);

        // Get grid resolution.
        // It can be in degrees (dx, dy), in meters (dxm, dym) or in minutes (ds)
        bool hasDx = options.contains("dx");
        bool hasDy = options.contains("dy");
        bool hasDs = options.contains("ds");
        bool hasDxM = options.contains("dxm");
        bool hasDyM = options.contains("dym");

        if ((hasDx && !hasDy) || (!hasDx && hasDy))
        {
          cerr << "Both dx and dy must be specified." << endl;
          return false;
        }

        if ((hasDxM && !hasDyM) || (!hasDxM && hasDyM))
        {
          cerr << "Both dxm and dym must be specified." << endl;
          return false;
        }

        if (hasDs)
        {
          // Get resolution in minutes
          float ds = options.getFloat("ds");

          // Set the same resolution on x and y
          dxDeg = ds / 60.0f;
          dyDeg = ds / 60.0f;

          auto sizeMeters = geo::cellSizeMeters(yMin, dxDeg, dyDeg);

          dxM = std::get<0>(sizeMeters);
          dyM = std::get<1>(sizeMeters);
        }
        else if (hasDx && hasDy)
        {
          // Get resolution in degrees
          dxDeg = options.getFloat("dx");
          dyDeg = options.getFloat("dy");

          auto sizeMeters = geo::cellSizeMeters(yMin, dxDeg, dyDeg);

          dxM = std::get<0>(sizeMeters);
          dyM = std::get<1>(sizeMeters);
        }
        else if (hasDxM && hasDyM)
        {
          // Get resolution in meters
          dxM = options.getFloat("dxm");
          dyM = options.getFloat("dym");

          auto degResolution = geo::cellSizeDegrees(yMin, dxM, dyM);

          dxDeg = std::get<0>(degResolution);
          dyDeg = std::get<1>(degResolution);
        }
        else
        {
          cerr << "Grid resolution must be specified using either ds (minutes), dx and dy (degrees), or dxm and dym (meters)." << endl;
          return false;
        }

        // Attempt to load the grid
        bool reverseRows = false;
        if (gridFormat == GridFormat::TEXT_REVERSE)
        {
          reverseRows = true;
        }

        geoStatus loadStatus = g.loadText(gridPath.string(), rows, columns, xMin, yMin, dxM, dyM, NAN, reverseRows);
        if (loadStatus != geoStatus::SUCCESS)
        {
          cerr << "Unable to load text grid with the provided parameters." << endl;
          return false;
        }

        auto extents = g.extents();

        // Set extents
        xMin = std::get<0>(extents);
        yMin = std::get<1>(extents);
        xMax = std::get<2>(extents);
        yMax = std::get<3>(extents);
      }
      catch (std::exception &ex)
      {
        cerr << "Error getting info for text grid : " << ex.what() << endl;
        return false;
      }
    }

    // If deform grid is not in Esri format (.bil), create a .bil file
    if (fileExtension.compare(".bil") != 0 && convertToEsri)
    {
      // Generate a path for the .bil file
      fs::path esriGridPath = gridPath.replace_extension(".bil");

      auto [rows, columns] = g.dimensions();

      if (geo::SaveGrid(g, esriGridPath.string(), GridFormat::ESRI_FLOAT) != geoStatus::SUCCESS)
      {
        cerr << "Unable to save temporary deform grid to " << esriGridPath << endl;
        return false;
      }

      // Load the ESRI grid instead of the  grid
      if (geo::LoadGrid(g, esriGridPath.string()) != geoStatus::SUCCESS)
      {
        cerr << "Unable to load temporary ESRI grid from " << esriGridPath << endl;
        return false;
      }
      // Update gridPath to the new esriGridPath
      gridPath = esriGridPath;
    }
    return true;
  }

