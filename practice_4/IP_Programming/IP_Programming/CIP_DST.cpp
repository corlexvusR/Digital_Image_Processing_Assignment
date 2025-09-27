#include "pch.h"
#include "CIP_DST.h"

#define PI 3.14159265
#define CLIP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))

CIP_DST::CIP_DST(void)
    : m_pucForwardDSTbuf(nullptr)
    , m_pucTempDSTbuf(nullptr)
    , m_pucInverseDSTbuf(nullptr)
    , FDSTImgbuf(nullptr)
    , IDSTImgbuf(nullptr)
{
}

CIP_DST::~CIP_DST(void)
{
    if (m_pucForwardDSTbuf) {
        free(m_pucForwardDSTbuf[0]);
        free(m_pucForwardDSTbuf);
        m_pucForwardDSTbuf = nullptr;
    }
    if (m_pucTempDSTbuf) {
        free(m_pucTempDSTbuf[0]);
        free(m_pucTempDSTbuf);
        m_pucTempDSTbuf = nullptr;
    }
    if (m_pucInverseDSTbuf) {
        free(m_pucInverseDSTbuf[0]);
        free(m_pucInverseDSTbuf);
        m_pucInverseDSTbuf = nullptr;
    }
    if (FDSTImgbuf) {
        free(FDSTImgbuf[0]);
        free(FDSTImgbuf);
        FDSTImgbuf = nullptr;
    }
    if (IDSTImgbuf) {
        free(IDSTImgbuf[0]);
        free(IDSTImgbuf);
        IDSTImgbuf = nullptr;
    }
}

void CIP_DST::DST_MakeBlock(int DST_blocksize, int select, UCHAR** imgbuf, int width, int height)
{
    if (select == 1) { // FDST
        m_pucForwardDSTbuf = memory_alloc2D_D(width, height);
        m_pucTempDSTbuf = memory_alloc2D_D(width, height);
        FDSTImgbuf = memory_alloc2D_UC(width, height);
    }
    if (select == 0) { // IDST
        m_pucInverseDSTbuf = memory_alloc2D_D(width, height);
        IDSTImgbuf = memory_alloc2D_UC(width, height);
    }

    double** DST_block = memory_alloc2D_D(DST_blocksize, DST_blocksize);

    for (int i = 0; i < height; i += DST_blocksize) {
        for (int j = 0; j < width; j += DST_blocksize) {
            for (int m = 0; m < DST_blocksize; m++) {
                for (int n = 0; n < DST_blocksize; n++) {
                    if (select == 1) DST_block[m][n] = (double)imgbuf[i + m][j + n];
                    if (select == 0) DST_block[m][n] = (double)m_pucTempDSTbuf[i + m][j + n];
                }
            }
            if (select == 1) DST_ForwardDST(DST_block, DST_blocksize, i, j);
            if (select == 0) DST_InverseDST(DST_block, DST_blocksize, i, j);
        }
    }
}

void CIP_DST::DST_ForwardDST(double** DST_Block, int DST_blocksize, int row, int col)
{
    double Sum;

    for (int u = 0; u < DST_blocksize; u++) {
        for (int v = 0; v < DST_blocksize; v++) {
            Sum = 0;

            for (int y = 0; y < DST_blocksize; y++) {
                for (int x = 0; x < DST_blocksize; x++) {
                    double sinVal1 = sin((y + 1) * (u + 1) * PI / (DST_blocksize + 1));
                    double sinVal2 = sin((x + 1) * (v + 1) * PI / (DST_blocksize + 1));
                    Sum += DST_Block[y][x] * sinVal1 * sinVal2;
                }
            }

            Sum *= sqrt(2.0 / (DST_blocksize + 1)) * sqrt(2.0 / (DST_blocksize + 1));

            m_pucTempDSTbuf[u + row][v + col] = Sum;
            m_pucForwardDSTbuf[u + row][v + col] = CLIP(Sum);
        }
    }
}

void CIP_DST::DST_InverseDST(double** DST_Block, int DST_blocksize, int row, int col)
{
    double Sum;

    for (int y = 0; y < DST_blocksize; y++) {
        for (int x = 0; x < DST_blocksize; x++) {
            Sum = 0;

            for (int u = 0; u < DST_blocksize; u++) {
                for (int v = 0; v < DST_blocksize; v++) {
                    double sinVal1 = sin((y + 1) * (u + 1) * PI / (DST_blocksize + 1));
                    double sinVal2 = sin((x + 1) * (v + 1) * PI / (DST_blocksize + 1));
                    Sum += DST_Block[u][v] * sinVal1 * sinVal2;
                }
            }

            Sum *= sqrt(2.0 / (DST_blocksize + 1)) * sqrt(2.0 / (DST_blocksize + 1));

            m_pucInverseDSTbuf[y + row][x + col] = CLIP(Sum);
        }
    }
}

// 메모리 할당 함수들 (DCT와 동일)
double** CIP_DST::memory_alloc2D_D(int width, int height) {
    double** ppMem2D = (double**)calloc(sizeof(double*), height);
    if (!ppMem2D) return nullptr;

    *ppMem2D = (double*)calloc(sizeof(double), height * width);
    if (!(*ppMem2D)) {
        free(ppMem2D);
        return nullptr;
    }

    for (int i = 1; i < height; i++) {
        ppMem2D[i] = ppMem2D[i - 1] + width;
    }

    return ppMem2D;
}

UCHAR** CIP_DST::memory_alloc2D_UC(int width, int height) {
    UCHAR** ppMem2D = (UCHAR**)calloc(sizeof(UCHAR*), height);
    if (!ppMem2D) return nullptr;

    *ppMem2D = (UCHAR*)calloc(sizeof(UCHAR), height * width);
    if (!(*ppMem2D)) {
        free(ppMem2D);
        return nullptr;
    }

    for (int i = 1; i < height; i++) {
        ppMem2D[i] = ppMem2D[i - 1] + width;
    }

    return ppMem2D;
}

void CIP_DST::DST_MakeFrequencytoGray(double** imgbuf, UCHAR** buf, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            buf[i][j] = (UCHAR)CLIP(imgbuf[i][j]);
        }
    }
}