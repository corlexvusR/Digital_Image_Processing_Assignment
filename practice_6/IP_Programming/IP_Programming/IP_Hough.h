// IP_Hough.h
#pragma once

class CIP_Hough
{
public:
    // ���� ���� �̹��� (������ �׷��� ����)
    UCHAR** m_pucLineDetectionImgBuf;

public:
    CIP_Hough();
    ~CIP_Hough();

    // ���� ��ȯ ���� �Լ�
    void HoughTransform(UCHAR** imgbuf, UCHAR** edgeimgbuf, int height, int width);

    // �޸� �Ҵ� �Լ�
    UCHAR** memory_alloc2D(int height, int width);
};