#pragma once

class CIP_Sobel
{
public:
    UCHAR** m_pucSobelFilteringImgbuf; // �Һ� ���͸��� �̹��� ���� ����

public:
    CIP_Sobel(void);
    ~CIP_Sobel(void);

    // �Һ� ���͸�
    void SobelFiltering(UCHAR** imgbuf, int height, int width);

    // �̹��� �е�
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // �޸� �Ҵ�/����
    UCHAR** memory_alloc2D(int height, int width);
    int memory_free2D(UCHAR** ppMemAllocated);
};