/**
 * @file
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>, <emezav@gmail.com>
 * @copyright GNU Public License.
 *
 * @brief Helper classes to manage wave data from timeseries.
 */
#include "WaveData.h"
#include "Peaks.h"

WaveData::WaveData(Timeseries point_data, double h_first_wave, double delta_first_wave) {
	//Get first wave for each point series
	for (int i = 0; i < point_data.series_count(); i++) {
		vector<float> point_series = point_data.series(i);
		vector<int> point_inds;
		int nrecords = point_series.size();
		int index; //Timestamp and height index
		double t_p, h_p; //Temporary timestamp and height

		//Find all wave peaks
		Peaks::findPeaks(point_series, point_inds);

		if (point_inds.size() <= 0) {
			//No peaks, add an empty wave point to first wave and max height
			firstWaveData.push_back(WavePoint());
			maxHeightData.push_back(WavePoint());
			continue;
		}

    //Get max height wave point
		//Find the peak with max height
		{
			double t_max_peak = 0.0f;
			double h_max_peak = 0.0f;
			for (index = 0; index < point_inds.size(); index++) {
				std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
				if (h_p > h_max_peak) {
					t_max_peak = t_p;
					h_max_peak = h_p;
				}
			}
			maxHeightData.push_back(WavePoint(t_max_peak, h_max_peak));
		}

    //Select first wave peak based on h_first_wave and delta_first_wave
		double t_peak, h_peak; //Time and height of the selected peak

		t_peak = 0.0f;
		h_peak = 0.0f;

		/*
		// Print all peaks
		index = 0;
		do {
			std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
			cout << t_p << " " << h_p << endl;
		} while (++index < point_inds.size());
		*/

		//Get the first peak
		std::tie(t_p, h_p) = point_data.at(point_inds[0], i);

		//If the first peak is greater than h_first_wave AND time >= delta_first_wave, this is the first wave time.
		//This is a NEAR event.
		if (t_p >= delta_first_wave && h_p >= h_first_wave) {
			t_peak = t_p;
			h_peak = h_p;
		}
		else {
			//Otherwise walk the entire wave looking for the first peak after delta_first_wave greater than h_first_wave
			//Find the timestamp when the wave height is greater than h_first_wave
			int j;
			double t_f, h_f; //t_f = time when the wave reaches h_first_wave for the first time

			t_f = 0.0f;
			h_f = 0.0f;
			j = 0;
			do {
				std::tie(t_f, h_f) = point_data.at(j, i);
			} while (++j < nrecords && h_f < h_first_wave);

			//If the wave height never reaches h_first_wave, just take the maximum peak AFTER delta_first_time
			if (j == nrecords) {
				double t_max_peak = 0.0f;
				double h_max_peak = 0.0f;
				for (index = 0; index < point_inds.size(); index++) {
					std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
					if (t_p > delta_first_wave && h_p > h_max_peak) {
						t_max_peak = t_p;
						h_max_peak = h_p;
					}
				}
				if (h_max_peak > 0.0f) {
					t_peak = t_max_peak;
					h_peak = h_max_peak;
				}
			}
			else {

				bool peak_found = false;
				//The wave reaches h_first_wave at t_f, find the first peak greater than h_first_wave AFTER t_f + delta_first_wave
				for (index = 0; index < point_inds.size() && !peak_found; index++) {
					std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
					if (t_p > t_f + delta_first_wave && h_p > h_first_wave) {
						t_peak = t_p;
						h_peak = h_p;
						peak_found = true;
					}
				}
        //If no such peak is found, just take the maximum peak AFTER t_f + delta_first_wave
				if (!peak_found) {
					//If no peak greater than h_first_wave is found AFTER t_f + delta_first_wave, just take the maximum AFTER t_f + delta_first_time
					double t_max_peak = 0.0f;
					double h_max_peak = 0.0f;
					for (index = 0; index < point_inds.size(); index++) {
						std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
						if (t_p > t_f + delta_first_wave && h_p > h_max_peak) {
							t_max_peak = t_p;
							h_max_peak = h_p;
						}
					}
					if (h_max_peak > 0.0f) {
						t_peak = t_max_peak;
						h_peak = h_max_peak;
					}
				}
			}
		}

		firstWaveData.push_back(WavePoint(t_peak, h_peak));
	}
}

