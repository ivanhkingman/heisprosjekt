#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "io.h"
#include <stdio.h>


int upButtons[3];
int downButtons[3];
int commandButtons[4];

enum states{
  IDLE = 0,
  UP = 1,
  DOWN = 2,
  WAIT = 3,
  EMERGANCY = 4,
};

void statemachine(){
  int a = IDLE;

  switch (a) {
    case IDLE:
      //idleCase();
      break;
    case UP:
      //upCase();
    case DOWN:
      //downCase();
      break;
    case WAIT:
      //waitCase();
      break;
    case EMERGANCY:
      //emergancyCase();
      break;
    default:
      a = IDLE;
      break;
  }
}

void resetOrders(){
  for(int i = 0; i < 3; i++){
    upButtons[i] = 0;
    downButtons[i] = 0;
  }
  for(int j = 0; j < N_FLOORS; j++){
    commandButtons[j] = 0;
  }
}

void checkButtons(){
  //upbuttons
  for(int i = 0; i < 3; i++){
    if(elev_get_button_signal(0,i)){
      upButtons[i] = 1;
    }
  }
  //downButtons
  for(int j = 1; j< 4; j++){
    if(elev_get_button_signal(1,j)){
      downButtons[j] = 1;
    }
  }
  //commandbuttons
  for(int k = 0; k < 4; k++){
    if(elev_get_button_signal(2,k)){
      commandButtons[k] = 1;
    }
  }
}

void lightControl(){
  //upbuttons
  for(int i = 0; i < 3; i++){
      elev_set_button_lamp(0,i,upButtons[i]);
      elev_set_button_lamp(1,i+1,downButtons[i]);
  }
  for(int k = 0; k < 4; k++){
      elev_set_button_lamp(2,k,commandButtons[k]);
  }
}

/*void idleCase(){
  elev_set_motor_direction(DIRN_STOP);
}

void upCase(){
  elev_set_motor_direction(DIRN_UP);
}

void downCase(){
  elev_set_motor_direction(DIRN_DOWN);
}*/

void emergancyCase(){
  elev_set_motor_direction(DIRN_STOP);
  //clearButtons();
}
