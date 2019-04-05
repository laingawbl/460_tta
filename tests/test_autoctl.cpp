#include "../src/manctl.h"
#include "../src/autoctl.h"
#include "../src/drive/irdrv.h"

int main(){

    manctlStart({100, 0});
    autoctlStart({100, 50});

    irDriverStart({50, 20});

    OS_Run();

    return 0;
}