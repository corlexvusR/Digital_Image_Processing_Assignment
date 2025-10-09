#include "pch.h"
#include "IP_LoG.h"
#include <cmath>

#define PI 3.14159265358979323846

CIP_LoG::CIP_LoG()
    : m_pucLoGFilteringImgbuf(NULL), m_pnLoGResultBuf(NULL)
{
}

CIP_LoG::~CIP_LoG()
{
    if (m_pucLoGFilteringImgbuf)
        memory_free2D_UCHAR(m_pucLoGFilteringImgbuf);
    if (m_pnLoGResultBuf)
        memory_free2D_INT(m_pnLoGResultBuf);
}

// UCHAR 메모리 할당
UCHAR** CIP_LoG::memory_alloc2D_UCHAR(int height, int width)
{
    UCHAR** ptr = new UCHAR * [height];
    for (int i = 0; i < height; i++)
        ptr[i] = new UCHAR[width];
    return ptr;
}

// INT 메모리 할당
int** CIP_LoG::memory_alloc2D_INT(int height, int width)
{
    int** ptr = new int* [height];
    for (int i = 0; i < height; i++)
    {
        ptr[i] = new int[width];
        for (int j = 0; j < width; j++)
            ptr[i][j] = 0;
    }
    return ptr;
}

// DOUBLE 메모리 할당
double** CIP_LoG::memory_alloc2D_DOUBLE(int height, int width)
{
    double** ptr = new double* [height];
    for (int i = 0; i < height; i++)
        ptr[i] = new double[width];
    return ptr;
}

// 메모리 해제 함수들
void CIP_LoG::memory_free2D_UCHAR(UCHAR** ptr)
{
    if (ptr)
    {
        delete[] ptr[0];
        delete[] ptr;
    }
}

void CIP_LoG::memory_free2D_INT(int** ptr)
{
    if (ptr)
    {
        delete[] ptr[0];
        delete[] ptr;
    }
}

void CIP_LoG::memory_free2D_DOUBLE(double** ptr)
{
    if (ptr)
    {
        delete[] ptr[0];
        delete[] ptr;
    }
}

// 패딩 함수
UCHAR** CIP_LoG::MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width)
{
    int Pad_Size = Mask_Size >> 1;
    UCHAR** Padding_Imgbuf = memory_alloc2D_UCHAR(height + (Pad_Size * 2),
        width + (Pad_Size * 2));

    // 위쪽과 아래쪽
    for (int i = 0; i < Pad_Size; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Padding_Imgbuf[i][j + Pad_Size] = imgbuf[0][j];
            Padding_Imgbuf[height + Pad_Size + i][j + Pad_Size] = imgbuf[height - 1][j];
        }
    }

    // 왼쪽과 오른쪽
    for (int j = 0; j < Pad_Size; j++)
    {
        for (int i = 0; i < height; i++)
        {
            Padding_Imgbuf[i + Pad_Size][j] = imgbuf[i][0];
            Padding_Imgbuf[i + Pad_Size][width + Pad_Size + j] = imgbuf[i][width - 1];
        }
    }

    // 가운데
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            Padding_Imgbuf[i + Pad_Size][j + Pad_Size] = imgbuf[i][j];

    // 모서리
    for (int i = 0; i < Pad_Size; i++)
    {
        for (int j = 0; j < Pad_Size; j++)
        {
            Padding_Imgbuf[i][j] = imgbuf[0][0];
            Padding_Imgbuf[height + Pad_Size + i][j] = imgbuf[height - 1][0];
            Padding_Imgbuf[i][width + Pad_Size + j] = imgbuf[0][width - 1];
            Padding_Imgbuf[height + Pad_Size + i][width + Pad_Size + j] =
                imgbuf[height - 1][width - 1];
        }
    }

    return Padding_Imgbuf;
}

