#ifndef ECOWITT_PARSER_H
#define ECOWITT_PARSER_H

void initializeOutputSerial(unsigned long baudRate);

void parse_ecowitt(char* message);

#endif //ECOWITT_PARSER_H
