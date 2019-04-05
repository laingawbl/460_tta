#include "../src/manctl.h"
#include "../src/autoctl.h"
#include "../src/drive/irdrv.h"

int main(){

    autoctlStart({200, 150});
    manctlStart({200, 50});

    irDriverStart({50, 25});

    OS_Run();

    return 0;
}