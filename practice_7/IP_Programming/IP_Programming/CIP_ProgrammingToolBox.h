#pragma once
#include "CIP_IO.h"
#include "IP_Morphology.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_Morphology morphology;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);
};