
#ifndef BSP_PWM_H
#define BSP_PWM_H

void BSP_initPWM(void);
void BSP_setHeaterPower(float duty);
void BSP_setHumidifierPower(float duty);
void BSP_setCoolerPower(float dutyPercent);


#endif // BSP_PWM_H


