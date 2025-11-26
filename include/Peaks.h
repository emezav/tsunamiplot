/**
 * @file
 * Based on https://github.com/Tugbars/peak-finder
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>, <emezav@gmail.com>
 * @brief Helper functions to find peaks in data series.
 */

#pragma once
// Peaks.h
#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * @brief Helper functions to find peaks in data series.
 *  Based on https://github.com/Tugbars/peak-finder
 */
namespace Peaks {
    const float EPS = 2.2204e-16f; /*!< Small epsilon value to replace zeros */
    /**
     * @brief Finds peaks in a data series.
     * @param x0 Input data series
     * @param peakInds Output vector with indices of detected peaks
     */
    void findPeaks(vector<float> x0, vector<int>& peakInds); 
}

#endif
