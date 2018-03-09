#include "elev.h"
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

int downPriorityHandeled(int currentFloor){
  for(int i = currentFloor; i <= 0; i--){
    printf("");
    if((i!=0)&&(downButtons[i-1] == 1)) {
      return 0;
    }
    if (commandButtons[i] == 1){
      return 0;
      printf("Command request pending\n");
    }
  }
  printf("All down priorities handeled\n");
  return 1;
}

int findEndgoalUp(){
  int goal = -1;
  int level = 0;
  if (elev_get_floor_sensor_signal() != 1){
    level = elev_get_floor_sensor_signal();
  }


  for(int i = level; i < N_FLOORS; i++){
    if(commandButtons[i]){
      goal = i;
    }
    if(i != 3){
    if(upButtons[i]){
      goal = i;
    }
    if((i != 0) && (downButtons[i-1])){
      goal = i;
      }
    }
  }
  return goal;
}

int findEndgoalDown(){
  int goal = -1;
  int level = 0;
  if (elev_get_floor_sensor_signal() != 1){
    level = elev_get_floor_sensor_signal();
  }


  for(int i = level; i >= 0; i--){
    if(commandButtons[i]){
      goal = i;
    }
    if((i != 3) && upButtons[i]){
      goal = i;
    }
    if((i != 0) && (downButtons[i-1])){
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

	elev_set_door_open_lamp(0);
	elev_set_motor_direction(DIRN_STOP);
  checkButtons();
  goToEmergancy();

  for (int i = 0; i < 3; i++) {
    if (upButtons[i]){
      if(i < elev_get_floor_sensor_signal()){
        STATE = DOWN;
        return;
      }
      printf("ABRA");
      STATE = UP;
      return;
    }
    else if (downButtons[i]) {
      if(i+1 < elev_get_floor_sensor_signal()){
        STATE = DOWN;
        return;
      }
      printf("KADABRA");
      STATE = UP;
      return;
    }
  }
  if(commandButtons[1]){
    printf("BOBBY\n" );
  }


  for (int j = 0; j < 4; j++) {
    //printf("%d", j);
    if (commandButtons[j] && (elev_get_floor_sensor_signal() > j)) {
      printf("HEY!");
      STATE = DOWN;
      return;
    }
    if (commandButtons[j] && (elev_get_floor_sensor_signal() < j)) {
      printf("YO!");
		  STATE = UP;
		  return;
    }
  }
}

void downCase(){
  elev_set_motor_direction(DIRN_DOWN);
  int endGoal = findEndgoalDown();

  if(endGoal == -1){
    STATE = IDLE;
    return;
  }

  while (elev_get_floor_sensor_signal() != endGoal){
    endGoal = findEndgoalDown();
    checkButtons();
    lightControl();
    goToEmergancy();

    if(elev_get_floor_sensor_signal() != -1){
      int currentFloor = elev_get_floor_sensor_signal();
      if ((currentFloor != 0) && (downButtons[currentFloor-1])){
        onAndOff(currentFloor);
      }
      if(commandButtons[currentFloor]){
        onAndOff(currentFloor);
      }
    }
  }

  if((upButtons[endGoal]) && (elev_get_floor_sensor_signal() != 3)){
    STATE = UP;
    printf("Ikke her\n");
    return;
  }
  else{
    STATE = IDLE;
  }
}

void upCase(){
  elev_set_motor_direction(DIRN_UP);
  int endGoal = findEndgoalUp();

  if(endGoal == -1){
    STATE = IDLE;
    return;
  }

  while (elev_get_floor_sensor_signal() != endGoal){
    endGoal = findEndgoalUp();
    checkButtons();
    lightControl();
    goToEmergancy();

    if(elev_get_floor_sensor_signal() != -1){
      int currentFloor = elev_get_floor_sensor_signal();
      if (currentFloor != 3 && (upButtons[currentFloor])){
        onAndOff(currentFloor);
      }
      if(commandButtons[currentFloor]){
        onAndOff(currentFloor);
      }
    }
  }

  if((downButtons[endGoal-1]) && (elev_get_floor_sensor_signal() != 0)){
    STATE = DOWN;
    return;
  }
  else{
    STATE = IDLE;
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
