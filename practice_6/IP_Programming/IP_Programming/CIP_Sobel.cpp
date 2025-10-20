#include "pch.h"
#include "CIP_Sobel.h"
#include <cmath>

// ������
CIP_Sobel::CIP_Sobel()
    : m_pucSobelFilteringImgbuf(NULL)
{
}

// �Ҹ���
CIP_Sobel::~CIP_Sobel()
{
    if (m_pucSobelFilteringImgbuf)
    {
        memory_free2D(m_pucSobelFilteringImgbuf);
    }
}

// �޸� �Ҵ� �Լ�
UCHAR** CIP_Sobel::memory_alloc2D(int height, int width)
{
    UCHAR** ppMemAllocated = new UCHAR * [height];

    for (int i = 0; i < height; i++)
    {
        ppMemAllocated[i] = new UCHAR[width];
    }

    return ppMemAllocated;
}

// �޸� ���� �Լ�
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

// �е� �Լ�
UCHAR** CIP_Sobel::MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width)
{
    int Pad_Size = Mask_Size >> 1; // Mask_Size / 2
    int i, j;

    UCHAR** Padding_Imgbuf = memory_alloc2D(height + (Pad_Size * 2),
        width + (Pad_Size * 2));

    // ���ʰ� �Ʒ���
    for (i = 0; i < Pad_Size; i++)
    {
        for (j = 0; j < width; j++)
        {
            // ����
            Padding_Imgbuf[i][j + Pad_Size] = imgbuf[0][j];
            // �Ʒ���
            Padding_Imgbuf[height + Pad_Size + i][j + Pad_Size] = imgbuf[height - 1][j];
        }
    }

    // ���ʰ� ������
    for (j = 0; j < Pad_Size; j++)
    {
        for (i = 0; i < height; i++)
        {
            // ����
            Padding_Imgbuf[i + Pad_Size][j] = imgbuf[i][0];
            // ������
            Padding_Imgbuf[i + Pad_Size][width + Pad_Size + j] = imgbuf[i][width - 1];
        }
    }

    // ���
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            Padding_Imgbuf[i + Pad_Size][j + Pad_Size] = imgbuf[i][j];
        }
    }

    // �𼭸�
    for (i = 0; i < Pad_Size; i++)
    {
        for (j = 0; j < Pad_Size; j++)
        {
            // ���� ���
            Padding_Imgbuf[i][j] = imgbuf[0][0];
            // ���� �ϴ�
            Padding_Imgbuf[height + Pad_Size + i][j] = imgbuf[height - 1][0];
            // ���� ���
            Padding_Imgbuf[i][width + Pad_Size + j] = imgbuf[0][width - 1];
            // ���� �ϴ�
            Padding_Imgbuf[height + Pad_Size + i][width + Pad_Size + j] =
                imgbuf[height - 1][width - 1];
        }
    }

    return Padding_Imgbuf;
}

// �Һ� ���͸� �Լ�
void CIP_Sobel::SobelFiltering(UCHAR** imgbuf, int height, int width)
{
    int Mask_Size = 3;
    int ImgPixel_x = 0;
    int ImgPixel_y = 0;
    int ImgPixel = 0;
    int threshold = 150;

    // �Һ� ����ũ ����
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

    // ���� �̹��� �е�
    UCHAR** Padded_Imgbuf = MakePaddingImage(imgbuf, Mask_Size, height, width);

    // ���͸��� ����� �̹����� ����
    m_pucSobelFilteringImgbuf = memory_alloc2D(height, width);

    // �е��� �̹����� �̿��Ͽ� ���͸� ����
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
                    // ����ũ ���� ��� �ȼ��� �� ���
                    ImgPixel_x += Padded_Imgbuf[i + m - 1][j + n - 1] * sobel_operator_x[m][n];
                    ImgPixel_y += Padded_Imgbuf[i + m - 1][j + n - 1] * sobel_operator_y[m][n];
                }
            }

            // �׷����Ʈ ũ�� ��� �� �Ӱ谪 ����
            if (abs(ImgPixel_x) + abs(ImgPixel_y) > threshold)
                m_pucSobelFilteringImgbuf[i - 1][j - 1] = 255;
            else
                m_pucSobelFilteringImgbuf[i - 1][j - 1] = 0;
        }
    }

    // �е��� ���� �޸� ����
    memory_free2D(Padded_Imgbuf);
}