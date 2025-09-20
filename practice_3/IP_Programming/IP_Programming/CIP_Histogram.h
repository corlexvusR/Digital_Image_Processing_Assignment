#pragma once
class CIP_Histogram
{
public:
	// ������׷� ��Ȱȭ �̹��� ���� ����
	UCHAR** m_pucHistEqualImgBuf;

public:
	CIP_Histogram(void);
	~CIP_Histogram(void);
	
	void Histogram_MakeHistogram(UCHAR** HistImgbuf, float HistArray[256], int width, int height);

	// �̹����� ������׷� ���
	void Histogram_MakeHistogramEqualization(UCHAR** imgbuf, int width, int height);

	// ������׷� ��Ȱȭ
	UCHAR** memory_alloc2D(int width, int height);  // 2D �޸� �Ҵ�

	// ��� ����Ʈ �Լ�
	void Histogram_BrightnessShift(UCHAR** imgbuf, int width, int height, int shift_value);

	// ������׷� ��Ī �Լ�
	void Histogram_Matching(UCHAR** source_img, UCHAR** target_img, int width, int height);
};

