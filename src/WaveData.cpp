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

#ifdef _DEBUG
		// Print all peaks
		// index = 0;
		// cout << "Peaks for series " << i << ":" << endl;
		// do {
		// 	std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
		// 	cout << t_p << " " << h_p << endl;
		// } while (++index < point_inds.size());
#endif

    //Get max height wave point
		//Find the peak with max height
		double t_max_peak = 0.0f;
		double h_max_peak = 0.0f;
		{

			for (index = 0; index < point_inds.size(); index++) {
				std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
				if (h_p > h_max_peak && h_p > h_first_wave && t_p >= delta_first_wave) {
					t_max_peak = t_p;
					h_max_peak = h_p;
				}
			}
			maxHeightData.push_back(WavePoint(t_max_peak, h_max_peak));
		}


    //Select first wave peak based on h_first_wave and delta_first_wave
		double t_peak = 0.0f;
		double h_peak = 0.0f;

		// Take the max height peak if it meets the first wave criteria
		if (h_max_peak > h_first_wave && t_max_peak >= delta_first_wave) {
			t_peak = t_max_peak;
			h_peak = h_max_peak;
		}

		{
			for (index = 0; index < point_inds.size(); index++) {
				std::tie(t_p, h_p) = point_data.at(point_inds[index], i);
				if (h_p > h_first_wave && t_p >= delta_first_wave) {
					t_peak = t_p;
					h_peak = h_p;
					break; // Found the first peak that meets the criteria
				}
			}
			firstWaveData.push_back(WavePoint(t_peak, h_peak));
		}
	}
}

