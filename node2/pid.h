#ifndef PID_H
#define PID_H

/* Generic PID controller (manual section 3.8.3, equation 2):
 *
 *   u = Kp*e + Ki*sum(e*dt) + Kd*(e - e_prev)/dt,   e = setpoint - measured
 *
 * The error is first scaled from input units to output units:
 *   e = (setpoint - measured) * (outMax - outMin) / |inMax - inMin|
 * so with kp = 1, an error of the whole input range gives the whole
 * output range, whatever units the input is in.
 *
 * One Pid struct per thing you control. Set kd = 0 for a PI controller. */
typedef struct
{
    float kp, ki, kd;       /* gains */
    float outMin, outMax;   /* output is clamped to this range */
    float scale;            /* output range / input range */
    float integralLimit;    /* anti-windup: |integral| is kept below this (0 = no limit) */

    float integral;         /* sum of e*dt so far */
    float prevError;        /* error from the last update, for the D term */
    int   hasPrev;          /* 0 until the first update (avoids a D-term kick) */
} Pid;

void  pidInit(Pid *pid, float kp, float ki, float kd,
              float inMin, float inMax,
              float outMin, float outMax, float integralLimit);
void  pidReset(Pid *pid);   /* clear integral and history, keep the gains */

/* Call at a steady rate. setpoint and measured are in input units.
 * dt = seconds since the last call.
 * Returns the controller output, clamped to [outMin, outMax]. */
float pidUpdate(Pid *pid, float setpoint, float measured, float dt);

#endif // PID_H
