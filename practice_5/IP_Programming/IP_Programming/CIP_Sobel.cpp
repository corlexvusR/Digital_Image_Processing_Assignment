#include "pch.h"
#include "CIP_Sobel.h"
#include <cmath>

// 생성자
CIP_Sobel::CIP_Sobel()
    : m_pucSobelFilteringImgbuf(NULL)
{
}

// 소멸자
CIP_Sobel::~CIP_Sobel()
{
    if (m_pucSobelFilteringImgbuf)
    {
        memory_free2D(m_pucSobelFilteringImgbuf);
    }
}

// 메모리 할당 함수
UCHAR** CIP_Sobel::memory_alloc2D(int height, int width)
{
    UCHAR** ppMemAllocated = new UCHAR * [height];

    for (int i = 0; i < height; i++)
    {
        ppMemAllocated[i] = new UCHAR[width];
    }

    return ppMemAllocated;
}

// 메모리 해제 함수
int CIP_Sobel::memory_free2D(UCHAR** ppMemAllocated)
{
    if (ppMemAllocated != NULL)
    {
        delete[] ppMemAllocated[0];
        delete[] ppMemAllocated;
        ppMemAllocated = NULL;
    }
    return 0;
}

// 패딩 함수
UCHAR** CIP_Sobel::MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width)
{
    int Pad_Size = Mask_Size >> 1; // Mask_Size / 2
    int i, j;

    UCHAR** Padding_Imgbuf = memory_alloc2D(height + (Pad_Size * 2),
        width + (Pad_Size * 2));

    // 위쪽과 아래쪽
    for (i = 0; i < Pad_Size; i++)
    {
        for (j = 0; j < width; j++)
        {
            // 위쪽
            Padding_Imgbuf[i][j + Pad_Size] = imgbuf[0][j];
            // 아래쪽
            Padding_Imgbuf[height + Pad_Size + i][j + Pad_Size] = imgbuf[height - 1][j];
        }
    }

    // 왼쪽과 오른쪽
    for (j = 0; j < Pad_Size; j++)
    {
        for (i = 0; i < height; i++)
        {
            // 왼쪽
            Padding_Imgbuf[i + Pad_Size][j] = imgbuf[i][0];
            // 오른쪽
            Padding_Imgbuf[i + Pad_Size][width + Pad_Size + j] = imgbuf[i][width - 1];
        }
    }

    // 가운데
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            Padding_Imgbuf[i + Pad_Size][j + Pad_Size] = imgbuf[i][j];
        }
    }

    // 모서리
    for (i = 0; i < Pad_Size; i++)
    {
        for (j = 0; j < Pad_Size; j++)
        {
            // 좌측 상단
            Padding_Imgbuf[i][j] = imgbuf[0][0];
            // 좌측 하단
            Padding_Imgbuf[height + Pad_Size + i][j] = imgbuf[height - 1][0];
            // 우측 상단
            Padding_Imgbuf[i][width + Pad_Size + j] = imgbuf[0][width - 1];
            // 우측 하단
            Padding_Imgbuf[height + Pad_Size + i][width + Pad_Size + j] =
                imgbuf[height - 1][width - 1];
        }
    }

    return Padding_Imgbuf;
}

// 소벨 필터링 함수
void CIP_Sobel::SobelFiltering(UCHAR** imgbuf, int height, int width)
{
    int Mask_Size = 3;
    int ImgPixel_x = 0;
    int ImgPixel_y = 0;
    int ImgPixel = 0;
    int threshold = 150;

    // 소벨 마스크 정의
    CHAR sobel_operator_y[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    CHAR sobel_operator_x[3][3] = {
        { 1,  0,  -1 },
        { 2,  0,  -2 },
        { 1,  0,  -1 }
    };

    // 원본 이미지 패딩
    UCHAR** Padded_Imgbuf = MakePaddingImage(imgbuf, Mask_Size, height, width);

    // 필터링이 수행된 이미지를 저장
    m_pucSobelFilteringImgbuf = memory_alloc2D(height, width);

    // 패딩된 이미지를 이용하여 필터링 수행
    for (int i = 1; i < height; i++)
    {
        for (int j = 1; j < width; j++)
        {
            ImgPixel_x = 0;
            ImgPixel_y = 0;

            for (int m = 0; m < Mask_Size; m++)
            {
                for (int n = 0; n < Mask_Size; n++)
                {
                    // 마스크 내의 모든 픽셀의 합 계산
                    ImgPixel_x += Padded_Imgbuf[i + m - 1][j + n - 1] * sobel_operator_x[m][n];
                    ImgPixel_y += Padded_Imgbuf[i + m - 1][j + n - 1] * sobel_operator_y[m][n];
                }
            }

            // 그래디언트 크기 계산 및 임계값 적용
            if (abs(ImgPixel_x) + abs(ImgPixel_y) > threshold)
                m_pucSobelFilteringImgbuf[i - 1][j - 1] = 255;
            else
                m_pucSobelFilteringImgbuf[i - 1][j - 1] = 0;
        }
    }

    // 패딩된 영상 메모리 삭제
    memory_free2D(Padded_Imgbuf);
}