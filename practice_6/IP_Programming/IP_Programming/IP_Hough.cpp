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
    // 경계선 영상이 없으면 종료
    if (edgeimgbuf == NULL) {
        return;
    }

    const double PI = 3.14159265;

    // 1. 파라미터 공간 크기 계산
    int range = (int)sqrt((double)(width * width) + (double)(height * height));
    int rhoSize = range * 2;   // ρ 범위: -range - +range
    int THETA = 180;           // 각도: 0°- 180° (1° 간격)
    int Threshold = 120;       // 직선 검출 임계값

    // 2. sin/cos 룩업 테이블 생성 (성능 최적화)
    double LOOKUP_TABLE_SIN[180];
    double LOOKUP_TABLE_COS[180];

    for (int i = 0; i < THETA; i++) {
        double radian = i * PI / THETA;
        LOOKUP_TABLE_SIN[i] = sin(radian);
        LOOKUP_TABLE_COS[i] = cos(radian);
    }

    // 3. 결과 이미지 메모리 할당
    m_pucLineDetectionImgBuf = memory_alloc2D(height, width);

    // 4. 누적 배열 (Accumulator) 생성 및 초기화
    unsigned int* pNvote = new unsigned int[THETA * rhoSize];
    memset(pNvote, 0, (THETA * rhoSize) * sizeof(unsigned int));

    // 5. 투표 단계: 모든 경계 픽셀에 대해 투표
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 경계 픽셀인가?
            if (edgeimgbuf[i][j] >= 128) {
                // 모든 각도 θ에 대해
                for (int m = 0; m < THETA; m++) {
                    // p 계산: p = x·sin(θ) + y·cos(θ)
                    double p = (i * LOOKUP_TABLE_SIN[m]) + (j * LOOKUP_TABLE_COS[m]);

                    // p를 양수 인덱스로 변환
                    p += range;

                    // 누적 배열에 투표
                    int rhoIndex = (int)(p + 0.5);
                    if (rhoIndex >= 0 && rhoIndex < rhoSize) {
                        pNvote[(m * rhoSize) + rhoIndex]++;
                    }
                }
            }
        }
    }

    // 6. 직선 검출 단계
    for (int i = 0; i < THETA; i++) {
        for (int j = range; j < rhoSize; j++) {
            int nVote = pNvote[i * rhoSize + j];

            // 임계값보다 투표가 많으면 직선으로 인식
            if (nVote > Threshold) {
                bool isTrueLine = true;

                // 7. 지역 최댓값 검출 (Non-maximum Suppression)
                // 주변 ±5 범위에서 최댓값인지 확인
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

                // 8. 직선 그리기
                if (isTrueLine) {
                    int m, n;
                    double rho = j - range;  // 원래 p값으로 복원

                    // 수평 방향으로 점들 계산
                    for (n = 0; n < height; n++) {
                        // x = (p - y·sin(θ)) / cos(θ)
                        m = (int)((rho - n * LOOKUP_TABLE_SIN[i]) / LOOKUP_TABLE_COS[i]);

                        if (m < width && m >= 0) {
                            m_pucLineDetectionImgBuf[0][n * width + m] = 255;
                        }
                    }

                    // 수직 방향으로 점들 계산
                    for (m = 0; m < width; m++) {
                        // y = (p - x·cos(θ)) / sin(θ)
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