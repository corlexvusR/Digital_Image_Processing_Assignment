#pragma once

class CIP_LoG
{
public:
    UCHAR** m_pucLoGFilteringImgbuf;  // LoG 필터링 결과
    int** m_pnLoGResultBuf;            // 실수 결과 저장용

public:
    CIP_LoG(void);
    ~CIP_LoG(void);

    // LoG 필터링
    void LoGFiltering(UCHAR** imgbuf, int height, int width, double sigma);

    // Zero Crossing 검출
    void ZeroCrossing(int height, int width);

    // 가우시안 필터 생성
    double** GenerateGaussianKernel(int size, double sigma);

    // 라플라시안 연산
    int** ApplyLaplacian(UCHAR** imgbuf, int height, int width);

    // 패딩
    UCHAR** MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width);

    // 메모리 할당/해제
    UCHAR** memory_alloc2D_UCHAR(int height, int width);
    int** memory_alloc2D_INT(int height, int width);
    double** memory_alloc2D_DOUBLE(int height, int width);
    void memory_free2D_UCHAR(UCHAR** ptr);
    void memory_free2D_INT(int** ptr);
    void memory_free2D_DOUBLE(double** ptr);
};