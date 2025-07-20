// control_utils.h
#ifndef CONTROL_UTILS_H
#define CONTROL_UTILS_H

typedef struct {
    float kp;
    float ki;
    float target;
    float integral;
} PIController;

float apply_PI_control(PIController *ctrl, float current_value);

#endif


