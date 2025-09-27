#pragma once
class CIP_DCT
{
public:
	double** m_pucForwardDCTbuf;	// clip CDT ��ȯ ��� ���� ����
	double** m_pucTempDCTbuf;	// CDT ��ȯ ��� ���� ����
	double** m_pucInverseDCTbuf;	//ICDT ��ȯ ��� ���� ����

	UCHAR** FDCTImgbuf;	// DCT �̹��� ���� ����
	UCHAR** IDCTImgbuf;	// IDCT �̹��� ���� ����

public:
	CIP_DCT(void);
	~CIP_DCT(void);

	void DCT_MakeBlock(int DCT_blocksize, int select, UCHAR** imgbuf, int width, int height);	// �̹����� ��� ������ �����Ͽ� DCT ��ȯ ����
	void DCT_ForwardDCT(double** DCT_Block, int DCT_blocksize, int row, int col);	// FDCT
	void DCT_InverseDCT(double** DCT_Block, int DCT_blocksize, int row, int col);	// IDCT
	void DCT_MakeFrequencytoGray(double** imgbuf, UCHAR** buf, int width, int height);	// Frequency -> Gray ��ȯ
	double** memory_alloc2D_D(int width, int height);	// 2D �޸� �Ҵ�
	UCHAR** memory_alloc2D_UC(int width, int height);	// 2D �޸� �Ҵ�

	// 1���� DCT ���� �޼���
	void DCT_Separable2D(double** input, int blocksize, int row, int col, bool forward);
	void DCT_1D_Row(double** data, int blocksize, bool forward);
	void DCT_1D_Col(double** data, int blocksize, bool forward);
	void ComparePerformance(UCHAR** imgbuf, int width, int height, int blocksize);
};

