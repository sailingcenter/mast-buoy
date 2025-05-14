#include "pb_encode.h"
#include "pb_decode.h"
#include "test.h"
#include "buoy_data.pb.h"
#include <stdbool.h>

// Helper functions to initialize data structures
WindData create_wind_data(bool complete_data_set)
{
    return (WindData){
        .has_windSpeedDirection_degrees = true,
        .windSpeedDirection_degrees = 359,
        .has_windSpeedAverage_kmh_scaled100 = true,
        .windSpeedAverage_kmh_scaled100 = 200 * 100,
        .has_windSpeedGust_kmh_scaled100 = complete_data_set,
        .windSpeedGust_kmh_scaled100 = 200 * 100,
        .has_windSpeedLull_kmh_scaled100 = complete_data_set,
        .windSpeedLull_kmh_scaled100 = 200 * 100,
    };
}

AirData create_air_data(bool complete_data_set)
{
    return (AirData){
        .has_airTemperature_celsius_scaled10 = complete_data_set,
        .airTemperature_celsius_scaled10 = 40 * 10,
        .has_airRelativeHumidity_percent_scaled10 = complete_data_set,
        .airRelativeHumidity_percent_scaled10 = 100 * 10,
        .has_airPressure_hpa_scaled100 = complete_data_set,
        .airPressure_hpa_scaled100 = 1000 * 100,
    };
}

CurrentData create_current_data(bool complete_data_set)
{
    return (CurrentData){
        .has_surfaceCurrentDirection_degrees = true,
        .surfaceCurrentDirection_degrees = 359,
        .has_surfaceCurrentSpeed_kmh_scaled100 = true,
        .surfaceCurrentSpeed_kmh_scaled100 = 10 * 100,
        .has_surfaceTemperature_celsius_scaled10 = complete_data_set,
        .surfaceTemperature_celsius_scaled10 = 40 * 10,
    };
}

WaveData create_wave_data(bool complete_data_set)
{
    return (WaveData){
        .has_dominantWavePeriod_seconds_scaled100 = complete_data_set,
        .dominantWavePeriod_seconds_scaled100 = 12 * 100,
        .has_significantWaveHeight_meters_scaled100 = complete_data_set,
        .significantWaveHeight_meters_scaled100 = 3 * 100,
        .has_meanWaveHeightHighestTenth_meters_scaled100 = complete_data_set,
        .meanWaveHeightHighestTenth_meters_scaled100 = 3 * 100,
        .has_maximumWaveHeight_meters_scaled100 = true,
        .maximumWaveHeight_meters_scaled100 = 3 * 100,
    };
}

Telemetry create_telemetry(bool complete_data_set)
{
    return (Telemetry){
        .has_gpsLatitude_degrees_scaled10000000 = true,
        .gpsLatitude_degrees_scaled10000000 = -90 * 10000000,
        .has_gpsLongitude_degrees_scaled10000000 = true,
        .gpsLongitude_degrees_scaled10000000 = -180 * 10000000,
        .has_gpsAltitude_meters_scaled100 = true,
        .gpsAltitude_meters_scaled100 = 1000 * 100,
    };
}

BuoyData create_buoy_data(bool complete_data_set)
{
    return (BuoyData){
        .has_windData = true,
        .windData = create_wind_data(complete_data_set),
        .has_airData = complete_data_set,
        .airData = create_air_data(complete_data_set),
        .has_currentData = true,
        .currentData = create_current_data(complete_data_set),
        .has_waveData = true,
        .waveData = create_wave_data(complete_data_set),
        .has_telemetry = true,
        .telemetry = create_telemetry(complete_data_set),
    };
}

bool encode_buoy_data(const BuoyData* data, uint8_t* buffer, size_t buffer_size, size_t* written)
{
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, buffer_size);

    if (!pb_encode(&ostream, &BuoyData_msg, data))
    {
        return false;
    }

    *written = ostream.bytes_written;
    return true;
}

bool decode_buoy_data(const uint8_t* buffer, size_t buffer_size, BuoyData* decoded)
{
    pb_istream_t istream = pb_istream_from_buffer(buffer, buffer_size);
    BuoyData buoy_data = BuoyData_init_zero;
    *decoded = buoy_data;

    return pb_decode(&istream, &BuoyData_msg, decoded);
}