// 가우시안 커널 생성
double** CIP_LoG::GenerateGaussianKernel(int size, double sigma)
{
    double** kernel = memory_alloc2D_DOUBLE(size, size);
    double sum = 0.0;
    int center = size / 2;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int x = i - center;
            int y = j - center;
            kernel[i][j] = exp(-(x * x + y * y) / (2.0 * sigma * sigma)) /
                (2.0 * PI * sigma * sigma);
            sum += kernel[i][j];
        }
    }

    // 정규화
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i][j] /= sum;

    return kernel;
}

// LoG 필터링
void CIP_LoG::LoGFiltering(UCHAR** imgbuf, int height, int width, double sigma)
{
    // 1. 가우시안 필터링
    int kernel_size = (int)(6 * sigma) | 1;  // 홀수로 만들기
    if (kernel_size < 3) kernel_size = 3;

    double** gaussian_kernel = GenerateGaussianKernel(kernel_size, sigma);
    UCHAR** padded = MakePaddingImage(imgbuf, kernel_size, height, width);
    UCHAR** gaussian_result = memory_alloc2D_UCHAR(height, width);

    int pad = kernel_size / 2;

    // 가우시안 필터 적용
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            double sum = 0.0;
            for (int m = 0; m < kernel_size; m++)
            {
                for (int n = 0; n < kernel_size; n++)
                {
                    sum += padded[i + m][j + n] * gaussian_kernel[m][n];
                }
            }
            gaussian_result[i][j] = (UCHAR)sum;
        }
    }

    memory_free2D_UCHAR(padded);
    memory_free2D_DOUBLE(gaussian_kernel);

    // 2. 라플라시안 필터 적용
    int laplacian[3][3] = {
        { 0, -1,  0 },
        {-1,  4, -1 },
        { 0, -1,  0 }
    };

    padded = MakePaddingImage(gaussian_result, 3, height, width);
    m_pnLoGResultBuf = memory_alloc2D_INT(height, width);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sum = 0;
            for (int m = 0; m < 3; m++)
            {
                for (int n = 0; n < 3; n++)
                {
                    sum += padded[i + m][j + n] * laplacian[m][n];
                }
            }
            m_pnLoGResultBuf[i][j] = sum;
        }
    }

    memory_free2D_UCHAR(padded);
    memory_free2D_UCHAR(gaussian_result);

    // 3. Zero Crossing 검출
    ZeroCrossing(height, width);
}

// Zero Crossing 검출
void CIP_LoG::ZeroCrossing(int height, int width)
{
    m_pucLoGFilteringImgbuf = memory_alloc2D_UCHAR(height, width);

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            int center = m_pnLoGResultBuf[i][j];
            bool zero_cross = false;

            // 4방향 검사
            if ((center > 0 && m_pnLoGResultBuf[i - 1][j] < 0) ||
                (center < 0 && m_pnLoGResultBuf[i - 1][j] > 0))
                zero_cross = true;

            if ((center > 0 && m_pnLoGResultBuf[i + 1][j] < 0) ||
                (center < 0 && m_pnLoGResultBuf[i + 1][j] > 0))
                zero_cross = true;

            if ((center > 0 && m_pnLoGResultBuf[i][j - 1] < 0) ||
                (center < 0 && m_pnLoGResultBuf[i][j - 1] > 0))
                zero_cross = true;

            if ((center > 0 && m_pnLoGResultBuf[i][j + 1] < 0) ||
                (center < 0 && m_pnLoGResultBuf[i][j + 1] > 0))
                zero_cross = true;

            m_pucLoGFilteringImgbuf[i][j] = zero_cross ? 255 : 0;
        }
    }

    // 경계 처리
    for (int i = 0; i < height; i++)
    {
        m_pucLoGFilteringImgbuf[i][0] = 0;
        m_pucLoGFilteringImgbuf[i][width - 1] = 0;
    }
    for (int j = 0; j < width; j++)
    {
        m_pucLoGFilteringImgbuf[0][j] = 0;
        m_pucLoGFilteringImgbuf[height - 1][j] = 0;
    }
}