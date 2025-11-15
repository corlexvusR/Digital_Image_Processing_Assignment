#include "pch.h"
#include "IP_Morphology.h"

// 생성자: 멤버 변수 초기화
CIP_Morphology::CIP_Morphology(void)
    : m_MorphologyBuf(NULL)
{
}

// 소멸자: 할당된 메모리 해제
CIP_Morphology::~CIP_Morphology(void)
{
    if (m_MorphologyBuf) {
        free(m_MorphologyBuf[0]);  // 데이터 영역 해제
        free(m_MorphologyBuf);      // 포인터 배열 해제
        m_MorphologyBuf = NULL;
    }
}

// 메모리 할당 함수
UCHAR** CIP_Morphology::memory_alloc2D(int height, int width)
{
    UCHAR** ppMem2D = 0;
    UINT i;

    // 2차원 배열 메모리 할당
    // 1단계: 포인터 배열 할당 (행 포인터들)
    ppMem2D = (UCHAR**)calloc(sizeof(UCHAR*), height);
    if (ppMem2D == 0) {
        return 0;
    }

    // 2단계: 실제 데이터 저장 공간 할당
    if ((ppMem2D))
    {
        *ppMem2D = (UCHAR*)calloc(sizeof(UCHAR), height * width);
        if (*ppMem2D == 0) {
            free(ppMem2D);
            return 0;
        }
    }

    // 3단계: 각 행 포인터가 올바른 위치를 가리키도록 설정
    for (i = 1; i < height; i++)
    {
        ppMem2D[i] = ppMem2D[i - 1] + width;
    }

    return ppMem2D;
}

// 팽창(Dilation) 연산자 멤버 함수
void CIP_Morphology::Morphology_Dilation(int width, int height)
{
    // 구조 요소(Structuring Element) 정의
    int masksize = 3;
    UCHAR mask[3][3] =
    {
        { 0, 1, 0 },
        { 1, 1, 1 },
        { 0, 1, 0 }
    };

    // 경계 처리를 위한 패딩된 이미지 버퍼 생성
    UCHAR** Padded_Imgbuf = memory_alloc2D(height + (masksize >> 1),
        width + (masksize >> 1));

    // 입력 이미지를 패딩된 버퍼의 중앙에 복사
    for (int i = 1; i <= height; i++)
        memcpy(&Padded_Imgbuf[i][masksize >> 1],
            m_MorphologyBuf[i - 1],
            sizeof(UCHAR) * width);

    // 팽창 연산 수행
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            // 현재 픽셀이 검은색(객체)인 경우
            if (Padded_Imgbuf[i][j] < 128) {
                // 구조 요소의 모든 위치 확인
                for (int m = 0; m < masksize; m++) {
                    for (int n = 0; n < masksize; n++) {
                        // 구조 요소가 1인 위치에 검은색 설정
                        if (mask[m][n] == 1)
                            m_MorphologyBuf[i + m - 1][j + n - 1] = 0;
                    }
                }
            }
        }
    }

    // 임시 버퍼 메모리 해제
    free(Padded_Imgbuf[0]);
    free(Padded_Imgbuf);
}

// 침식(Erosion) 연산자 멤버 함수
void CIP_Morphology::Morphology_Erosion(int width, int height)
{
    // 구조 요소 정의
    int masksize = 3;
    UCHAR mask[3][3] =
    {
        { 0, 1, 0 },
        { 1, 1, 1 },
        { 0, 1, 0 }
    };

    // 패딩된 이미지 버퍼 생성
    UCHAR** Padded_Imgbuf = memory_alloc2D(height + (masksize >> 1), width + (masksize >> 1));

    // 입력 이미지를 패딩된 버퍼에 복사
    for (int i = 1; i <= height; i++)
        memcpy(&Padded_Imgbuf[i][masksize >> 1], m_MorphologyBuf[i - 1], sizeof(UCHAR) * width);

    // 침식 연산 수행
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            // 현재 픽셀이 흰색(배경)인 경우
            if (Padded_Imgbuf[i][j] >= 128) {
                // 구조 요소의 모든 위치 확인
                for (int m = 0; m < masksize; m++) {
                    for (int n = 0; n < masksize; n++) {
                        // 구조 요소가 1인 위치에 흰색 설정
                        if (mask[m][n] == 1)
                            m_MorphologyBuf[i + m - 1][j + n - 1] = 255;
                    }
                }
            }
        }
    }

    // 임시 버퍼 메모리 해제
    free(Padded_Imgbuf[0]);
    free(Padded_Imgbuf);
}

