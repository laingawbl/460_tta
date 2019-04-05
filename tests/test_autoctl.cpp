#include "../src/manctl.h"
#include "../src/autoctl.h"
#include "../src/drive/irdrv.h"
#include "../src/drive/turretdrv.h"

int main()
{
    init_turret_pwm();
    //set_turret_x_val(50);
    //set_turret_y_val(50);
    
    manctlStart({100, 20});
    autoctlStart({200, 60});

    irDriverStart({50, 0});
    lightDriverStart({50, 10});

    modeSwitchStart({30000, 15170});
    deathMonStart({200, 160});

    OS_Run();

    return 0;
}
