#ifndef DATA_H
#define DATA_H

#include <stdint.h>

typedef struct State {
  int RUNNING;
  int lotCount;
} State;

typedef struct Space {
  uint8_t id;
  uint8_t isOccupied;
  uint64_t timeTaken;
  uint16_t popularity;
} Space;

typedef struct Lot {
  uint8_t id;
  uint8_t spaceCount;
  Space *space;
} Lot;

extern State state;

#endif
