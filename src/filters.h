#ifndef FILTERS_H
#define FILTERS_H

#include <cmath>

/**
 * @brief Statistic block for min/max/avg
 */
class MinMaxAvgStatistic {
    float min_;
    float max_;
    float sum_;
    int count_;

public:
    MinMaxAvgStatistic() : min_(NAN), max_(NAN), sum_(0), count_(0) {}

    void process(float value) {  
        min_ = isnan(min_) ? value : fmin(min_, value);
        max_ = isnan(max_) ? value : fmax(max_, value);
        sum_ += value;
        count_++;
    }

    void reset() { min_ = NAN; max_ = NAN; sum_ = 0; count_ = 0; }

    float minimum() const { return min_; }
    float maximum() const { return max_; }
    float average() const { return count_ > 0 ? sum_ / count_ : NAN; }
};

/**
 * @brief High-Pass Filter 
 */
class HighPassFilter {
    const float kX, kA0, kA1, kB1;
    float last_filter_value_, last_raw_value_;

public:
    HighPassFilter(float cutoff, float sampling_frequency)
        : kX(exp(-1 / (sampling_frequency / (cutoff * 2 * M_PI)))),
          kA0((1 + kX) / 2), kA1(-kA0), kB1(kX),
          last_filter_value_(0), last_raw_value_(0) {}

    float process(float value) { 
        last_filter_value_ = kA0 * value + kA1 * last_raw_value_ + kB1 * last_filter_value_;
        last_raw_value_ = value;
        return last_filter_value_;
    }

    void reset() { last_raw_value_ = 0; last_filter_value_ = 0; }
};

/**
 * @brief Low-Pass Filter 
 */
class LowPassFilter {
    const float kX, kA0, kB1;
    float last_value_;

public:
    LowPassFilter(float cutoff, float sampling_frequency)
        : kX(exp(-1 / (sampling_frequency / (cutoff * 2 * M_PI)))),
          kA0(1 - kX), kB1(kX), last_value_(0) {}

    float process(float value) {  
        last_value_ = kA0 * value + kB1 * last_value_;
        return last_value_;
    }

    void reset() { last_value_ = 0; }
};

/**
 * @brief Differentiator (Computes the derivative)
 */
class Differentiator {
    const float kSamplingFrequency;
    float last_value_;

public:
    Differentiator(float sampling_frequency) : kSamplingFrequency(sampling_frequency), last_value_(0) {}

    float process(float value) {  
        float diff = (value - last_value_) * kSamplingFrequency;
        last_value_ = value;
        return diff;
    }

    void reset() { last_value_ = 0; }
};

/**
 * @brief Moving Average Filter
 */
template<int kBufferSize>
class MovingAverageFilter {
    int index_;
    float values_[kBufferSize];
    float runningSum_;

public:
    MovingAverageFilter() : index_(0), runningSum_(0.0f) {
        for (int i = 0; i < kBufferSize; i++) values_[i] = 0.0f;
    }

    float process(float value) {
        // Subtract the old value from the running sum
        runningSum_ -= values_[index_];

        // Add the new value
        values_[index_] = value;
        runningSum_ += value;

        // Advance the index (circular buffer)
        index_ = (index_ + 1) % kBufferSize;

        // Return the average
        return runningSum_ / kBufferSize;
    }

    void reset() {
        index_ = 0;
        runningSum_ = 0.0f;
        for (int i = 0; i < kBufferSize; i++) values_[i] = 0.0f;
    }
};


/**
 * @brief Butterworth Bandpass Filter (0.5 Hz – 8 Hz)
 */
class ButterworthFilter {
    static constexpr int order = 2;  // 2nd-order filter
    float b[order + 1] = { 0.2929, 0.0, -0.2929 };  // Filter Coefficients
    float a[order + 1] = { 1.0, -0.5858, 0.1716 };  
    float x[order + 1] = {0};  // Input buffer
    float y[order + 1] = {0};  // Output buffer
    float dcOffset = NAN;  // Use NaN to detect the first input

public:
    float process(float input) {
        // Initialize dcOffset with the first input value
        if (isnan(dcOffset)) {
            dcOffset = input;
        }

        // **Slow DC Correction Instead of Full Removal**
        dcOffset = 0.99 * dcOffset + 0.001 * input;  // Adjust slowly
        float inputCorrected = input - dcOffset;

        // Shift input & output buffers
        for (int i = order; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }

        // Apply Butterworth filter equation
        x[0] = inputCorrected;
        y[0] = b[0] * x[0];
        for (int i = 1; i <= order; i++) {
            y[0] += b[i] * x[i] - a[i] * y[i];
        }

        // Return filtered output
        return y[0];
    }

    float getDCOffset() {
        return dcOffset;
    }
};

#endif // FILTERS_H
