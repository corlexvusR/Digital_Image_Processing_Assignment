#pragma once
#include <cmath>
#define PI 3.14159265
#define CLIP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))

class CIP_DST
{
public:
    double** m_pucForwardDSTbuf;    // DST ��ȯ ��� ���� ����
    double** m_pucTempDSTbuf;       // DST �ӽ� ���� ����
    double** m_pucInverseDSTbuf;    // IDST ��ȯ ��� ���� ����

    UCHAR** FDSTImgbuf;             // DST �̹��� ���� ����
    UCHAR** IDSTImgbuf;             // IDST �̹��� ���� ����

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