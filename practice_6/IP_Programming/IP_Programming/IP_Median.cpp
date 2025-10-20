#include "pch.h"
#include "IP_Median.h"
#include <algorithm>

CIP_Median::CIP_Median()
    : m_pucMedianFilteringImgbuf(NULL)
{
}

CIP_Median::~CIP_Median()
{
    if (m_pucMedianFilteringImgbuf)
        memory_free2D(m_pucMedianFilteringImgbuf);
}

UCHAR** CIP_Median::memory_alloc2D(int height, int width)
{
    UCHAR** ptr = new UCHAR * [height];
    for (int i = 0; i < height; i++)
        ptr[i] = new UCHAR[width];
    return ptr;
}

void CIP_Median::memory_free2D(UCHAR** ptr)
{
    if (ptr)
    {
        delete[] ptr[0];
        delete[] ptr;
    }
}

UCHAR** CIP_Median::MakePaddingImage(UCHAR** imgbuf, int Mask_Size, int height, int width)
{
    int Pad_Size = Mask_Size >> 1;
    UCHAR** Padding_Imgbuf = memory_alloc2D(height + (Pad_Size * 2), width + (Pad_Size * 2));

    // ���ʰ� �Ʒ���
    for (int i = 0; i < Pad_Size; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Padding_Imgbuf[i][j + Pad_Size] = imgbuf[0][j];
            Padding_Imgbuf[height + Pad_Size + i][j + Pad_Size] = imgbuf[height - 1][j];
        }
    }

    // ���ʰ� ������
    for (int j = 0; j < Pad_Size; j++)
    {
        for (int i = 0; i < height; i++)
        {
            Padding_Imgbuf[i + Pad_Size][j] = imgbuf[i][0];
            Padding_Imgbuf[i + Pad_Size][width + Pad_Size + j] = imgbuf[i][width - 1];
        }
    }

    // ���
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            Padding_Imgbuf[i + Pad_Size][j + Pad_Size] = imgbuf[i][j];

    // �𼭸�
    for (int i = 0; i < Pad_Size; i++)
    {
        for (int j = 0; j < Pad_Size; j++)
        {
            Padding_Imgbuf[i][j] = imgbuf[0][0];
            Padding_Imgbuf[height + Pad_Size + i][j] = imgbuf[height - 1][0];
            Padding_Imgbuf[i][width + Pad_Size + j] = imgbuf[0][width - 1];
            Padding_Imgbuf[height + Pad_Size + i][width + Pad_Size + j] = imgbuf[height - 1][width - 1];
        }
    }

    return Padding_Imgbuf;
}

// �߰��� ã�� �Լ�
UCHAR CIP_Median::FindMedian(UCHAR* values, int count)
{
    // ������ ���� �ӽ� �迭
    UCHAR* temp = new UCHAR[count];
    for (int i = 0; i < count; i++)
        temp[i] = values[i];

    // ���� ���� (������ ����)
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (temp[j] > temp[j + 1])
            {
                UCHAR t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    UCHAR median = temp[count / 2];
    delete[] temp;

    return median;
}

// Median ���͸�
void CIP_Median::MedianFiltering(UCHAR** imgbuf, int height, int width, int window_size)
{
    // ������ ũ��� Ȧ������ ��
    if (window_size % 2 == 0)
        window_size++;

    // �е�
    UCHAR** padded = MakePaddingImage(imgbuf, window_size, height, width);

    // ��� ���� �Ҵ�
    m_pucMedianFilteringImgbuf = memory_alloc2D(height, width);

    int pad = window_size / 2;
    int window_count = window_size * window_size;
    UCHAR* window_values = new UCHAR[window_count];

    // �� �ȼ��� ���� median ���� ����
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int idx = 0;

            // ������ �� ��� �� ����
            for (int m = 0; m < window_size; m++)
            {
                for (int n = 0; n < window_size; n++)
                {
                    window_values[idx++] = padded[i + m][j + n];
                }
            }

            // �߰��� ã��
            m_pucMedianFilteringImgbuf[i][j] = FindMedian(window_values, window_count);
        }
    }

    delete[] window_values;
    memory_free2D(padded);
}