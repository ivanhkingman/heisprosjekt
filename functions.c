#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "io.h"
#include <stdio.h>

void initialize() {
  elev_set_motor_direction(DIRN_DOWN);
  int uninitalized = 1;
  while (uninitalized) {
    if (elev_get_floor_sensor_signal() != -1) {
      elev_set_motor_direction(DIRN_STOP);
      uninitalized = 0;
    }
  }
}

void floorlights(){
  int floor_signal = elev_get_floor_sensor_signal();
  if (floor_signal != -1) {
    elev_set_floor_indicator(floor_signal);
  }
}

void set_lights(){
  for (int i = 1; i < 3; i++) {
    for (int j = BUTTON_CALL_UP; j <= BUTTON_COMMAND; j++){
      if(elev_get_button_signal(j, i)){
        elev_set_button_lamp(j, i, 1);
      }
    }
  }
  if (elev_get_button_signal(BUTTON_CALL_UP,0)){
    elev_set_button_lamp(BUTTON_CALL_UP,0,1);
  }
  else if (elev_get_button_signal(BUTTON_COMMAND,0)){
    elev_set_button_lamp(BUTTON_COMMAND,0,1);
  }
  else if (elev_get_button_signal(BUTTON_CALL_DOWN,3)){
    elev_set_button_lamp(BUTTON_CALL_DOWN,3,1);
  }
  else if (elev_get_button_signal(BUTTON_COMMAND,3)){
    elev_set_button_lamp(BUTTON_COMMAND,3,1);
  }
}

void turn_off_light(){
  for(int i = BUTTON_CALL_UP; i <= BUTTON_COMMAND; i++){
    
  }
}
