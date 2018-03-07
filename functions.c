#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "controll.h"
#include "io.h"
#include <stdio.h>
#include <time.h>



void initialize() {
  if (!elev_init()) {
      printf("Unable to initialize elevator hardware!\n");
      return;
  }
  printf("Initializing...\n");

  resetOrders();

  elev_set_motor_direction(DIRN_DOWN);
  int uninitalized = 1;
  while (uninitalized) {
    if (elev_get_floor_sensor_signal() != -1) {
      elev_set_motor_direction(DIRN_STOP);
      uninitalized = 0;
    }
  }
  printf("Elevator is initialized\n");
}

void floorlights(){
  int floor_signal = elev_get_floor_sensor_signal();
  if (floor_signal != -1) {
    elev_set_floor_indicator(floor_signal);
  }
}

void wait(int secs){
  int retTime = time(0) + secs;
  while(time(0) < retTime);
}

void upAndDown(){
  if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
      elev_set_motor_direction(DIRN_DOWN);
  }
  else if (elev_get_floor_sensor_signal() == 0) {
      elev_set_motor_direction(DIRN_UP);
  }
}

void testStop(){
  if (elev_get_stop_signal()) {
    elev_set_motor_direction(DIRN_STOP);
    return;
  }
}
