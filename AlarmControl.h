#ifndef ALARM_CTRL_H
#define ALARM_CTRL_H

#include "qpc.h"

/***********************************************************************************************************************
* @file alarm_ctrl.h
*
* @brief This file contains the public interface for the AlarmControl active object.
*
* The AlarmControl active object is responsible for managing the alarms.
*
***********************************************************************************************************************/

#define ALARM_TIMEOUT (3 * 60 * BSP_TICKS_PER_SEC) // 3 minutes

#define TEMP_ALARM_THRESHOLD 1.5f
#define HUMIDITY_ALARM_THRESHOLD 10.0f
#define CO2_ALARM_THRESHOLD 1000.0f

#endif // ALARM_CTRL_H
