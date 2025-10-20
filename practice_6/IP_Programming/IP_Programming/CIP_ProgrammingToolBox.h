#pragma once
#include "CIP_IO.h"
#include "CIP_Sobel.h"
#include "IP_LoG.h"
#include "IP_Median.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_Sobel sobel;
    CIP_LoG log;
    CIP_Median median;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);
};