int validate_decoded_data(const BuoyData* decoded, bool complete_data_set)
{
    int status = 0;

    TEST(decoded->windData.windSpeedDirection_degrees == 359);
    TEST(decoded->windData.windSpeedAverage_kmh_scaled100 == 200 * 100);
    if (complete_data_set)
    {
        TEST(decoded->windData.windSpeedGust_kmh_scaled100 == 200 * 100);
        TEST(decoded->windData.windSpeedLull_kmh_scaled100 == 200 * 100);
    }
    if (complete_data_set)
    {
        TEST(decoded->airData.airTemperature_celsius_scaled10 == 40 * 10);
        TEST(decoded->airData.airRelativeHumidity_percent_scaled10 == 100 * 10);
        TEST(decoded->airData.airPressure_hpa_scaled100 == 1000 * 100);
    }
    TEST(decoded->currentData.surfaceCurrentDirection_degrees == 359);
    TEST(decoded->currentData.surfaceCurrentSpeed_kmh_scaled100 == 10 * 100);
    if (complete_data_set)
    {
        TEST(decoded->currentData.surfaceTemperature_celsius_scaled10 == 40 * 10);
    }
    TEST(decoded->waveData.maximumWaveHeight_meters_scaled100 == 3 * 100);
    if (complete_data_set)
    {
        TEST(decoded->waveData.dominantWavePeriod_seconds_scaled100 == 12 * 100);
        TEST(decoded->waveData.significantWaveHeight_meters_scaled100 == 3 * 100);
        TEST(decoded->waveData.meanWaveHeightHighestTenth_meters_scaled100 == 3 * 100);
    }
    TEST(decoded->telemetry.gpsLatitude_degrees_scaled10000000 == -90 * 10000000);
    TEST(decoded->telemetry.gpsLongitude_degrees_scaled10000000 == -180 * 10000000);
    TEST(decoded->telemetry.gpsAltitude_meters_scaled100 == 1000 * 100);
    TEST(decoded->has_windData == true);
    TEST(decoded->has_airData == complete_data_set);
    TEST(decoded->has_currentData == true);
    TEST(decoded->has_waveData == true);
    TEST(decoded->has_telemetry == true);
    TEST(decoded->windData.has_windSpeedDirection_degrees == true);
    TEST(decoded->windData.has_windSpeedAverage_kmh_scaled100 == true);
    TEST(decoded->windData.has_windSpeedGust_kmh_scaled100 == complete_data_set);
    TEST(decoded->windData.has_windSpeedLull_kmh_scaled100 == complete_data_set);
    TEST(decoded->airData.has_airTemperature_celsius_scaled10 == complete_data_set);
    TEST(decoded->airData.has_airRelativeHumidity_percent_scaled10 == complete_data_set);
    TEST(decoded->airData.has_airPressure_hpa_scaled100 == complete_data_set);
    TEST(decoded->currentData.has_surfaceCurrentDirection_degrees == true);
    TEST(decoded->currentData.has_surfaceCurrentSpeed_kmh_scaled100 == true);
    TEST(decoded->currentData.has_surfaceTemperature_celsius_scaled10 == complete_data_set);
    TEST(decoded->waveData.has_dominantWavePeriod_seconds_scaled100 == complete_data_set);
    TEST(decoded->waveData.has_significantWaveHeight_meters_scaled100 == complete_data_set);
    TEST(decoded->waveData.has_meanWaveHeightHighestTenth_meters_scaled100 == complete_data_set);
    TEST(decoded->waveData.has_maximumWaveHeight_meters_scaled100 == true);
    TEST(decoded->telemetry.has_gpsLatitude_degrees_scaled10000000 == true);
    TEST(decoded->telemetry.has_gpsLongitude_degrees_scaled10000000 == true);
    TEST(decoded->telemetry.has_gpsAltitude_meters_scaled100 == true);

    return status;
}

int test_data_set(bool complete_data_set)
{
    int status = 0;
    size_t written = 0;
    uint8_t buffer[256];

    BuoyData original = create_buoy_data(complete_data_set);
    if (!encode_buoy_data(&original, buffer, sizeof(buffer), &written))
    {
        printf("Encoding failed\n");
        return 1;
    }
    printf("Written: %zu\n", written);

    BuoyData decoded;
    if (!decode_buoy_data(buffer, written, &decoded))
    {
        printf("Decoding failed\n");
        return 1;
    }

    TEST(validate_decoded_data(&decoded, complete_data_set) == 0);

    return status;
}

int main(int argc, char* argv[])
{
    int status = 0;

    printf("Testing minimal data set\n");
    status = test_data_set(false);
    if (status != 0)
    {
        return status;
    }

    printf("Testing complete data set\n");
    status = test_data_set(true);

    return status;
}
