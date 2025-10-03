#pragma once

class CIP_Sobel
{
public:
    UCHAR** m_pucSobelFilteringImgbuf; // 소벨 필터링된 이미지 저장 버퍼

public:
    CIP_Sobel(void);
    ~CIP_Sobel(void);

    // 소벨 필터링
    void SobelFiltering(UCHAR** imgbuf, int height, int width);

    // 이미지 패딩
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // 메모리 할당/해제
    UCHAR** memory_alloc2D(int height, int width);
    int memory_free2D(UCHAR** ppMemAllocated);
};