#pragma once

class CIP_Median
{
public:
    UCHAR** m_pucMedianFilteringImgbuf;

public:
    CIP_Median(void);
    ~CIP_Median(void);

    // Median 필터링
    void MedianFiltering(UCHAR** imgbuf, int height, int width, int window_size);

    // 중간값 찾기
    UCHAR FindMedian(UCHAR* values, int count);

    // 패딩
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // 메모리 할당/해제
    UCHAR** memory_alloc2D(int height, int width);
    void memory_free2D(UCHAR** ptr);
};
