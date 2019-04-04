#include "../src/manctl.h"

int main(){

    manctlStart({100, 50});

    OS_Run();

    return 0;
}