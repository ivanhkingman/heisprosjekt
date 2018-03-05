#include "elev.h"
#include "controll.h"
#include "functions.h"
#include <stdio.h>


int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    struct Controller controllTest;
      

    initialize();
    wait(3);
    printf("Press STOP button to stop elevator and exit program.\n");

    elev_set_motor_direction(DIRN_UP);

    while (1) {
    floorlights();

    if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
        elev_set_motor_direction(DIRN_DOWN);
    } else if (elev_get_floor_sensor_signal() == 0) {
        elev_set_motor_direction(DIRN_UP);
    }

    // Stop elevator and exit program if the stop button is pressed
    if (elev_get_stop_signal()) {
        elev_set_motor_direction(DIRN_STOP);
        break;
      }
    }

  return 0;
}
