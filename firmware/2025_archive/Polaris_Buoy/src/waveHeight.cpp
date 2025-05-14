#include <Arduino.h>
#include <Adafruit_BNO055.h>

#include "buoy_data.pb.h"

const int MAX_SAMPLES = 600;
static float accZ_buffer[MAX_SAMPLES];
static float z_filtered[MAX_SAMPLES];
static float velocity[MAX_SAMPLES];
static float position[MAX_SAMPLES];

static imu::Vector<3> rotateVectorByQuaternion(const imu::Vector<3>& v, const imu::Quaternion& q) {
  float w = q.w(), x = q.x(), y = q.y(), z = q.z();
  float vx = v.x(), vy = v.y(), vz = v.z();
  float qw2 = w*w, qx2 = x*x, qy2 = y*y, qz2 = z*z;

  float r00 = qw2 + qx2 - qy2 - qz2;
  float r01 = 2 * (x*y - w*z);
  float r02 = 2 * (x*z + w*y);
  float r10 = 2 * (x*y + w*z);
  float r11 = qw2 - qx2 + qy2 - qz2;
  float r12 = 2 * (y*z - w*x);
  float r20 = 2 * (x*z - w*y);
  float r21 = 2 * (y*z + w*x);
  float r22 = qw2 - qx2 - qy2 + qz2;

  float rx = r00 * vx + r01 * vy + r02 * vz;
  float ry = r10 * vx + r11 * vy + r12 * vz;
  float rz = r20 * vx + r21 * vy + r22 * vz;

  return imu::Vector<3>(rx, ry, rz);
}

static float getWaveHeight(Adafruit_BNO055& bno) {
  for (int i = 0; i < MAX_SAMPLES; i++) {
    sensors_event_t event;
    bno.getEvent(&event);
    imu::Quaternion q = bno.getQuat();
    imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    imu::Vector<3> acc_earth = rotateVectorByQuaternion(acc, q);
    accZ_buffer[i] = acc_earth.z();
    delay(10); // 100 Hz
  }

  for (int i = 1; i < MAX_SAMPLES - 1; i++)
    z_filtered[i] = 0.25 * accZ_buffer[i - 1] + 0.5 * accZ_buffer[i] + 0.25 * accZ_buffer[i + 1];
  z_filtered[0] = accZ_buffer[0];
  z_filtered[MAX_SAMPLES - 1] = accZ_buffer[MAX_SAMPLES - 1];

  float meanZ = 0.0;
  for (int i = 0; i < MAX_SAMPLES; i++) meanZ += z_filtered[i];
  meanZ /= MAX_SAMPLES;
  for (int i = 0; i < MAX_SAMPLES; i++) z_filtered[i] -= meanZ;

  float dt = 0.01;
  velocity[0] = 0.0;
  for (int i = 1; i < MAX_SAMPLES; i++)
    velocity[i] = velocity[i - 1] + 0.5 * (z_filtered[i] + z_filtered[i - 1]) * dt;

  position[0] = 0.0;
  for (int i = 1; i < MAX_SAMPLES; i++)
    position[i] = position[i - 1] + 0.5 * (velocity[i] + velocity[i - 1]) * dt;

  float minPos = position[0], maxPos = position[0];
  for (int i = 1; i < MAX_SAMPLES; i++) {
    if (position[i] < minPos) minPos = position[i];
    if (position[i] > maxPos) maxPos = position[i];
  }

  float waveHeight = maxPos - minPos;
  return (isnan(waveHeight) || waveHeight < 0.01 || waveHeight > 10.0) ? -1 : waveHeight;
}

// Usage
static WaveData getWaveHeightInfo(Adafruit_BNO055& bno) {
  WaveData waveData = WaveData_init_zero;
  float h = getWaveHeight(bno);
  waveData.has_maximumWaveHeight_meters_scaled100 = h >= 0;
  waveData.maximumWaveHeight_meters_scaled100 = h * 100;
  return waveData;
}

