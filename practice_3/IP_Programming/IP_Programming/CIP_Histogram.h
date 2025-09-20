#pragma once
class CIP_Histogram
{
public:
	// 히스토그램 평활화 이미지 저장 버퍼
	UCHAR** m_pucHistEqualImgBuf;

public:
	CIP_Histogram(void);
	~CIP_Histogram(void);
	
	void Histogram_MakeHistogram(UCHAR** HistImgbuf, float HistArray[256], int width, int height);

	// 이미지의 히스토그램 계산
	void Histogram_MakeHistogramEqualization(UCHAR** imgbuf, int width, int height);

	// 히스토그램 평활화
	UCHAR** memory_alloc2D(int width, int height);  // 2D 메모리 할당

	// 밝기 시프트 함수
	void Histogram_BrightnessShift(UCHAR** imgbuf, int width, int height, int shift_value);

	// 히스토그램 매칭 함수
	void Histogram_Matching(UCHAR** source_img, UCHAR** target_img, int width, int height);
};

