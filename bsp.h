#ifndef BSP_H
#define BSP_H

#include "qpc.h"

// BSP function prototypes
void BSP_init(void);

void BSP_onStartup(void);

void BSP_onIdle(void);

void BSP_updateLCD1(bool show_temp, float temp, bool show_humidity, float humidity);

void BSP_updateLCD2(bool show_co2, float co2,
                    bool show_o2, float o2,
                    bool show_time, const char *time_str);

char *BSP_get_time_string(void);

float BSP_readTemperature(void);

float BSP_readHumidity(void);

void BSP_enableUltrasonic(void);

void BSP_disableUltrasonic(void);

float BSP_read_co2_sensor(void);

void BSP_start_inlet_fan(void);

void BSP_stop_inlet_fan(void);

void BSP_start_outlet_fan(void);

void BSP_stop_outlet_fan(void);

void BSP_stop_fans(void);

void BSP_uart_write(const char *str);

#define DHT11_PORT      GPIO_PORTK_BASE
#define DHT11_PIN       GPIO_PIN_4

#endif // BSP_H