// 닫힘(Closing) 연산
void CIP_Morphology::Morphology_Closing(UCHAR** input, int width, int height)
{
    int row = 3;
    int col = 3;
    int masksize = row;

    // 메모리 할당
    if (!m_MorphologyBuf)
        m_MorphologyBuf = memory_alloc2D(height, width);

    // 입력 이미지 복사
    for (int i = 0; i < height; i++)
        memcpy(m_MorphologyBuf[i], input[i], sizeof(UCHAR) * width);

    // 닫힘 = 팽창 후 침식
    Morphology_Dilation(width, height);
    Morphology_Erosion(width, height);
}

// 열림(Opening) 연산
void CIP_Morphology::Morphology_Opening(UCHAR** input, int width, int height)
{
    int row = 3;
    int col = 3;
    int masksize = row;

    // 메모리 할당
    if (!m_MorphologyBuf)
        m_MorphologyBuf = memory_alloc2D(height, width);

    // 입력 이미지 복사
    for (int i = 0; i < height; i++)
        memcpy(m_MorphologyBuf[i], input[i], sizeof(UCHAR) * width);

    // 열림 = 침식 후 팽창
    Morphology_Erosion(width, height);
    Morphology_Dilation(width, height);
}

// 내부 침식 함수 (침식 결과를 별도 버퍼에 저장하는 함수)
UCHAR** CIP_Morphology::Internal_Erosion(UCHAR** input, int width, int height)
{
    // 결과를 저장할 새 버퍼 할당
    UCHAR** result = memory_alloc2D(height, width);

    // 입력 영상을 결과 버퍼에 복사
    for (int i = 0; i < height; i++) {
        memcpy(result[i], input[i], sizeof(UCHAR) * width);
    }

    // 구조 요소 정의
    int masksize = 3;
    UCHAR mask[3][3] =
    {
        { 0, 1, 0 },
        { 1, 1, 1 },
        { 0, 1, 0 }
    };

    // 패딩된 버퍼 생성
    UCHAR** Padded_Imgbuf = memory_alloc2D(height + (masksize >> 1), width + (masksize >> 1));

    // 입력을 패딩 버퍼에 복사
    for (int i = 1; i <= height; i++) {
        memcpy(&Padded_Imgbuf[i][masksize >> 1], input[i - 1], sizeof(UCHAR) * width);
    }

    // 침식 연산
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (Padded_Imgbuf[i][j] >= 128) {  // 배경(흰색)인 경우
                for (int m = 0; m < masksize; m++) {
                    for (int n = 0; n < masksize; n++) {
                        if (mask[m][n] == 1) {
                            result[i + m - 1][j + n - 1] = 255;
                        }
                    }
                }
            }
        }
    }

    // 임시 버퍼 해제
    free(Padded_Imgbuf[0]);
    free(Padded_Imgbuf);

    return result;
}

// 경계 추출 함수
void CIP_Morphology::Morphology_EdgeExtraction(UCHAR** input, int width, int height)
{
    // 메모리 할당
    if (!m_MorphologyBuf) {
        m_MorphologyBuf = memory_alloc2D(height, width);
    }

    // 침식 영상 생성
    UCHAR** eroded = Internal_Erosion(input, width, height);

    // 경계 추출: 원본 - 침식
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 원본이 객체(검은색)이고 침식 결과가 배경(흰색)인 경우가 "경계"임
            if (input[i][j] < 128 && eroded[i][j] >= 128) {
                m_MorphologyBuf[i][j] = 0;  // 경계선 (검은색)
            }
            else {
                m_MorphologyBuf[i][j] = 255;  // 배경 (흰색)
            }
        }
    }

    // 침식 버퍼 해제
    free(eroded[0]);
    free(eroded);
}