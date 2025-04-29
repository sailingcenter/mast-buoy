
#ifndef BUOY_WAVES_H
#define BUOY_WAVES_H

// TODO: https://www.pschatzmann.ch/home/2021/04/30/back-to-basics-sampling-audio-with-the-esp32/
// TODO: https://github.com/pschatzmann/arduino-audio-tools/tree/main/src/AudioTools
// TODO: https://github.com/pschatzmann/arduino-audio-tools/blob/main/src/AudioAnalog/AnalogConfigESP32V1.h

// TODO: https://www.weather.gov/dlh/WaveHeightExplanation

// TODO: https://medium.com/swlh/how-to-perform-fft-onboard-esp32-and-get-both-frequency-and-amplitude-45ec5712d7da

//import numpy as np
//from scipy.fft import fft
//
//# Define water pressure data (replace with your actual data)
//        pressure_data = np.random.rand(1024)  # Simulate 1024 pressure readings
//
//# Sampling frequency (Hz) - Adjust based on your data collection rate
//fs = 10
//
//# Calculate Fast Fourier Transform (FFT)
//fft_data = fft(pressure_data)
//
//# Calculate power spectrum (absolute value squared of FFT)
//power_spectrum = np.abs(fft_data) ** 2
//
//# Calculate frequency bins based on sampling frequency
//frequency_bins = np.linspace(0, fs / 2, len(power_spectrum) // 2 + 1)
//
//# Find the peak frequency (frequency with the highest power)
//peak_frequency_index = np.argmax(power_spectrum[:len(power_spectrum) // 2])
//peak_frequency = frequency_bins[peak_frequency_index]
//
//# Calculate significant wave height (Hm0) using moments of the spectrum
//moment_0 = np.trapz(power_spectrum, frequency_bins)
//significant_wave_height = 4 * np.sqrt(moment_0)
//
//# Calculate peak period (inverse of peak frequency)
//peak_period = 1 / peak_frequency
//
//# Print results
//print("Significant Wave Height (Hm0):", significant_wave_height)
//print("Peak Period:", peak_period)


#endif //BUOY_WAVES_H
