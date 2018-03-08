#include "elev.h"
#include "controll.h"
#include <stdio.h>


int main() {

    initialize();

    wait(3);

    printf("Press STOP button to stop elevator and exit program.\n");

    while (1) {

      statemachine();
      floorlights();
      lightControl();

      checkButtons();
      testStop();

    /*  int b = elev_get_floor_sensor_signal();
      if (b != -1){
        if(b != 3){
          upButtons[b] = 0;
        }
        if (b != 0){
          downButtons[b-1] = 0;
        }
        commandButtons[b] = 0;
      }



      testStop();*/

    }

  return 0;

}
