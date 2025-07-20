#include "qpc.h"
#include "bsp.h"
#include "incubator_hatcher.h"

int main() {
    QF_init();       // initialize the framework and the underlying RT kernel
    BSP_init();      // initialize the BSP
    BSP_start();     // start the AOs/Threads
    return QF_run(); // run the QF application
}


