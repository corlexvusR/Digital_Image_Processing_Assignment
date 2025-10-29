#include "pch.h"
#include "IP_Canny.h"

CIP_Canny::CIP_Canny()
    : m_pucCannyEdgeImgBuf(NULL)
    , m_pucBlurredImg(NULL)
    , m_pnGx(NULL)
    , m_pnGy(NULL)
    , m_pdMagnitude(NULL)
    , m_pdDirection(NULL)
    , m_pucSuppressed(NULL)
    , m_nWidth(0)
    , m_nHeight(0)
{
}

CIP_Canny::~CIP_Canny()
{
    if (m_pucCannyEdgeImgBuf) {
        free(m_pucCannyEdgeImgBuf[0]);
        free(m_pucCannyEdgeImgBuf);
    }
    if (m_pucBlurredImg) memory_free2D(m_pucBlurredImg, m_nHeight);
    if (m_pnGx) memory_free2D(m_pnGx, m_nHeight);
    if (m_pnGy) memory_free2D(m_pnGy, m_nHeight);
    if (m_pdMagnitude) memory_free2D(m_pdMagnitude, m_nHeight);
    if (m_pdDirection) memory_free2D(m_pdDirection, m_nHeight);
    if (m_pucSuppressed) memory_free2D(m_pucSuppressed, m_nHeight);
}

UCHAR** CIP_Canny::memory_alloc2D_uchar(int height, int width)
{
    UCHAR** arr = (UCHAR**)malloc(sizeof(UCHAR*) * height);
    arr[0] = (UCHAR*)malloc(sizeof(UCHAR) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(UCHAR) * height * width);
    return arr;
}

int** CIP_Canny::memory_alloc2D_int(int height, int width)
{
    int** arr = (int**)malloc(sizeof(int*) * height);
    arr[0] = (int*)malloc(sizeof(int) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(int) * height * width);
    return arr;
}

double** CIP_Canny::memory_alloc2D_double(int height, int width)
{
    double** arr = (double**)malloc(sizeof(double*) * height);
    arr[0] = (double*)malloc(sizeof(double) * height * width);

    for (int i = 1; i < height; i++) {
        arr[i] = arr[i - 1] + width;
    }

    memset(arr[0], 0, sizeof(double) * height * width);
    return arr;
}

void CIP_Canny::memory_free2D(UCHAR** arr, int height)
{
    if (arr) {
        free(arr[0]);
        free(arr);
    }
}

void CIP_Canny::memory_free2D(int** arr, int height)
{
    if (arr) {
        free(arr[0]);
        free(arr);
    }
}

void CIP_Canny::memory_free2D(double** arr, int height)
{
    if (arr) {
        free(arr[0]);
        free(arr);
    }
}

void CIP_Canny::CannyEdgeDetection(UCHAR** imgbuf, int height, int width, int low_threshold, int high_threshold)
{
    m_nHeight = height;
    m_nWidth = width;

    // �޸� �Ҵ�
    m_pucCannyEdgeImgBuf = memory_alloc2D_uchar(height, width);
    m_pucBlurredImg = memory_alloc2D_uchar(height, width);
    m_pnGx = memory_alloc2D_int(height, width);
    m_pnGy = memory_alloc2D_int(height, width);
    m_pdMagnitude = memory_alloc2D_double(height, width);
    m_pdDirection = memory_alloc2D_double(height, width);
    m_pucSuppressed = memory_alloc2D_uchar(height, width);

    // 1. ����þ� ����
    GaussianBlur(imgbuf, m_pucBlurredImg, height, width);

    // 2. �׷����Ʈ ���
    ComputeGradient(m_pucBlurredImg, height, width);

    // 3. ���ִ� ����
    NonMaximumSuppression(height, width);

    // 4. ���� �Ӱ谪
    DoubleThreshold(height, width, low_threshold, high_threshold);

    // 5. ���� ����
    EdgeTracking(height, width);
}

void CIP_Canny::GaussianBlur(UCHAR** input, UCHAR** output, int height, int width)
{
    // 5x5 ����þ� Ŀ�� (�� = 1.4)
    double kernel[5][5] = {
        {2,  4,  5,  4,  2},
        {4,  9, 12,  9,  4},
        {5, 12, 15, 12,  5},
        {4,  9, 12,  9,  4},
        {2,  4,  5,  4,  2}
    };
    double sum_kernel = 159.0;

    // ������� ����
    for (int i = 2; i < height - 2; i++) {
        for (int j = 2; j < width - 2; j++) {
            double sum = 0.0;

            for (int m = -2; m <= 2; m++) {
                for (int n = -2; n <= 2; n++) {
                    sum += input[i + m][j + n] * kernel[m + 2][n + 2];
                }
            }

            output[i][j] = (UCHAR)(sum / sum_kernel);
        }
    }

    // �׵θ� ����
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < 2; j++) {
            output[i][j] = input[i][j];
            output[i][width - 1 - j] = input[i][width - 1 - j];
        }
    }
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < 2; i++) {
            output[i][j] = input[i][j];
            output[height - 1 - i][j] = input[height - 1 - i][j];
        }
    }
}

