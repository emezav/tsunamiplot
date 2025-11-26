
/**
 * @file
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>, <emezav@gmail.com>
 * @copyright GNU Public License.
 *
 * @brief Helper class to manage timeseries data stored in text files.
 */

#ifndef TIMESERIES_H
#define TIMESERIES_H
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <algorithm>
#include "geo.h"
using std::vector;
using std::cerr;
using std::string;

/**
* @brief Encapsula los los datos de marcas de tiempo y valores para N series almacenados como una matriz en un archivo de texto.
* La primera columna es la marca de tiempo.
* Las demas columnas contienen los valores para las marcas de tiempo correspondientes.
*/
class Timeseries
{
public:

	/**
	 * @brief Construct a new Timeseries instance
	 */
	Timeseries() = default;

	/**
	 * @brief Construct a new Timeseries instance loading data from a file
	 * @param path Path to the timeseries data file
	 */
	Timeseries(string path) {
    //Load data from file
   string content = geo::Strings::loadFile(path);
   vector<string> lines = geo::Strings::splitLines(content);
    bool valid = true;
    if (lines.size() < 2) {
      valid = false;
    }

    if (!valid) {
      return;
    }

    // File does not have a header, so all lines are data

    //Parse first line as header to get number of columns
    vector<string> header = geo::Strings::split(geo::Strings::trim(lines[0]));
    int ncols = header.size();

    //Initialize data vectors. Data is an array of series, each series is an array of floats
    // Number of series is ncols - 1 (first column is time)
    // Each series has lines.size() elements
    data.resize(ncols - 1);
    for (int i = 0; i < ncols - 1; i++) {
      data[i].resize(lines.size());
    }
    timesteps.resize(lines.size());

    //Parse data lines
    // For each line: first column goes to timesteps, remaining columns go to data
    for (int i = 0; i < lines.size(); i++) {
      vector<string> values = geo::Strings::split(geo::Strings::trim(lines[i]));
      if (values.size() != ncols) {
        //Invalid line
        valid = false;
        break;
      }

      //First column is time
      timesteps[i] = std::stof(values[0]);

      //Remaining columns are data
      for (int j = 1; j < ncols; j++) {
        data[j-1][i] = std::stof(values[j]);
      }
    }
	}

	/**
	 * @brief Creates a subseries from the timeseries
	 * @param startidx Starting index of the subseries
	 * @param count Number of series to include in the subseries
	 * @return Timeseries Subseries object
	 */
	Timeseries subseries(int startidx, int count) {
		vector<vector<float>>dat;


		for (int i = startidx; i < data.size() && count > 0; i++) {
			dat.push_back(data[i]);
		}

		return Timeseries(timesteps, dat);

	}

	/**
	 * @brief Apply a factor to all time steps
	 * @param factor Factor to apply
	 * @return Timeseries, copy of all the data andd the new time steps
	 */
	Timeseries applyTimeFactor(float factor) {
		for (int i = 0; i < timesteps.size(); i++) {
			timesteps[i] *= factor;
		}
		vector<vector<float>>dat;

		for (int i = 0; i < data.size(); i++) {
			dat.push_back(data[i]);
		}
		return Timeseries(timesteps, dat);
	}

	/**
	 * @brief Checks if the timeseries is empty
	 * @return true if empty
	 * @return false if not empty
	 */
	bool empty() {
		return (timesteps.size() == 0 || data.size() == 0);
	}

	/**
	 * @brief Gets the number of time steps in the timeseries
	 * @return const int Number of time steps
	 */
	const int step_count() const  {
		return timesteps.size();
	}

	/**
	 * @brief Gets the number of series in the timeseries
	 * @return const int Number of series
	 */
	const int series_count() const {
		return data.size();
	}

	/**
	 * @brief Gets the time steps vector
	 * @return const vector<float>& Reference to the time steps vector
	 */
	const vector<float> & steps() const {
		return timesteps;
	}

