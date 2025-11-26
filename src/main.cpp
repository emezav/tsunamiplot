/**
 * @file
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Tsunami plot utilities
 * @version 0.1
 * @date 2025-11-15
 * @copyright Copyright (c) 2025
 */

#include <iostream>
#include <string>
#include "geo.h"
#include "tsunamiplot.h"

void usage(char * program) ;


int main(int argc, char * argv[]) {

  using std::cout;
  using std::string;
  using std::endl;

  using namespace TsunamiPlot;


  std::ostringstream cmdLineOss;

  // argv[0] is the program name
  // Arguments from argv[2] to argv[argc-1] are options
  for (int i = 2; i < argc; ++i) {
    cmdLineOss << argv[i] << ";";
  }

  // If argv[1] exists, it is the plot type
  if (argc > 1) {
    cmdLineOss << "type=" << argv[1] << ";";
  }

  std::string cmdLine = cmdLineOss.str();

  // Plot deform examples
  // cmdLine ="type=plot-deform;source=Pacifico;plot_invbat=true;lang=es;deform=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Deform\\result_v1_ver.dat;grid=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Grids\\grid1.txt;dxm=810;dym=810;dim=-81.8101618 -1.0038784 1324 874;format=lrf";
  // cmdLine="type=plot-deform;deform=D:\\Projects\\Tsunami\\Deform\\Gebco_caribbean_15sec\\cdnp_02_caribbean_15sec.bil;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_caribbean_15sec\\Gebco_caribbean_15sec.bil";
  // cmdLine="type=plot-deform;deform=D:\\Projects\\Tsunami\\Deform\\Gebco_pacific_30sec\\deform_kamchatka_np2_allpacific_30sec.bil;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_pacific_30sec\\allpacific_30sec.bil;source=Kamchatka 2025 NP2;";
  // cmdLine="type=plot-deform;deform=D:\\Projects\\Tsunami\\Deform\\Gebco_pacific_5min\\deform_kmck_v0_allpacific_5min.bil;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_pacific_5min\\allpacific_5min.bil;source=Kamchatka 2025";
  // cmdLine ="type=plot-deform;plot_invbat=true;deform=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Deform\\result_v1_ver.bil;grid=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Grids\\grid1.bil";
  // cmdLine="type=plot-deform;plot_invbat=true;deform=D:\\Caro\\Doctorado\\Simulaciones_Tunami\\Simulaciones_Tumaco\\mareal_100_Worse_Slip_Deficit\\Deform\\def_v1_ver.dat;grid=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\grid1.txt;dxm=819;dym=810;dim=-81.81016541 -1.003877997 700 667;format=lrf";
  // cmdLine="type=plot-deform;deform=C:\\Users\\er_mv\\Desktop\\ATLANTICO\\Deform\\Central_MARidge_Atlantico_1min.bil;grid=C:\\Users\\er_mv\\Desktop\\ATLANTICO\\Grids\\Atlantico.bil";
  // cmdLine="type=plot-deform;deform=D:\\Projects\\Tsunami\\Batch\\CasoCDNP_02\\Deform_CasoCDNP_02.bil;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_caribbean_30sec\\Gebco_caribbean_30sec.bil;source=CDNP 02";

  // Plot zmax examples

  // cmdLine="lang=en;type=plot-max;event_type=near;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_caribbean_30sec\\Gebco_caribbean_30sec.bil;zmax=D:\\Projects\\Tsunami\\Batch\\CasoCDNP_02\\zmax.bil;wave_data=D:\\Projects\\Tsunami\\Batch\\CasoCDNP_02\\point.dat;gauge=D:\\Projects\\Tsunami\\Grids\\Gebco_caribbean_30sec\\outpoint_caribbean_30sec.txt";
  // cmdLine="lang=en;type=plot-max;event_type=far;grid=D:\\Projects\\Tsunami\\Grids\\Gebco_pacific_1min\\allpacific_1min.bil;zmax=D:\\Projects\\Tsunami\\Result\\case_1906_slip_deficit_1min\\zmax.bil;wave_data=D:\\Projects\\Tsunami\\Result\\case_1906_slip_deficit_1min\\point.dat;gauge=D:\\Projects\\Tsunami\\Grids\\Gebco_pacific_1min\\outpoint.txt";

  // cmdLine="type=plot-max;event_type=near;plot_invbat=true;format=lrf;zmax_nodata=0;dim=-81.81016541 -1.003877997 700 667;dxm=810;dym=810;zmax=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\RESULT\\max1.txt;grid=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\grid1.txt;wave_data=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Result\\point1.txt;gauge=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\outpoint1.txt";
  // cmdLine="type=plot-max;event_type=near;plot_invbat=true;zmax_nodata=0;zmax=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\RESULT\\max1.bil;grid=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\grid1.bil;wave_data=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Result\\point1.txt;gauge=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\outpoint1.txt;outline=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\outline.bf2";

  // Plot wave examples
  // cmdLine="lang=en;type=plot-wave;event_type=far;dim=-81.81016541 -1.003877997 700 667;dxm=810;dym=810;zmax=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\RESULT\\max1.txt;wave_data=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Result\\point1.txt;gauge=C:\\Users\\er_mv\\Desktop\\mareal_100_Worse_Slip_Deficit\\Grids\\outpoint1.txt;source=Max1";
  // cmdLine="type=plot-wave;wave_data=D:\\Projects\\Tsunami\\Result\\case_1906_slip_deficit_1min\\point.dat;gauge=D:\\Projects\\Tsunami\\Grids\\Gebco_pacific_1min\\outpoint.txt;zmax=D:\\Projects\\Tsunami\\Result\\case_1906_slip_deficit_1min\\zmax.bil;lang=en;source=1906 Ecuador";
  // cmdLine="type=plot-wave;wave_data=C:\\Users\\er_mv\\Desktop\\ATLANTICO\\Result\\Central_MARidge_Atlantico_1min\\point_1.dat;gauge=C:\\Users\\er_mv\\Desktop\\ATLANTICO\\Grids\\outpoint.txt;zmax=C:\\Users\\er_mv\\Desktop\\ATLANTICO\\Result\\Central_MARidge_Atlantico_1min\\zmax.bil;lang=es;time_res=h;source=1906 Ecuador";
  // cmdLine="type=plot-wave;wave_data=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Result\\point1.txt;gauge=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\Grids\\outpoint1.txt;zmax=D:\\Erwin\\Proyectos\\Tsunami\\2024\\D5\\result\\max1.txt;format=lrf;lang=es;time_res=h;source=2024 D5;invbat=true;dim=-81.8101618 -1.0038784 1324 874;dxm=810;dym=810";

  geo::Options options(cmdLine, '=', "\r\n;");

#ifdef _DEBUG
  cout << "Tsunami Plot Utilities" << endl;
  cout << "=====================" << endl;
  cout << "Parsed Options:" << endl;
  for (const auto &key : options.getKeys()) {
    cout << "  " << key << " = " << options.get(key) << endl;
  }
#endif

  string plotType = options.get("type");

  if (plotType == "plot-deform") {
    plotDeform(options);
  } else if (plotType == "plot-max") {
    plotZMax(options);
  } else if (plotType == "plot-wave") {
    plotWaveProfile(options);
  } else {
    usage(argv[0]);
    return 1;
  }

  return 0;
}

