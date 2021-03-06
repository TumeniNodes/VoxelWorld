/*================================================================
Filename: Application.h
Date: 2018.2.27
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"

class Application
{
public:
    void Run(void);

private:
    bool Initialize(std::string &errMsg);

private:
    AppConf appConf_;
};
