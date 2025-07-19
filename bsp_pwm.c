#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define PWM_CLK_DIVIDER     64

// Heater (PWM0 Gen0 Out1 ? PF1)
#define HEATER_PWM_BASE     PWM0_BASE
#define HEATER_PWM_GEN      PWM_GEN_0
#define HEATER_PWM_OUT      PWM_OUT_1
#define HEATER_PWM_BIT      PWM_OUT_1_BIT
#define HEATER_GPIO_PORT    GPIO_PORTF_BASE
#define HEATER_GPIO_PIN     GPIO_PIN_1
#define HEATER_GPIO_CFG     GPIO_PF1_M0PWM1
#define HEATER_FREQ_HZ      1000

// Humidifier (PWM1 Gen0 Out2 ? PE4)
#define HUMID_PWM_BASE      PWM1_BASE
#define HUMID_PWM_GEN       PWM_GEN_0
#define HUMID_PWM_OUT       PWM_OUT_2
#define HUMID_PWM_BIT       PWM_OUT_2_BIT
#define HUMID_GPIO_PORT     GPIO_PORTE_BASE
#define HUMID_GPIO_PIN      GPIO_PIN_4
#define HUMID_GPIO_CFG      GPIO_PE4_M1PWM2
#define HUMID_FREQ_HZ       2000

// Cooler (PWM0 Gen1 Out3 ? PF3)
#define COOLER_PWM_BASE     PWM0_BASE
#define COOLER_PWM_GEN      PWM_GEN_1
#define COOLER_PWM_OUT      PWM_OUT_3
#define COOLER_PWM_BIT      PWM_OUT_3_BIT
#define COOLER_GPIO_PORT    GPIO_PORTF_BASE
#define COOLER_GPIO_PIN     GPIO_PIN_3
#define COOLER_GPIO_CFG     GPIO_PF3_M0PWM3
#define COOLER_FREQ_HZ      1000

void BSP_initPWM(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Heater pin
    GPIOPinConfigure(HEATER_GPIO_CFG);
    GPIOPinTypePWM(HEATER_GPIO_PORT, HEATER_GPIO_PIN);

    // Humidifier pin
    GPIOPinConfigure(HUMID_GPIO_CFG);
    GPIOPinTypePWM(HUMID_GPIO_PORT, HUMID_GPIO_PIN);

    // Cooler pin
    GPIOPinConfigure(COOLER_GPIO_CFG);
    GPIOPinTypePWM(COOLER_GPIO_PORT, COOLER_GPIO_PIN);

    // Heater setup
    PWMGenConfigure(HEATER_PWM_BASE, HEATER_PWM_GEN, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    uint32_t heaterClock = SysCtlClockGet() / PWM_CLK_DIVIDER;
    uint32_t heaterLoad = heaterClock / HEATER_FREQ_HZ;
    PWMGenPeriodSet(HEATER_PWM_BASE, HEATER_PWM_GEN, heaterLoad);
    PWMGenEnable(HEATER_PWM_BASE, HEATER_PWM_GEN);
    PWMOutputState(HEATER_PWM_BASE, HEATER_PWM_BIT, true);

    // Humidifier setup
    PWMGenConfigure(HUMID_PWM_BASE, HUMID_PWM_GEN, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    uint32_t humidClock = SysCtlClockGet() / PWM_CLK_DIVIDER;
    uint32_t humidLoad = humidClock / HUMID_FREQ_HZ;
    PWMGenPeriodSet(HUMID_PWM_BASE, HUMID_PWM_GEN, humidLoad);
    PWMGenEnable(HUMID_PWM_BASE, HUMID_PWM_GEN);
    PWMOutputState(HUMID_PWM_BASE, HUMID_PWM_BIT, true);

    // Cooler setup
    PWMGenConfigure(COOLER_PWM_BASE, COOLER_PWM_GEN, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    uint32_t coolerClock = SysCtlClockGet() / PWM_CLK_DIVIDER;
    uint32_t coolerLoad = coolerClock / COOLER_FREQ_HZ;
    PWMGenPeriodSet(COOLER_PWM_BASE, COOLER_PWM_GEN, coolerLoad);
    PWMGenEnable(COOLER_PWM_BASE, COOLER_PWM_GEN);
    PWMOutputState(COOLER_PWM_BASE, COOLER_PWM_BIT, true);
}



void BSP_setHeaterPower(float duty);
void BSP_setHeaterPower(float duty) {
    uint32_t load = SysCtlClockGet() / PWM_CLK_DIVIDER / HEATER_FREQ_HZ;
    uint32_t pulse = (uint32_t)((duty / 100.0f) * load);
    PWMPulseWidthSet(HEATER_PWM_BASE, HEATER_PWM_OUT, pulse);
}


void BSP_setHumidifierPower(float duty);
void BSP_setHumidifierPower(float duty) {
    uint32_t load = SysCtlClockGet() / PWM_CLK_DIVIDER / HUMID_FREQ_HZ;
    uint32_t pulse = (uint32_t)((duty / 100.0f) * load);
    PWMPulseWidthSet(HUMID_PWM_BASE, HUMID_PWM_OUT, pulse);
}


void BSP_setCoolerPower(float dutyPercent);
void BSP_setCoolerPower(float dutyPercent) {
    uint32_t load = SysCtlClockGet() / PWM_CLK_DIVIDER / COOLER_FREQ_HZ;
    uint32_t pulse = (uint32_t)((dutyPercent / 100.0f) * load);
    PWMPulseWidthSet(COOLER_PWM_BASE, COOLER_PWM_OUT, pulse);
}
