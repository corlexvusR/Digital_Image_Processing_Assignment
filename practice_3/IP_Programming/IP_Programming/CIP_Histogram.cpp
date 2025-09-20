#include "pch.h"
#include "CIP_Histogram.h"

CIP_Histogram::CIP_Histogram(void)
	: m_pucHistEqualImgBuf(NULL)
{

}

CIP_Histogram::~CIP_Histogram(void)
{
	if (m_pucHistEqualImgBuf) {
		free(m_pucHistEqualImgBuf[0]);
		free(m_pucHistEqualImgBuf);
		m_pucHistEqualImgBuf = NULL;
	}
}

void CIP_Histogram::Histogram_MakeHistogram(UCHAR** HistImgbuf, float HistArray[256], int width, int height)
{
	int i, j;

	// 8-bit depth 임시 배열 생성
	int Image_Histogram[256] = { 0, };

	// 히스토그램 계산
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Image_Histogram[HistImgbuf[i][j]]++;
		}
	}

	// 히스토그램 정규화
	float Image_Area = (float)(height * width);
	for (int i = 0; i < 256; i++) {
		HistArray[i] = Image_Histogram[i] / Image_Area;
	}
}

void CIP_Histogram::Histogram_MakeHistogramEqualization(UCHAR** imgbuf, int width, int height) 
{
	int i, j;

	// 원본 영상의 히스토그램 생성
	float Image_Histogram[256] = { 0, };
	Histogram_MakeHistogram(imgbuf, Image_Histogram, width, height);

	// 히스토그램 누적 합 계산
	double CDF[256] = { 0.0, };
	CDF[0] = Image_Histogram[0];
	for (int i = 1; i < 256; i++) {
		CDF[i] = CDF[i - 1] + Image_Histogram[i];
	}

	m_pucHistEqualImgBuf = memory_alloc2D(height, width);

	// 히스토그램 정규화
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			m_pucHistEqualImgBuf[i][j] = (UCHAR)(CDF[imgbuf[i][j]] * 255);
		}
	}
}

UCHAR** CIP_Histogram::memory_alloc2D(int width, int height)
{
	UCHAR** ppMem2D = 0;
	int i;

	// 2차원 배열 메모리 할당
	ppMem2D = (UCHAR**)calloc(sizeof(UCHAR*), height);
	if (ppMem2D == 0) {
		return 0;
	}

	*ppMem2D = (UCHAR*)calloc(sizeof(UCHAR), height * width);
	if ((*ppMem2D) == 0) {
		free(ppMem2D);
		return 0;
	}

	for (int i = 1; i < height; i++) {
		ppMem2D[i] = ppMem2D[i - 1] + width;
	}

	return ppMem2D;
}

// 밝기 시프트 함수
void CIP_Histogram::Histogram_BrightnessShift(UCHAR** imgbuf, int width, int height, int shift_value)
{
	// 결과 영상을 위한 메모리 할당
	if (m_pucHistEqualImgBuf) {
		free(m_pucHistEqualImgBuf[0]);
		free(m_pucHistEqualImgBuf);
	}
	m_pucHistEqualImgBuf = memory_alloc2D(height, width);

	// 밝기 시프트 적용
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int new_value = imgbuf[i][j] + shift_value;

			// 범위 제한 (0~255)
			if (new_value > 255) new_value = 255;
			if (new_value < 0) new_value = 0;

			m_pucHistEqualImgBuf[i][j] = (UCHAR)new_value;
		}
	}
}

// 히스토그램 매칭 함수
void CIP_Histogram::Histogram_Matching(UCHAR** source_img, UCHAR** target_img,
	int width, int height)
{
	// 1. 두 영상의 히스토그램 계산
	float source_hist[256] = { 0, };
	float target_hist[256] = { 0, };

	Histogram_MakeHistogram(source_img, source_hist, width, height);
	Histogram_MakeHistogram(target_img, target_hist, width, height);

	// 2. 각각의 누적분포함수(CDF) 계산
	double source_cdf[256] = { 0, };
	double target_cdf[256] = { 0, };

	source_cdf[0] = source_hist[0];
	target_cdf[0] = target_hist[0];

	for (int i = 1; i < 256; i++) {
		source_cdf[i] = source_cdf[i - 1] + source_hist[i];
		target_cdf[i] = target_cdf[i - 1] + target_hist[i];
	}

	// 3. 매핑 테이블 생성
	int mapping[256];
	for (int i = 0; i < 256; i++) {
		int j = 0;
		double min_diff = 1.0;
		int best_match = 0;

		// 가장 가까운 target CDF 값 찾기
		for (j = 0; j < 256; j++) {
			double diff = fabs(target_cdf[j] - source_cdf[i]);
			if (diff < min_diff) {
				min_diff = diff;
				best_match = j;
			}
		}
		mapping[i] = best_match;
	}

	// 4. 결과 영상 생성
	if (m_pucHistEqualImgBuf) {
		free(m_pucHistEqualImgBuf[0]);
		free(m_pucHistEqualImgBuf);
	}
	m_pucHistEqualImgBuf = memory_alloc2D(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			m_pucHistEqualImgBuf[i][j] = mapping[source_img[i][j]];
		}
	}
}