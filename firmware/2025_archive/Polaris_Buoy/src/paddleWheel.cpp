#include "driver/pcnt.h"
#include "Arduino.h"

#include "buoy_data.pb.h"

// Constants
const int pulsePin = 12;  // GPIO for paddlewheel input
const double PADDLE_CIRCUMFERENCE_CM = 11.624;
const int PULSES_PER_ROTATION = 6;
const double CM_PER_SEC_TO_KNOTS = 1.0 / 51.444;
const unsigned long intervalMs = 10000;  // 10 seconds

// Returns just the speed in knots
static double readWaterSpeedPCNT() {
  // Configure PCNT
  pcnt_config_t pcnt_config;
  pcnt_config.pulse_gpio_num = pulsePin;
  pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
  pcnt_config.channel = PCNT_CHANNEL_0;
  pcnt_config.unit = PCNT_UNIT_0;
  pcnt_config.pos_mode = PCNT_COUNT_INC;
  pcnt_config.neg_mode = PCNT_COUNT_INC;
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;

  pcnt_unit_config(&pcnt_config);
  pcnt_counter_clear(PCNT_UNIT_0);

  // Count pulses over interval
  unsigned long startTime = millis();
  while (millis() - startTime < intervalMs) {
    delay(10);
  }

  // Read pulse count
  int16_t pulseCount = 0;
  pcnt_get_counter_value(PCNT_UNIT_0, &pulseCount);

  // Convert to kmh
  double rotations = pulseCount / double(PULSES_PER_ROTATION);
  double distance_cm = rotations * PADDLE_CIRCUMFERENCE_CM;
  double speed_cm_per_s = distance_cm / (intervalMs / 1000.0);
  return speed_cm_per_s * 60 * 60 / 100;
}

// Returns water speed as a compact string
static CurrentData getWaterSpeedInfo() {
  CurrentData currentData = CurrentData_init_zero;
  double speed_kmh = readWaterSpeedPCNT();
  currentData.has_surfaceCurrentSpeed_kmh_scaled100 = true;
  currentData.surfaceCurrentSpeed_kmh_scaled100 = speed_kmh * 100;
  return currentData;
}
