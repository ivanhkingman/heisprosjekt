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
