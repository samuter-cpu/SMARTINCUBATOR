float apply_PI_control(PIController *ctrl, float current_value);

float apply_PI_control(PIController *ctrl, float current_value) {
    float error = ctrl->target - current_value;

    // Full power outside threshold zone
    if (fabsf(error) > FULL_POWER_THRESHOLD) {
        return (error > 0.0f) ? PWM_MAX_DUTY : PWM_MIN_DUTY;
    }

    // PI calculation
    ctrl->integral += error;
    float raw_output = (ctrl->kp * error) + (ctrl->ki * ctrl->integral);

    // Scale raw output to duty cycle: 0–100%
    float duty = ((raw_output - RAW_OUTPUT_MIN) / (RAW_OUTPUT_MAX - RAW_OUTPUT_MIN)) * 100.0f;

    // Clamp final duty cycle
    if (duty < PWM_MIN_DUTY) duty = PWM_MIN_DUTY;
    if (duty > PWM_MAX_DUTY) duty = PWM_MAX_DUTY;

    return duty;
}