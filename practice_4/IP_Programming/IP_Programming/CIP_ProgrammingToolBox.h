#pragma once
#include "CIP_IO.h"
#include "CIP_DCT.h"
#include "CIP_DST.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_DCT dct;
    CIP_DST dst;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);
};