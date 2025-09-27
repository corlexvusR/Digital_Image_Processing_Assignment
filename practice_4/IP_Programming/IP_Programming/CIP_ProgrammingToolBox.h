#pragma once
#include "CIP_IO.h"
#include "CIP_DCT.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_DCT dct;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);
};