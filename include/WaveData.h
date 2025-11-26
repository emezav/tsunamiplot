/**
 * @file
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>, <emezav@gmail.com>
 * @copyright GNU Public License.
 *
 * @brief Helper classes to manage wave data from timeseries.
 */

#ifndef WAVE_DATA
#define WAVE_DATA
#pragma once

#include <string>
#include <vector>
#include "Timeseries.h"

/**
 * @brief Represents a point in a wave with a specific time and height.
 */
struct WavePoint {
	double time; /*!< Time of the wave point */
	double height; /*!< Height of the wave point */

	/**
	 * @brief Construct a new Wave Point instance
	 */
	WavePoint() : WavePoint(0.0f, 0.0f){
	}

	/**
	 * @brief Construct a new Wave Point instance
	 * @param p_time Time of the wave point
	 * @param p_height Height of the wave point
	 */
	WavePoint(double p_time, double p_height) : time(p_time), height(p_height) {

	}
};

/**
 * @brief Represents a point in a wave with a specific time and height.
 */
class WaveData
{
public:

	/**
	 * @brief Construct a new Wave Data instance
	 * @param series Timeseries data
	 */
	WaveData(Timeseries series) : WaveData(series, 0.0, 0.0) {
	}

	/**
	 * @brief Construct a new Wave Data instance
	 * @param series Timeseries data
	 * @param h_first_wave Minimum height to consider a wave as the first wave
	 * @param delta_first_wave Minimum difference in height to consider a wave as the first wave
	 */
	WaveData(Timeseries series, double h_first_wave, double delta_first_wave);

	/**
	 * @brief Get the first wave data points
	 * @return vector<WavePoint>
	 */
	vector<WavePoint> firstWave() {
		return firstWaveData;
	}

	/**
	 * @brief Get the maximum height wave data points
	 * @return vector<WavePoint>
	 */
	vector<WavePoint> maxHeight() {
		return maxHeightData;
	}

private:
	vector<WavePoint> firstWaveData; /*!< First wave data points */
	vector<WavePoint> maxHeightData; /*!< Maximum height wave data points */
};

#endif