void CIP_Canny::ComputeGradient(UCHAR** input, int height, int width)
{
    // �Һ� Ŀ��
    int Sobel_x[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int Sobel_y[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int Gx = 0, Gy = 0;

            // Gx ���
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    Gx += input[i + m][j + n] * Sobel_x[m + 1][n + 1];
                }
            }

            // Gy ���
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    Gy += input[i + m][j + n] * Sobel_y[m + 1][n + 1];
                }
            }

            m_pnGx[i][j] = Gx;
            m_pnGy[i][j] = Gy;

            // ũ��� ���� ���
            m_pdMagnitude[i][j] = sqrt((double)(Gx * Gx + Gy * Gy));
            m_pdDirection[i][j] = atan2((double)Gy, (double)Gx) * 180.0 / 3.14159265;
        }
    }
}

void CIP_Canny::NonMaximumSuppression(int height, int width)
{
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double angle = m_pdDirection[i][j];
            double magnitude = m_pdMagnitude[i][j];

            // ������ 0~180 ������ ����ȭ
            if (angle < 0) angle += 180.0;

            double neighbor1 = 0.0, neighbor2 = 0.0;

            // ������ ���� �̿� �ȼ� ����
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                // 0�� (����): �¿� ��
                neighbor1 = m_pdMagnitude[i][j - 1];
                neighbor2 = m_pdMagnitude[i][j + 1];
            }
            else if (angle >= 22.5 && angle < 67.5) {
                // 45�� (�밢��): �� ��
                neighbor1 = m_pdMagnitude[i + 1][j - 1];
                neighbor2 = m_pdMagnitude[i - 1][j + 1];
            }
            else if (angle >= 67.5 && angle < 112.5) {
                // 90�� (����): ���� ��
                neighbor1 = m_pdMagnitude[i - 1][j];
                neighbor2 = m_pdMagnitude[i + 1][j];
            }
            else if (angle >= 112.5 && angle < 157.5) {
                // 135�� (�밢��): �� ��
                neighbor1 = m_pdMagnitude[i - 1][j - 1];
                neighbor2 = m_pdMagnitude[i + 1][j + 1];
            }

            // ���� �ִ񰪸� ����
            if (magnitude >= neighbor1 && magnitude >= neighbor2) {
                m_pucSuppressed[i][j] = (UCHAR)(magnitude > 255 ? 255 : magnitude);
            }
            else {
                m_pucSuppressed[i][j] = 0;
            }
        }
    }
}

void CIP_Canny::DoubleThreshold(int height, int width, int low, int high)
{
    const UCHAR STRONG = 255;
    const UCHAR WEAK = 128;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            UCHAR pixel = m_pucSuppressed[i][j];

            if (pixel >= high) {
                m_pucCannyEdgeImgBuf[i][j] = STRONG;  // ���� ���
            }
            else if (pixel >= low) {
                m_pucCannyEdgeImgBuf[i][j] = WEAK;    // ���� ���
            }
            else {
                m_pucCannyEdgeImgBuf[i][j] = 0;       // ��� �ƴ�
            }
        }
    }
}

void CIP_Canny::EdgeTracking(int height, int width)
{
    const UCHAR STRONG = 255;
    const UCHAR WEAK = 128;

    // BFS�� ���� ť
    std::queue<std::pair<int, int>> edgeQueue;

    // ���� ��踦 ť�� �߰�
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (m_pucCannyEdgeImgBuf[i][j] == STRONG) {
                edgeQueue.push(std::make_pair(i, j));
            }
        }
    }

    // 8����
    int dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    // BFS�� ���� ��� ����
    while (!edgeQueue.empty()) {
        std::pair<int, int> current = edgeQueue.front();
        edgeQueue.pop();

        int i = current.first;
        int j = current.second;

        // 8���� �̿� Ȯ��
        for (int k = 0; k < 8; k++) {
            int ni = i + dx[k];
            int nj = j + dy[k];

            if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                if (m_pucCannyEdgeImgBuf[ni][nj] == WEAK) {
                    m_pucCannyEdgeImgBuf[ni][nj] = STRONG;  // ���� ���� �°�
                    edgeQueue.push(std::make_pair(ni, nj));
                }
            }
        }
    }

    // ������� ���� ���� ��� ����
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (m_pucCannyEdgeImgBuf[i][j] == WEAK) {
                m_pucCannyEdgeImgBuf[i][j] = 0;
            }
        }
    }
}