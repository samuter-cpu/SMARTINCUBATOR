#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/i2c.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "bsp_pwm.h"



// Frequency generataor 1.7 MHZ
#define ULTRASONIC_GPIO_PORT   GPIO_PORTB_BASE
#define ULTRASONIC_GPIO_PIN    GPIO_PIN_2
#define ULTRASONIC_GPIO_CFG    GPIO_PB2_T3CCP0
#define ULTRASONIC_TIMER_BASE  TIMER3_BASE
#define ULTRASONIC_TIMER_CFG   TIMER_CFG_A_PWM
#define ULTRASONIC_FREQ        1700000  // 1.7 MHz

// lcd DISPLAY
#define LCD_I2C_BASE    I2C0_BASE
#define LCD1_ADDR       0x27
#define LCD2_ADDR       0x26

// Co2 sensor i2c 
#define SCD30_I2C_BASE         I2C1_BASE
#define SCD30_I2C_PORT         GPIO_PORTA_BASE
#define SCD30_SCL_PIN          GPIO_PIN_6
#define SCD30_SDA_PIN          GPIO_PIN_7
#define SCD30_SCL_CFG          GPIO_PA6_I2C1SCL
#define SCD30_SDA_CFG          GPIO_PA7_I2C1SDA
#define SCD30_I2C_ADDR        0x61
#define SCD30_CMD_READ_CO2    0x0300  // Continuous measurement

// fan ventilation
#define FAN_GPIO_PORT          GPIO_PORTF_BASE
#define INLET_FAN_PIN          GPIO_PIN_4
#define OUTLET_FAN_PIN         GPIO_PIN_5



void BSP_initUltrasonic(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(ULTRASONIC_GPIO_CFG);
    GPIOPinTypeTimer(ULTRASONIC_GPIO_PORT, ULTRASONIC_GPIO_PIN);
    TimerConfigure(ULTRASONIC_TIMER_BASE, ULTRASONIC_TIMER_CFG);
    uint32_t timerClock = SysCtlClockGet();
    uint32_t loadVal = timerClock / ULTRASONIC_FREQ;
    TimerLoadSet(ULTRASONIC_TIMER_BASE, TIMER_A, loadVal);
    TimerMatchSet(ULTRASONIC_TIMER_BASE, TIMER_A, loadVal / 2); // 50% duty
}


void BSP_initLCDs(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    I2CMasterInitExpClk(LCD_I2C_BASE, SysCtlClockGet(), false); // Standard mode
}



void BSP_initCO2Sensor(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(SCD30_SCL_CFG);
    GPIOPinConfigure(SCD30_SDA_CFG);
    GPIOPinTypeI2CSCL(SCD30_I2C_PORT, SCD30_SCL_PIN);
    GPIOPinTypeI2C(SCD30_I2C_PORT, SCD30_SDA_PIN);

    I2CMasterInitExpClk(SCD30_I2C_BASE, SysCtlClockGet(), true);  // Fast mode
}


void BSP_initFanRelays(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(FAN_GPIO_PORT, INLET_FAN_PIN | OUTLET_FAN_PIN);
    BSP_stop_fans(); // Ensure safe default state
}



void BSP_init(void) {
    // Configure the system clock to 120 MHz.
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                        SYSCTL_OSC_MAIN |
                        SYSCTL_USE_PLL |
                        SYSCTL_CFG_VCO_480), 120000000);

    // Enable the GPIO port for the DHT11 sensor.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK))
    {
    }

    // Enable and configure UART0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    BSP_initLCDs();
    BSP_initPWM();
    BSP_initUltrasonic();
    BSP_initCO2Sensor();
    BSP_initFanRelays();
}

