// IP_Canny.h
#pragma once
#include <math.h>
#include <queue>

class CIP_Canny
{
public:
    CIP_Canny();
    ~CIP_Canny();

    // ĳ�� ��輱 ���� ���� �Լ�
    void CannyEdgeDetection(UCHAR** imgbuf, int height, int width, int low_threshold = 20, int high_threshold = 50);

    // ��� �̹��� ����
    UCHAR** m_pucCannyEdgeImgBuf;

private:
    int m_nWidth;
    int m_nHeight;

    // �߰� ��� ����� ����
    UCHAR** m_pucBlurredImg;      // ���� ���
    int** m_pnGx;                  // x���� �׷����Ʈ
    int** m_pnGy;                  // y���� �׷����Ʈ
    double** m_pdMagnitude;        // �׷����Ʈ ũ��
    double** m_pdDirection;        // �׷����Ʈ ����
    UCHAR** m_pucSuppressed;       // ���ִ� ���� ���

    // ���� �Լ���
    void GaussianBlur(UCHAR** input, UCHAR** output, int height, int width);
    void ComputeGradient(UCHAR** input, int height, int width);
    void NonMaximumSuppression(int height, int width);
    void DoubleThreshold(int height, int width, int low, int high);
    void EdgeTracking(int height, int width);

    // �޸� ���� �Լ�
    UCHAR** memory_alloc2D_uchar(int height, int width);
    int** memory_alloc2D_int(int height, int width);
    double** memory_alloc2D_double(int height, int width);
    void memory_free2D(UCHAR** arr, int height);
    void memory_free2D(int** arr, int height);
    void memory_free2D(double** arr, int height);
};