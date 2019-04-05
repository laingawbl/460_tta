#include "../src/manctl.h"

int main(){

    manctlStart({50, 25});

    OS_Run();

    return 0;
}