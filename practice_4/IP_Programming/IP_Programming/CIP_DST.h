#pragma once
#include <cmath>
#define PI 3.14159265
#define CLIP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))

class CIP_DST
{
public:
    double** m_pucForwardDSTbuf;    // DST 변환 결과 저장 버퍼
    double** m_pucTempDSTbuf;       // DST 임시 저장 버퍼
    double** m_pucInverseDSTbuf;    // IDST 변환 결과 저장 버퍼

    UCHAR** FDSTImgbuf;             // DST 이미지 저장 버퍼
    UCHAR** IDSTImgbuf;             // IDST 이미지 저장 버퍼

public:
    CIP_DST(void);
    ~CIP_DST(void);

    void DST_MakeBlock(int DST_blocksize, int select, UCHAR** imgbuf, int width, int height);
    void DST_ForwardDST(double** DST_Block, int DST_blocksize, int row, int col);
    void DST_InverseDST(double** DST_Block, int DST_blocksize, int row, int col);
    void DST_MakeFrequencytoGray(double** imgbuf, UCHAR** buf, int width, int height);

    double** memory_alloc2D_D(int width, int height);
    UCHAR** memory_alloc2D_UC(int width, int height);
};