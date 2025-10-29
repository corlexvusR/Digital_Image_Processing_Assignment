// IP_Canny.h
#pragma once
#include <math.h>
#include <queue>

class CIP_Canny
{
public:
    CIP_Canny();
    ~CIP_Canny();

    // 캐니 경계선 검출 메인 함수
    void CannyEdgeDetection(UCHAR** imgbuf, int height, int width, int low_threshold = 20, int high_threshold = 50);

    // 결과 이미지 버퍼
    UCHAR** m_pucCannyEdgeImgBuf;

private:
    int m_nWidth;
    int m_nHeight;

    // 중간 결과 저장용 버퍼
    UCHAR** m_pucBlurredImg;      // 블러링 결과
    int** m_pnGx;                  // x방향 그래디언트
    int** m_pnGy;                  // y방향 그래디언트
    double** m_pdMagnitude;        // 그래디언트 크기
    double** m_pdDirection;        // 그래디언트 방향
    UCHAR** m_pucSuppressed;       // 비최대 억제 결과

    // 헬퍼 함수들
    void GaussianBlur(UCHAR** input, UCHAR** output, int height, int width);
    void ComputeGradient(UCHAR** input, int height, int width);
    void NonMaximumSuppression(int height, int width);
    void DoubleThreshold(int height, int width, int low, int high);
    void EdgeTracking(int height, int width);

    // 메모리 관리 함수
    UCHAR** memory_alloc2D_uchar(int height, int width);
    int** memory_alloc2D_int(int height, int width);
    double** memory_alloc2D_double(int height, int width);
    void memory_free2D(UCHAR** arr, int height);
    void memory_free2D(int** arr, int height);
    void memory_free2D(double** arr, int height);
};