#include "pch.h"
#include "IP_Hough.h"
#include <math.h>

CIP_Hough::CIP_Hough()
    : m_pucLineDetectionImgBuf(NULL)
{
}

CIP_Hough::~CIP_Hough()
{
    if (m_pucLineDetectionImgBuf) {
        free(m_pucLineDetectionImgBuf[0]);
        free(m_pucLineDetectionImgBuf);
    }
}

UCHAR** CIP_Hough::memory_alloc2D(int height, int width)
{
    UCHAR** arr = (UCHAR**)malloc(sizeof(UCHAR*) * height);
    arr[0] = (UCHAR*)malloc(sizeof(UCHAR) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(UCHAR) * height * width);
    return arr;
}

void CIP_Hough::HoughTransform(UCHAR** imgbuf, UCHAR** edgeimgbuf, int height, int width)
{
    // ��輱 ������ ������ ����
    if (edgeimgbuf == NULL) {
        return;
    }

    const double PI = 3.14159265;

    // 1. �Ķ���� ���� ũ�� ���
    int range = (int)sqrt((double)(width * width) + (double)(height * height));
    int rhoSize = range * 2;   // �� ����: -range - +range
    int THETA = 180;           // ����: 0��- 180�� (1�� ����)
    int Threshold = 120;       // ���� ���� �Ӱ谪

    // 2. sin/cos ��� ���̺� ���� (���� ����ȭ)
    double LOOKUP_TABLE_SIN[180];
    double LOOKUP_TABLE_COS[180];

    for (int i = 0; i < THETA; i++) {
        double radian = i * PI / THETA;
        LOOKUP_TABLE_SIN[i] = sin(radian);
        LOOKUP_TABLE_COS[i] = cos(radian);
    }

    // 3. ��� �̹��� �޸� �Ҵ�
    m_pucLineDetectionImgBuf = memory_alloc2D(height, width);

    // 4. ���� �迭 (Accumulator) ���� �� �ʱ�ȭ
    unsigned int* pNvote = new unsigned int[THETA * rhoSize];
    memset(pNvote, 0, (THETA * rhoSize) * sizeof(unsigned int));

    // 5. ��ǥ �ܰ�: ��� ��� �ȼ��� ���� ��ǥ
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // ��� �ȼ��ΰ�?
            if (edgeimgbuf[i][j] >= 128) {
                // ��� ���� �迡 ����
                for (int m = 0; m < THETA; m++) {
                    // p ���: p = x��sin(��) + y��cos(��)
                    double p = (i * LOOKUP_TABLE_SIN[m]) + (j * LOOKUP_TABLE_COS[m]);

                    // p�� ��� �ε����� ��ȯ
                    p += range;

                    // ���� �迭�� ��ǥ
                    int rhoIndex = (int)(p + 0.5);
                    if (rhoIndex >= 0 && rhoIndex < rhoSize) {
                        pNvote[(m * rhoSize) + rhoIndex]++;
                    }
                }
            }
        }
    }

    // 6. ���� ���� �ܰ�
    for (int i = 0; i < THETA; i++) {
        for (int j = range; j < rhoSize; j++) {
            int nVote = pNvote[i * rhoSize + j];

            // �Ӱ谪���� ��ǥ�� ������ �������� �ν�
            if (nVote > Threshold) {
                bool isTrueLine = true;

                // 7. ���� �ִ� ���� (Non-maximum Suppression)
                // �ֺ� ��5 �������� �ִ����� Ȯ��
                for (int dAngle = -5; dAngle <= 5 && isTrueLine; dAngle++) {
                    int angle = i + dAngle;
                    if (angle < 0) continue;
                    if (angle >= THETA) break;

                    for (int dRho = -5; dRho <= 5 && isTrueLine; dRho++) {
                        int rho = j + dRho;
                        if (rho < 0) continue;
                        if (rho >= rhoSize) break;

                        if (pNvote[(angle * rhoSize) + rho] > nVote) {
                            isTrueLine = false;
                        }
                    }
                }

                // 8. ���� �׸���
                if (isTrueLine) {
                    int m, n;
                    double rho = j - range;  // ���� p������ ����

                    // ���� �������� ���� ���
                    for (n = 0; n < height; n++) {
                        // x = (p - y��sin(��)) / cos(��)
                        m = (int)((rho - n * LOOKUP_TABLE_SIN[i]) / LOOKUP_TABLE_COS[i]);

                        if (m < width && m >= 0) {
                            m_pucLineDetectionImgBuf[0][n * width + m] = 255;
                        }
                    }

                    // ���� �������� ���� ���
                    for (m = 0; m < width; m++) {
                        // y = (p - x��cos(��)) / sin(��)
                        n = (int)((rho - m * LOOKUP_TABLE_COS[i]) / LOOKUP_TABLE_SIN[i]);

                        if (n < height && n >= 0) {
                            m_pucLineDetectionImgBuf[0][n * width + m] = 255;
                        }
                    }
                }
            }
        }
    }
}