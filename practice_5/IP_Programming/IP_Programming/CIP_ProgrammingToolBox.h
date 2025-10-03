#pragma once
#include "CIP_IO.h"
#include "CIP_Sobel.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_Sobel sobel;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);
};