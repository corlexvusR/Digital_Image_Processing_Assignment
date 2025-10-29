// IP_Hough.h
#pragma once

class CIP_Hough
{
public:
    // 직선 검출 이미지 (직선이 그려진 영상)
    UCHAR** m_pucLineDetectionImgBuf;

public:
    CIP_Hough();
    ~CIP_Hough();

    // 허프 변환 메인 함수
    void HoughTransform(UCHAR** imgbuf, UCHAR** edgeimgbuf, int height, int width);

    // 메모리 할당 함수
    UCHAR** memory_alloc2D(int height, int width);
};