#pragma once
#include "CIP_IO.h"
#include "CIP_Histogram.h"

class CIP_ProgrammingToolBox
{
public:
    CIP_IO io;
    CIP_Histogram histogram;

    // 히스토그램 매칭을 위한 참조 영상
    UCHAR** m_ReferenceImage;
    int m_RefWidth;
    int m_RefHeight;

public:
    CIP_ProgrammingToolBox(void);
    ~CIP_ProgrammingToolBox(void);

    void SetReferenceImage(UCHAR** refImg, int width, int height);
};