void usage(char * program) {
  using std::cout;
  using std::endl;

  cout << "Usage: " << program << " type [options]" << endl;
  cout << "General options:" << endl;
  cout << "  type                     Type of plot to create. See below." << endl;
  cout << "  input=<path>             Path to input data directory (default: current directory)" << endl;
  cout << "  output=<path>            Path to output data directory (default: current directory)" << endl;
  cout << "  lang=<es|en>             Language for plot titles and labels (default: es)" << endl;
  cout << "  source=<text>            Source description to include in the plot title" << endl;
  cout << "  time_res=m|h             Set time resolution for wave data to minutes or hours" << endl;
  cout << "  event_type=near|far      Set plot mode to nearshore or farshore (default: near)" << endl;
  cout << endl;
  cout << "Options for bathymetry:" << endl;
  cout << "  plot_invbat=<true|false> Invert bathymetry for plotting (default: false)" << endl;
  cout << endl;
  cout << "Options for zmax grid:" << endl;
  cout << "  zmax_nodata=<value>       Converts the specified value to Nodata (100) in the zmax grid" << endl;

  cout << endl;
  cout << "Options depending on the plot type:" << endl;
  cout << endl;
  cout << "  plot-deform  <options> Plot deformation grid" << endl;
  cout << "                         deform=<path to deform grid>" << endl;
  cout << "                         grid=<path to bathymetry>" << endl;
  cout << endl;
  cout << "  plot-max <options>     Plot maximum wave height grid" << endl;
  cout << "                         zmax=<path to zmax grid>" << endl;
  cout << "                         grid=<path to bathymetry>" << endl;
  cout << "                         wave_data=<path to wave data>" << endl;
  cout << "                         gauge=<path to gauge file>" << endl;
  cout << endl;
  cout << "  plot-wave <options>    Plot wave profile at gauges" << endl;
  cout << "                         zmax=<path to zmax grid>" << endl;
  cout << "                         wave_data=<path to wave data>" << endl;
  cout << "                         gauge=<path to gauge file>" << endl;
  cout << endl;
  cout << "Note on text grids: " << endl;
  cout << "  For text grids, the following options must be used to specify the format:" << endl;
  cout << "    format=<lrf|frf>  Format of the text grid (default: lrf)" << endl;
  cout << "       lrf: Last row of the grid is the first record on the file" << endl;
  cout << "       frf: First row of the grid is the first record on the file" << endl;
  cout << "    dim=<xllcorner yllcorner ncols nrows>  Grid dimensions for text grids" << endl;
  cout << "Grid resolution can be specified using one of the following:" << endl;
  cout << "  ds=<cellsize>          Cell size for text grids (minutes) for square cells (dx = dy)" << endl;
  cout << "  dx=<cellsize_x>        Cell size in x direction for text grids (decimal degrees)" << endl;
  cout << "  dy=<cellsize_y>        Cell size in y direction for text grids (decimal degrees)" << endl;
  cout << "  dxm=<cellsize_x>       Cell size in x direction for text grids (meters)" << endl;
  cout << "  dym=<cellsize_y>       Cell size in y direction for text grids (meters)" << endl;
  cout << endl;
}