void BSP_start(void) {
    // initialize event pools
  
  static QEvt const *tempQueueSto[10];
    TemperatureControl_ctor();
    QACTIVE_START(AO_TemperatureControl,
        TEMPERATURE_CONTROL_PRIO,            // QP prio. of the AO
        tempQueueSto,           // event queue storage
        Q_DIM(tempQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
  static QEvt const *humQueueSto[10];
    HumidityControl_ctor();
    QACTIVE_START(AO_HumidityControl,
        HUMIDITY_CONTROL_PRIO,            // QP prio. of the AO
        humQueueSto,           // event queue storage
        Q_DIM(humQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
    static QEvt const *co2QueueSto[10];
    CO2Control_ctor();
    QACTIVE_START(AO_CO2Control,
        CO2_CONTROL_PRIO,            // QP prio. of the AO
        co2QueueSto,           // event queue storage
        Q_DIM(co2QueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
    static QEvt const *motQueueSto[10];
    MotorControl_ctor();
    QACTIVE_START(AO_MotorControl,
        MOTOR_CONTROL_PRIO,            // QP prio. of the AO
        motQueueSto,           // event queue storage
        Q_DIM(motQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
    static QEvt const *lcdQueueSto[10];
    LcdDisplay_ctor();
    QACTIVE_START(AO_LcdDisplay,
        LCD_DISPLAY_PRIO,            // QP prio. of the AO
        lcdQueueSto,           // event queue storage
        Q_DIM(lcdQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
    static QEvt const *almQueueSto[10];
    AlarmControl_ctor();
    QACTIVE_START(AO_AlarmControl,
        ALARM_CONTROL_PRIO,            // QP prio. of the AO
        almQueueSto,           // event queue storage
        Q_DIM(almQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
    
    static QEvt const *hatchQueueSto[10];
    HatchMode_ctor();
    QACTIVE_START(AO_HatchMode,
        HATCH_MODE_PRIO,            // QP prio. of the AO
        hatchQueueSto,           // event queue storage
        Q_DIM(hatchQueueSto),    // queue length [events]
        (void *)0, 0U,           // no stack storage
        (void *)0);              // no initialization param
}



void BSP_enableUltrasonic(void) {
    TimerEnable(ULTRASONIC_TIMER_BASE, TIMER_A);
}
void BSP_disableUltrasonic(void) {
    TimerDisable(ULTRASONIC_TIMER_BASE, TIMER_A);
}


//Helper function to be called from lcd update functions
void BSP_i2c_write_line(uint8_t addr, uint8_t line_num, uint8_t *line) {
     uint8_t cursor_cmd = (line_num == 0) ? 0x80 : 0xC0; // LCD set cursor command
     I2CMasterSlaveAddrSet(LCD_I2C_BASE, addr, false);

    // Set cursor
    I2CMasterDataPut(LCD_I2C_BASE, cursor_cmd);
    I2CMasterControl(LCD_I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(LCD_I2C_BASE));

    // Write 16 bytes
    for (int i = 0; i < 16; ++i) {
        I2CMasterDataPut(LCD_I2C_BASE, line[i]);
        I2CMasterControl(LCD_I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        while (I2CMasterBusy(I2C_BASE));
    }
}


void BSP_updateLCD1(bool show_temp, float temp, bool show_humidity, float humidity) {
    char line[16] = "                ";  // 16 blank chars

    if (show_temp) {
        snprintf(line, sizeof(line), "Temp: %.1fC", temp);
        BSP_i2c_write_line(LCD1_ADDR, 0, (uint8_t *)line); // Line 0 (top)
    }

    if (show_humidity) {
        snprintf(line, sizeof(line), "Humi: %.1f%%", humidity);
        BSP_i2c_write_line(LCD1_ADDR, 1, (uint8_t *)line); // Line 1 (bottom)
    }
}




void BSP_updateLCD2(float co2, float o2, const char *time_str) {
    char line1[16], line2[16];
    snprintf(line1, sizeof(line1), "CO2: %.0f O2: %.0f", co2, o2);
    snprintf(line2, sizeof(line2), "Time: %s", time_str);

    uint8_t data[32];
    memcpy(&data[0], line1, 16);
    memcpy(&data[16], line2, 16);
    BSP_i2c_write(LCD2_ADDR, data, 32);
}

char *BSP_get_time_string(void) {
    static char time_str[9]; // "HH:MM:SS"
    uint8_t raw[3];  // Hours, Minutes, Seconds

    // Read RTC registers at 0x00
    I2CMasterSlaveAddrSet(LCD_I2C_BASE, 0x68, false);
    I2CMasterDataPut(LCD_I2C_BASE, 0x00);
    I2CMasterControl(LCD_I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(LCD_I2C_BASE));

    I2CMasterSlaveAddrSet(LCD_I2C_BASE, 0x68, true);
    for (int i = 0; i < 3; ++i) {
        I2CMasterControl(LCD_I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while (I2CMasterBusy(LCD_I2C_BASE));
        raw[i] = I2CMasterDataGet(LCD_I2C_BASE);
    }

    snprintf(time_str, sizeof(time_str), "%02x:%02x:%02x", raw[2], raw[1], raw[0]); // HH:MM:SS
    return time_str;
}


float BSP_read_co2_sensor(void) {
    uint8_t readBuffer[6];
    uint16_t raw_co2 = 0;

    // Start transmission to sensor
    I2CMasterSlaveAddrSet(SCD30_I2C_BASE, SCD30_I2C_ADDR, false);  // Write
    I2CMasterDataPut(SCD30_I2C_BASE, (SCD30_CMD_READ_CO2 >> 8) & 0xFF);
    I2CMasterControl(SCD30_I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(SCD30_I2C_BASE));

    // Switch to read mode
    I2CMasterSlaveAddrSet(SCD30_I2C_BASE, SCD30_I2C_ADDR, true);
    for (int i = 0; i < 6; ++i) {
        I2CMasterControl(SCD30_I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(I2CMasterBusy(SCD30_I2C_BASE));
        readBuffer[i] = I2CMasterDataGet(SCD30_I2C_BASE);
    }

    // Convert raw data (MSB, LSB)
    raw_co2 = ((uint16_t)readBuffer[0] << 8) | readBuffer[1];
    return (float)raw_co2;  // Example scaling — use sensor datasheet for accuracy
}


void BSP_start_inlet_fan(void) {
    GPIOPinWrite(FAN_GPIO_PORT, INLET_FAN_PIN, INLET_FAN_PIN);
}


void BSP_stop_inlet_fan(void) {
    GPIOPinWrite(FAN_GPIO_PORT, INLET_FAN_PIN, 0);
}


void BSP_start_outlet_fan(void) {
    GPIOPinWrite(FAN_GPIO_PORT, OUTLET_FAN_PIN, OUTLET_FAN_PIN);
}


void BSP_stop_outlet_fan(void) {
    GPIOPinWrite(FAN_GPIO_PORT, OUTLET_FAN_PIN, 0);
}


void BSP_stop_fans(void) {
    BSP_stop_inlet_fan();
    BSP_stop_outlet_fan();
}


void BSP_onStartup(void) {
    // Set up the SysTick timer to fire at 1ms intervals.
    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickIntEnable();
    SysTickEnable();

    // Enable interrupts to the processor.
    IntMasterEnable();
}

void BSP_onIdle(void) {
    // Idle loop code
}

// QF callbacks
static void l_SysTick_Handler(void);

void QF_onStartup(void) {
    BSP_onStartup();
}

void SysTick_Handler(void) {
    QF_TICK_X(0, &l_SysTick_Handler);
}

static void l_SysTick_Handler(void) {
    //
}

void BSP_uart_write(const char *str) {
    while (*str) {
        UARTCharPut(UART0_BASE, *str++);
    }
}

static void BSP_delay_us(uint32_t us) {
    SysCtlDelay(us * (SysCtlClockGet() / 3 / 1000000));
}

static int BSP_readDHT11(uint8_t *data) {
    uint8_t i, j, temp;
    uint8_t retry = 0;

    // Start signal
    GPIOPinTypeGPIOOutput(DHT11_PORT, DHT11_PIN);
    GPIOPinWrite(DHT11_PORT, DHT11_PIN, 0);
    BSP_delay_us(18000);
    GPIOPinWrite(DHT11_PORT, DHT11_PIN, DHT11_PIN);
    BSP_delay_us(40);
    GPIOPinTypeGPIOInput(DHT11_PORT, DHT11_PIN);

    // Wait for response
    retry = 0;
    while (GPIOPinRead(DHT11_PORT, DHT11_PIN) && (retry < 100)) {
        retry++;
        BSP_delay_us(1);
    }
    if (retry >= 100) return -1;

    retry = 0;
    while (!GPIOPinRead(DHT11_PORT, DHT11_PIN) && (retry < 100)) {
        retry++;
        BSP_delay_us(1);
    }
    if (retry >= 100) return -1;

    // Read data
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {
            retry = 0;
            while (!GPIOPinRead(DHT11_PORT, DHT11_PIN) && (retry < 100)) {
                retry++;
                BSP_delay_us(1);
            }
            if (retry >= 100) return -1;

            BSP_delay_us(30);

            if (GPIOPinRead(DHT11_PORT, DHT11_PIN)) {
                temp = 1;
            } else {
                temp = 0;
            }

            data[j] = (data[j] << 1) | temp;

            retry = 0;
            while (GPIOPinRead(DHT11_PORT, DHT11_PIN) && (retry < 100)) {
                retry++;
                BSP_delay_us(1);
            }
            if (retry >= 100) return -1;
        }
    }

    // Checksum
    if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return -1;
    }

    return 0; // Success
}

float BSP_readTemperature(void) {
    uint8_t data[5] = {0};
    if (BSP_readDHT11(data) == 0) {
        return (float)data[2];
    }
    return -1.0f; // Error
}

float BSP_readHumidity(void) {
    uint8_t data[5] = {0};
    if (BSP_readDHT11(data) == 0) {
        return (float)data[0];
    }
    return -1.0f; // Error
}

void QF_onIdle(void) {
    BSP_onIdle();
}

void QK_onIdle(void) {
    QF_onIdle(); // For QK kernel
}

// Q_onAssert is called when an assertion fails.
void Q_onAssert(char const * const module, int_t const loc) {
    (void)module;
    (void)loc;
    while (1) {}
}
