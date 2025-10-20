#pragma once

class CIP_Median
{
public:
    UCHAR** m_pucMedianFilteringImgbuf;

public:
    CIP_Median(void);
    ~CIP_Median(void);

    // Median ���͸�
    void MedianFiltering(UCHAR** imgbuf, int height, int width, int window_size);

    // �߰��� ã��
    UCHAR FindMedian(UCHAR* values, int count);

    // �е�
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // �޸� �Ҵ�/����
    UCHAR** memory_alloc2D(int height, int width);
    void memory_free2D(UCHAR** ptr);
};
