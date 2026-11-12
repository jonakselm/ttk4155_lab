#include "pid.h"

void pidInit(Pid *pid, float kp, float ki, float kd,
             float inMin, float inMax,
             float outMin, float outMax, float integralLimit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->outMin = outMin;
    pid->outMax = outMax;
    pid->integralLimit = integralLimit;

    float inRange = inMax - inMin;
    if (inRange < 0.0f)
        inRange = -inRange;
    pid->scale = (inRange > 0.0f) ? (outMax - outMin) / inRange : 1.0f;

    pidReset(pid);
}

void pidReset(Pid *pid)
{
    pid->integral  = 0.0f;
    pid->prevError = 0.0f;
    pid->hasPrev   = 0;
}

float pidUpdate(Pid *pid, float setpoint, float measured, float dt)
{
    if (dt <= 0.0f)
        return 0.0f;

    float e = (setpoint - measured) * pid->scale;

    pid->integral += e * dt;
    if (pid->integralLimit > 0.0f)
    {
        if (pid->integral >  pid->integralLimit) pid->integral =  pid->integralLimit;
        if (pid->integral < -pid->integralLimit) pid->integral = -pid->integralLimit;
    }

    float d = 0.0f;
    if (pid->hasPrev)
        d = (e - pid->prevError) / dt;
    pid->prevError = e;
    pid->hasPrev   = 1;

    float u = pid->kp * e + pid->ki * pid->integral + pid->kd * d;

    if (u > pid->outMax) u = pid->outMax;
    if (u < pid->outMin) u = pid->outMin;
    return u;
}
