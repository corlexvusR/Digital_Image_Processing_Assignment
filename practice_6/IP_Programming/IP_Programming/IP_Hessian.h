// IP_Hessian.h
#pragma once
#include <math.h>
#include <vector>

class CIP_Hessian
{
public:
    CIP_Hessian();
    ~CIP_Hessian();

    // 헤시안 코너 검출 메인 함수
    void HessianCornerDetection(UCHAR** imgbuf, int height, int width,
        double threshold_ratio = 0.01);

    // 결과 이미지 (코너가 표시된 영상)
    UCHAR** m_pucCornerImgBuf;

    // 검출된 코너 정보
    struct CornerPoint {
        int x;
        int y;
        double response;
    };
    std::vector<CornerPoint> m_detectedCorners;

private:
    int m_nWidth;
    int m_nHeight;

    // 2차 미분 결과
    double** m_pdIxx;  // x방향 2차 미분
    double** m_pdIyy;  // y방향 2차 미분
    double** m_pdIxy;  // 교차 미분
    double** m_pdDet;  // 행렬식 값

    // 헬퍼 함수
    void ComputeSecondDerivatives(UCHAR** input, int height, int width);
    void ComputeDeterminant(int height, int width);
    void NonMaximumSuppression(int height, int width, double threshold);
    void DrawCorners(UCHAR** input, int height, int width);

    // 메모리 관리
    UCHAR** memory_alloc2D_uchar(int height, int width);
    double** memory_alloc2D_double(int height, int width);
    void memory_free2D(UCHAR** arr, int height);
    void memory_free2D(double** arr, int height);
};