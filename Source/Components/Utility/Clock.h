/*================================================================
Filename: Clock.h
Date: 2017.10.16
Created by AirGuanZ
================================================================*/
#pragma once

class Clock
{
public:
    Clock(void);

    void Restart(void);

    void Tick(void);

    float ElapsedTime(void) const;

    float TotalTime(void) const;

private:
    using InternalFormat = float;

    InternalFormat startTime_;
    InternalFormat lastTick_;
    InternalFormat elapsedTime_;
    InternalFormat ratio_;
};
