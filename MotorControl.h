#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H

#include "qpc.h"

/***********************************************************************************************************************
* @file motor_ctrl.h
*
* @brief This file contains the public interface for the MotorControl active object.
*
* The MotorControl active object is responsible for controlling the egg rack motor.
*
***********************************************************************************************************************/

#define MOTOR_PIN_1 0
#define MOTOR_PIN_2 1
#define MOTOR_PIN_3 2
#define MOTOR_PIN_4 3

#define ROTATION_TIME (45 * 60 * BSP_TICKS_PER_SEC) // 45 minutes

#endif // MOTOR_CTRL_H
