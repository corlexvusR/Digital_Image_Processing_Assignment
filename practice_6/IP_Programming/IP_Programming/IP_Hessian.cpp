#include "pch.h"
#include "IP_Hessian.h"

// 생성자와 소멸자
CIP_Hessian::CIP_Hessian()
    : m_pucCornerImgBuf(NULL)
    , m_pdIxx(NULL)
    , m_pdIyy(NULL)
    , m_pdIxy(NULL)
    , m_pdDet(NULL)
    , m_nWidth(0)
    , m_nHeight(0)
{
}

CIP_Hessian::~CIP_Hessian()
{
    if (m_pucCornerImgBuf) {
        free(m_pucCornerImgBuf[0]);
        free(m_pucCornerImgBuf);
    }

    if (m_pdIxx) memory_free2D(m_pdIxx, m_nHeight);
    if (m_pdIyy) memory_free2D(m_pdIyy, m_nHeight);
    if (m_pdIxy) memory_free2D(m_pdIxy, m_nHeight);
    if (m_pdDet) memory_free2D(m_pdDet, m_nHeight);
}

// 메모리 할당 함수
UCHAR** CIP_Hessian::memory_alloc2D_uchar(int height, int width)
{
    UCHAR** arr = (UCHAR**)malloc(sizeof(UCHAR*) * height);
    arr[0] = (UCHAR*)malloc(sizeof(UCHAR) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(UCHAR) * height * width);
    return arr;
}

double** CIP_Hessian::memory_alloc2D_double(int height, int width)
{
    double** arr = (double**)malloc(sizeof(double*) * height);
    arr[0] = (double*)malloc(sizeof(double) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(double) * height * width);
    return arr;
}

void CIP_Hessian::memory_free2D(UCHAR** arr, int height)
{
    if (arr) {
        free(arr[0]);
        free(arr);
    }
}

void CIP_Hessian::memory_free2D(double** arr, int height)
{
    if (arr) {
        free(arr[0]);
        free(arr);
    }
}

// 메인 함수
void CIP_Hessian::HessianCornerDetection(UCHAR** imgbuf, int height, int width,
    double threshold_ratio)
{
    m_nHeight = height;
    m_nWidth = width;

    // 메모리 할당
    m_pucCornerImgBuf = memory_alloc2D_uchar(height, width);
    m_pdIxx = memory_alloc2D_double(height, width);
    m_pdIyy = memory_alloc2D_double(height, width);
    m_pdIxy = memory_alloc2D_double(height, width);
    m_pdDet = memory_alloc2D_double(height, width);

    // 입력 영상 복사
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            m_pucCornerImgBuf[i][j] = imgbuf[i][j];
        }
    }

    // 1단계: 2차 미분 계산
    ComputeSecondDerivatives(imgbuf, height, width);

    // 2단계: 헤시안 행렬식 계산
    ComputeDeterminant(height, width);

    // 3단계: 최댓값 찾기 및 임계값 계산
    double max_det = 0.0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (m_pdDet[i][j] > max_det) {
                max_det = m_pdDet[i][j];
            }
        }
    }

    double threshold = threshold_ratio * max_det;

    // 4단계: 비최대 억제 및 코너 검출
    NonMaximumSuppression(height, width, threshold);

    // 5단계: 검출된 코너를 영상에 표시
    DrawCorners(imgbuf, height, width);
}

// 2차 미분 계산 구현
void CIP_Hessian::ComputeSecondDerivatives(UCHAR** input, int height, int width)
{
    // 2차 미분 커널
    // Ixx (x방향 2차 미분)
    int kernel_xx[3][3] = {
        { 1, -2,  1},
        { 2, -4,  2},
        { 1, -2,  1}
    };

    // Iyy (y방향 2차 미분)
    int kernel_yy[3][3] = {
        { 1,  2,  1},
        {-2, -4, -2},
        { 1,  2,  1}
    };

    // Ixy (교차 미분)
    int kernel_xy[3][3] = {
        {-1,  0,  1},
        { 0,  0,  0},
        { 1,  0, -1}
    };

    // 컨볼루션 수행
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double ixx = 0.0, iyy = 0.0, ixy = 0.0;

            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    double pixel = (double)input[i + m][j + n];
                    ixx += pixel * kernel_xx[m + 1][n + 1];
                    iyy += pixel * kernel_yy[m + 1][n + 1];
                    ixy += pixel * kernel_xy[m + 1][n + 1];
                }
            }

            m_pdIxx[i][j] = ixx;
            m_pdIyy[i][j] = iyy;
            m_pdIxy[i][j] = ixy;
        }
    }
}

// 헤시안 행렬식 계산
void CIP_Hessian::ComputeDeterminant(int height, int width)
{
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double ixx = m_pdIxx[i][j];
            double iyy = m_pdIyy[i][j];
            double ixy = m_pdIxy[i][j];

            // Det(H) = Ixx * Iyy - Ixy^2
            m_pdDet[i][j] = ixx * iyy - ixy * ixy;
        }
    }
}

// 비최대 억제 및 코너 검출
void CIP_Hessian::NonMaximumSuppression(int height, int width, double threshold)
{
    m_detectedCorners.clear();

    const int WINDOW_SIZE = 3;  // 3x3 윈도우
    const int HALF_WINDOW = WINDOW_SIZE / 2;

    for (int i = HALF_WINDOW; i < height - HALF_WINDOW; i++) {
        for (int j = HALF_WINDOW; j < width - HALF_WINDOW; j++) {
            double det_value = m_pdDet[i][j];

            // 임계값 체크
            if (det_value > threshold) {
                bool is_local_max = true;

                // 3x3 윈도우 내에서 최댓값 확인
                for (int m = -HALF_WINDOW; m <= HALF_WINDOW; m++) {
                    for (int n = -HALF_WINDOW; n <= HALF_WINDOW; n++) {
                        if (m == 0 && n == 0) continue;

                        if (m_pdDet[i + m][j + n] > det_value) {
                            is_local_max = false;
                            break;
                        }
                    }
                    if (!is_local_max) break;
                }

                // 지역 최댓값이면 코너로 저장
                if (is_local_max) {
                    CornerPoint corner;
                    corner.x = j;
                    corner.y = i;
                    corner.response = det_value;
                    m_detectedCorners.push_back(corner);
                }
            }
        }
    }
}

// 코너 표시 (원 그리기)
void CIP_Hessian::DrawCorners(UCHAR** input, int height, int width)
{
    const int RADIUS = 3;  // 코너 표시 반지름
    const UCHAR COLOR = 255;  // 흰색

    for (size_t i = 0; i < m_detectedCorners.size(); i++) {
        int cx = m_detectedCorners[i].x;
        int cy = m_detectedCorners[i].y;

        // 중점 원 알고리즘으로 원 그리기
        int x = 0;
        int y = RADIUS;
        int d = 1 - RADIUS;

        while (x <= y) {
            // 8방향 대칭 점 그리기
            auto putPixel = [&](int px, int py) {
                if (px >= 0 && px < width && py >= 0 && py < height) {
                    m_pucCornerImgBuf[py][px] = COLOR;
                }
            };

            putPixel(cx + x, cy + y);
            putPixel(cx - x, cy + y);
            putPixel(cx + x, cy - y);
            putPixel(cx - x, cy - y);
            putPixel(cx + y, cy + x);
            putPixel(cx - y, cy + x);
            putPixel(cx + y, cy - x);
            putPixel(cx - y, cy - x);

            if (d < 0) {
                d += 2 * x + 3;
            }
            else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }
}