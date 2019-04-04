#include "../src/manctl.h"

int main(){

    manctlStart({2000, 0});

    OS_Run();

    return 0;
}