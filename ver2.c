#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "controll.h"
#include "io.h"
#include <stdio.h>
#include <time.h>

#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "controll.h"
#include "io.h"
#include <stdio.h>
#include <time.h>


int upButtons[3];
int downButtons[3];
int commandButtons[4];
int STATE;
//int LAST_FLOOR;
int DIR;


enum states{
  IDLE = 0,
  UP = 1,
  DOWN = 2,
  EMERGANCY = 3,
};

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
      downButtons[j-1] = 1; //HER MÅ man ha -1 ?? right
    }
  }
  //commandbuttons
  for(int k = 0; k < 4; k++){
    if(elev_get_button_signal(2,k)){
      commandButtons[k] = 1;
    }
  }
}

void upAndDown(){
  if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
      elev_set_motor_direction(DIRN_DOWN);
  }
  else if (elev_get_floor_sensor_signal() == 0) {
      elev_set_motor_direction(DIRN_UP);
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

void goToEmergancy(){
  if (elev_get_stop_signal()) {
	  STATE = EMERGANCY;
	  return;
  }
}

/*int lastFloor(){
	while (elev_get_floor_sensor_signal() != -1) {
		LAST_FLOOR = elev_get_floor_sensor_signal();
	}
  return LAST_FLOOR;
}
*/

void onAndOff(int last_floor){
  elev_set_motor_direction(DIRN_STOP);
  printf("Doors are open... \n");
  elev_set_door_open_lamp(1);

  if (STATE == UP){
    printf("Gikk av\n");
    if(last_floor != 3){
      printf("denen loopen\n");
      upButtons[last_floor] = 0;
    }
    commandButtons[last_floor] = 0;
  }

  if(STATE == DOWN){
    if(last_floor != 0){
      downButtons[last_floor-1] = 0;
    }
    commandButtons[last_floor] = 0;
  }

  int retTime = time(0) + 3;
  while(time(0) < retTime){
    checkButtons();
    lightControl();
    goToEmergancy();
  }
  elev_set_door_open_lamp(0);
  printf("Doors are closed...\n");
}

void initialize() {
  if (!elev_init()) {
      printf("Unable to initialize elevator hardware!\n");
      return;
  }
  printf("Initializing...\n");
  STATE = IDLE;
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

void testStop(){
  if (elev_get_stop_signal()) {
    elev_set_motor_direction(DIRN_STOP);
    return;
  }
}

int checkForUpReq(){
  int goal = -1;
  for (int i = 0; i < 4; i++ ){
    if(downButtons[2]){ //Hvis ingen skal opp, men i 3. og noen i 4. skal ned
      goal = 3;
    }
    else if (i != 3 && upButtons[i]){ //gjør ikke denne i 4 etasje
      goal = i;
    }
    else if (commandButtons[i]){
      goal = i;
    }
  }
  return goal;
}

int checkForDownReq(){
  int goal = -1;
  for (int i = 0; i < 4; i++ ){
    if(upButtons[0]){
      goal = 0;
    }
    else if (i != 3 && downButtons[i]){ //gjør ikke denne i 4 etasje
        goal = i+1;
    }
    else if (commandButtons[i]){
      goal = i;
    }
  }
  return goal;
}

/////////////////////
//Functios






//Cases
////////////////////

void emergancyCase(){
	printf("Emergancy button pressed");
	while (elev_get_stop_signal()) { //FAC test
		elev_set_stop_lamp(1);
		elev_set_motor_direction(DIRN_STOP);
		resetOrders();
		lightControl();
		if (elev_get_floor_sensor_signal() != -1) {
			elev_set_door_open_lamp(1);
		}
	}
	elev_set_stop_lamp(0);
	printf("%s %d" ,"Last visited before emergancy: ", elev_get_floor_sensor_signal()+1);
  while(elev_get_floor_sensor_signal() == -1){
    elev_set_motor_direction(DIRN_DOWN);
  }
  elev_set_motor_direction(DIRN_STOP);
  printf("%s %d" ,"Elevator up and running at floor\n", elev_get_floor_sensor_signal()+1);

	STATE = IDLE;
}

void idleCase(){
  //printf("IDLE\n" );
	elev_set_door_open_lamp(0);
	elev_set_motor_direction(DIRN_STOP);
  goToEmergancy();


	 for (int i = 0; i < 3; i++) {
     if (upButtons[i]){
			 STATE = UP;
			 return;
     }
    else if (downButtons[i]){
			  STATE = DOWN;
        return;
    }
	 }

 for (int j = 0; j < 4; j++) {
	  if ((commandButtons[j]) && (elev_get_floor_sensor_signal() < j)) {
		  STATE = UP;
		  return;
	  }
	  else if ((commandButtons[j]) && (elev_get_floor_sensor_signal() > j)) {
		  STATE = DOWN;
		  return;
	  }
  }
}

void downCase(){
  elev_set_motor_direction(DIRN_DOWN);
  printf("var her");
  goToEmergancy();
  int upgoald = -1;
  int downgoald = -1;
  int commandgoald = -1;

  
  if(elev_get_floor_sensor_signal() != -1){
    for(int i = elev_get_floor_sensor_signal(); i <= 0 ; i--){
      if((i != 3) && (upButtons[i])){
      upgoald = i;
      }
      if((i != 0) && (downButtons[i-1])){
      downgoald = i;
      }
    }
    for (int j = elev_get_floor_sensor_signal(); j <= 0; j--){
      if(commandButtons[j]){
        commandgoald = j;
        printf("yes");
      }
    }

  //betjener opp og command goals

    if ((elev_get_floor_sensor_signal() != 0 && (downButtons[elev_get_floor_sensor_signal()-1])) || commandButtons[elev_get_floor_sensor_signal()]) {
      onAndOff(elev_get_floor_sensor_signal());
    }


    if((downgoald == -1) && (commandgoald == -1) && (upgoald != 1)){
      if(elev_get_floor_sensor_signal() == upgoald){
        STATE = UP;
        return;
      }
    }

    if((upgoald == -1) && (commandgoald == -1) && (downgoald == -1)){
      if(elev_get_floor_sensor_signal() == downgoald){
        STATE = IDLE;
        return;
      }
    }
  }
}

void upCase(){
  elev_set_motor_direction(DIRN_UP);
  goToEmergancy();
  int currentFlooru = -1;
  int upgoalu = -1;
  int downgoalu = -1;
  int commandgoalu = -1;

  if(elev_get_floor_sensor_signal() != -1){
    currentFlooru = elev_get_floor_sensor_signal();
  }

  for(int i = currentFlooru; i < 3; i++){
    if(upButtons[i]){
      upgoalu = i;
    }
    if((i != 0) && (downButtons[i-1])){
      downgoalu = i;
    }
  }

  for (int j = currentFlooru; j < N_FLOORS; j++){
    if(commandButtons[j]){
      commandgoalu = j;
    }
  }
  //betjener opp og command goals

  if(elev_get_floor_sensor_signal() != -1){
    if ((currentFlooru != 3 && (upButtons[currentFlooru])) || commandButtons[currentFlooru]) {
      onAndOff(currentFlooru);
    }
  }

  if((upgoalu == -1) && (commandgoalu == -1) && (downgoalu != -1)){
    if(currentFlooru == downgoalu){
      STATE = DOWN;
      return;
    }
  }

  if((upgoalu == -1) && (commandgoalu == -1) && (downgoalu == -1)){
    if(currentFlooru == downgoalu){
      STATE = IDLE;
      return;
    }
  }




}

void statemachine(){

  switch (STATE) {
    case IDLE:
      idleCase();
      break;
    case UP:
      upCase();
      break;
    case DOWN:
      downCase();
      break;
    case EMERGANCY:
      emergancyCase();
      break;
    default:
      STATE = IDLE;
      break;
  }
}

