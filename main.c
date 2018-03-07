#include "elev.h"
#include "controll.h"
#include "functions.h"
#include <stdio.h>


int main() {

    initialize();

    wait(3);

    printf("Press STOP button to stop elevator and exit program.\n");

    elev_set_motor_direction(DIRN_UP);

    while (1) {

      floorlights();
      lightControl();

      checkButtons();

      upAndDown();

      int b = elev_get_floor_sensor_signal();
      if (b != -1){
        if(b != 3){
          upButtons[b] = 0;
        }
        if (b != 0){
          downButtons[b] = 0;
        }
        commandButtons[b] = 0;
      }



      testStop();

    }

  return 0;

}
