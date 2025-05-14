#ifndef GPS_H
#define GPS_H

#include <buoy_data.pb.h>

#define GPS_UART_NR 1
#define GPS_BAUD 9600

#define GPS_RX 16 //GPS Receive
#define GPS_TX 15 //GPS Transmit

bool initializeGPS();
Telemetry getGPS();

#endif //GPS_H
