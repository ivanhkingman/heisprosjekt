#include "elev.h"
#include "functions.h"
#include "channels.h"
#include "io.h"
#include <stdio.h>

int upButtons[3];
int downButtons[3];
int commandButtons[4];

void resetOrders();

void checkButtons();

void emergancyCase();

void lightControl();

int lastFloor();

void goToEmergancy();

void onAndOff(int last_floor);

void downCase();

void upCase();

//initialiserer heisen
void initialize();

//etasjelys
void floorlights();

//funksjon som pauser kjøring av programmet
void wait(int secs);

//heis går kontinuerlig opp og ned
void upAndDown();

//stop for testing
void testStop();

void statemachine();

int checkForUpReq();

int checkForDownReq();

int downPriorityHandeled(int currentFloor);

int upPriorityHandeled(int currentFloor);

int findEndgoalUp();

int findEndgoalDown();

int noButtons();

int noCommandButtons();

int noUpButtons();

int noDownButtons();

int noDownReqAbove(int currentFloor);

int noUpReqBelow(int currentFloor);
