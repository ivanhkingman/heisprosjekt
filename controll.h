#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "io.h"
#include <stdio.h>

struct Floor {
  int up;
  int down;
  int order;
};

struct Controller {
  struct Floor floors[N_FLOORS];
  elev_motor_direction_t current_direction;
  int last_floor;
};
