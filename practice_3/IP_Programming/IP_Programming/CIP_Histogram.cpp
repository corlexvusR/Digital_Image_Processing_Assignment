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

	// 8-bit depth �ӽ� �迭 ����
	int Image_Histogram[256] = { 0, };

	// ������׷� ���
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Image_Histogram[HistImgbuf[i][j]]++;
		}
	}

	// ������׷� ����ȭ
	float Image_Area = (float)(height * width);
	for (int i = 0; i < 256; i++) {
		HistArray[i] = Image_Histogram[i] / Image_Area;
	}
}

void CIP_Histogram::Histogram_MakeHistogramEqualization(UCHAR** imgbuf, int width, int height) 
{
	int i, j;

	// ���� ������ ������׷� ����
	float Image_Histogram[256] = { 0, };
	Histogram_MakeHistogram(imgbuf, Image_Histogram, width, height);

	// ������׷� ���� �� ���
	double CDF[256] = { 0.0, };
	CDF[0] = Image_Histogram[0];
	for (int i = 1; i < 256; i++) {
		CDF[i] = CDF[i - 1] + Image_Histogram[i];
	}

	m_pucHistEqualImgBuf = memory_alloc2D(height, width);

	// ������׷� ����ȭ
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

	// 2���� �迭 �޸� �Ҵ�
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

// ��� ����Ʈ �Լ�
void CIP_Histogram::Histogram_BrightnessShift(UCHAR** imgbuf, int width, int height, int shift_value)
{
	// ��� ������ ���� �޸� �Ҵ�
	if (m_pucHistEqualImgBuf) {
		free(m_pucHistEqualImgBuf[0]);
		free(m_pucHistEqualImgBuf);
	}
	m_pucHistEqualImgBuf = memory_alloc2D(height, width);

	// ��� ����Ʈ ����
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int new_value = imgbuf[i][j] + shift_value;

			// ���� ���� (0~255)
			if (new_value > 255) new_value = 255;
			if (new_value < 0) new_value = 0;

			m_pucHistEqualImgBuf[i][j] = (UCHAR)new_value;
		}
	}
}

// ������׷� ��Ī �Լ�
void CIP_Histogram::Histogram_Matching(UCHAR** source_img, UCHAR** target_img,
	int width, int height)
{
	// 1. �� ������ ������׷� ���
	float source_hist[256] = { 0, };
	float target_hist[256] = { 0, };

	Histogram_MakeHistogram(source_img, source_hist, width, height);
	Histogram_MakeHistogram(target_img, target_hist, width, height);

	// 2. ������ ���������Լ�(CDF) ���
	double source_cdf[256] = { 0, };
	double target_cdf[256] = { 0, };

	source_cdf[0] = source_hist[0];
	target_cdf[0] = target_hist[0];

	for (int i = 1; i < 256; i++) {
		source_cdf[i] = source_cdf[i - 1] + source_hist[i];
		target_cdf[i] = target_cdf[i - 1] + target_hist[i];
	}

	// 3. ���� ���̺� ����
	int mapping[256];
	for (int i = 0; i < 256; i++) {
		int j = 0;
		double min_diff = 1.0;
		int best_match = 0;

		// ���� ����� target CDF �� ã��
		for (j = 0; j < 256; j++) {
			double diff = fabs(target_cdf[j] - source_cdf[i]);
			if (diff < min_diff) {
				min_diff = diff;
				best_match = j;
			}
		}
		mapping[i] = best_match;
	}

	// 4. ��� ���� ����
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