	/**
	 * @brief Gets the series at the specified index
	 * @param idx Index of the series
	 * @return const vector<float>& Reference to the series vector
	 */
	const vector<float> & series(int idx) const {
		return data[idx];
	}

	/**
	 * @brief Gets the minimum time value in the timeseries
	 * @return float Minimum time value
	 */
	float min_t() {
		float t = 0.0;
		if (timesteps.size() > 0) {
			t = timesteps[0];
		}
		return t;
	}

	/**
	 * @brief Gets the maximum time value in the timeseries
	 * @return float Maximum time value
	 */
	float max_t() {
		float t = 0.0;
		if (timesteps.size() > 0) {
			t = timesteps[timesteps.size() - 1];
		}
		return t;
	}

	/**
	 * @brief Gets the value at the specified time step and series index
	 * @param step Time step index
	 * @param idx Series index
	 * @return std::pair<float, float> Pair of time and value
	 */
	std::pair<float, float> at(int step, int idx) {

		float t = -1.0;
		float z;

		if (step >= timesteps.size() || idx >= data.size()) {
			return { t, z };
		}

		return { timesteps[step], data[idx][step] };

	}

	/**
	 * @brief Gets the minimum value in the timeseries
	 * @return std::tuple<int, int, float, float> Tuple of series index, time step index, time and value
	 */
	std::tuple<int, int, float, float> min_val() {
		int step = -1;
		int idx = -1;
		float t = -1.0;
		float z;


		if (empty()) {
			return { idx, step, t, z };
		}

		bool min_found = false;
		int min_idx = -1;
		int min_step = -1;
		float min_val = 0.0;
		float val = 0.0;

		for (int idx = 0; idx < data.size(); idx++) {
			auto it = std::min_element(data[idx].begin(), data[idx].end());
			int series_step = std::distance(data[idx].begin(), it);

			val = *it;

			if (!min_found || val < min_val) {
				min_val = val;
				min_step = series_step;
				min_idx = idx;
				min_found = true;
			}
		}

		return { min_idx, min_step, timesteps[min_step], min_val };
	}

	/**
	 * @brief Gets the maximum value in the timeseries
	 * @return std::tuple<int, int, float, float> Tuple of series index, time step index, time and value
	 */
	std::tuple<int, int, float, float> max_val() {
		int step = -1;
		int idx = -1;
		float t = -1.0;
		float z;


		if (empty()) {
			return { idx, step, t, z };
		}

		bool max_found = false;
		int max_idx = -1;
		int max_step = -1;
		float max_val = 0.0;
		float val = 0.0;

		for (int idx = 0; idx < data.size(); idx++) {
			auto it = std::max_element(data[idx].begin(), data[idx].end());
			int series_step = std::distance(data[idx].begin(), it);

			val = *it;

			if (!max_found || val > max_val) {
				max_val = val;
				max_step = series_step;
				max_idx = idx;
				max_found = true;
			}
		}

		return { max_idx, max_step, timesteps[max_step], max_val };
	}

	bool save(string path) {
		std::ofstream ofs(path);
		if (!ofs.is_open()) {
			return false;
		}

		//Write data
		// First column is time steps
		// Remaining columns are data series

		for (int i = 0; i < timesteps.size(); i++) {
			ofs << timesteps[i];
			for (int j = 0; j < data.size(); j++) {
				ofs << " " << data[j][i];
			}
			ofs << std::endl;
		}

		ofs.close();
		return true;
	}


private:
	/**
	 * @brief Construct a new Timeseries instance
	 * @param tsteps Time steps vector
	 * @param dat Data matrix
	 */
	Timeseries(vector<float>tsteps, vector<vector<float>>dat) : timesteps(tsteps), data(dat) {

	}

	vector<float> timesteps; /*!< Time steps vector */
	vector<vector<float>> data; /*!< Data matrix: array of series, each series is an array of floats */
};

#endif // TIMESERIES_H