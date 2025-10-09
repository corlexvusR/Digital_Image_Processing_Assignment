#pragma once

class CIP_LoG
{
public:
    UCHAR** m_pucLoGFilteringImgbuf;  // LoG ���͸� ���
    int** m_pnLoGResultBuf;            // �Ǽ� ��� �����

public:
    CIP_LoG(void);
    ~CIP_LoG(void);

    // LoG ���͸�
    void LoGFiltering(UCHAR** imgbuf, int height, int width, double sigma);

    // Zero Crossing ����
    void ZeroCrossing(int height, int width);

    // ����þ� ���� ����
    double** GenerateGaussianKernel(int size, double sigma);

    // ���ö�þ� ����
    int** ApplyLaplacian(UCHAR** imgbuf, int height, int width);

    // �е�
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // �޸� �Ҵ�/����
    UCHAR** memory_alloc2D_UCHAR(int height, int width);
    int** memory_alloc2D_INT(int height, int width);
    double** memory_alloc2D_DOUBLE(int height, int width);
    void memory_free2D_UCHAR(UCHAR** ptr);
    void memory_free2D_INT(int** ptr);
    void memory_free2D_DOUBLE(double** ptr);
};