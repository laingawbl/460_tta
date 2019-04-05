#include "../src/manctl.h"

int main(){

    manctlStart({100, 25});

    OS_Run();

    return 0;
}