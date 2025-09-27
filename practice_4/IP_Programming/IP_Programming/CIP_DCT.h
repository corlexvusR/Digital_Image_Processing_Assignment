#pragma once
class CIP_DCT
{
public:
	double** m_pucForwardDCTbuf;	// clip CDT 변환 결과 저장 버퍼
	double** m_pucTempDCTbuf;	// CDT 변환 결과 저장 버퍼
	double** m_pucInverseDCTbuf;	//ICDT 변환 결과 저장 버퍼

	UCHAR** FDCTImgbuf;	// DCT 이미지 저장 버퍼
	UCHAR** IDCTImgbuf;	// IDCT 이미지 저장 버퍼

public:
	CIP_DCT(void);
	~CIP_DCT(void);

	void DCT_MakeBlock(int DCT_blocksize, int select, UCHAR** imgbuf, int width, int height);	// 이미지를 블록 단위로 분할하여 DCT 변환 수행
	void DCT_ForwardDCT(double** DCT_Block, int DCT_blocksize, int row, int col);	// FDCT
	void DCT_InverseDCT(double** DCT_Block, int DCT_blocksize, int row, int col);	// IDCT
	void DCT_MakeFrequencytoGray(double** imgbuf, UCHAR** buf, int width, int height);	// Frequency -> Gray 변환
	double** memory_alloc2D_D(int width, int height);	// 2D 메모리 할당
	UCHAR** memory_alloc2D_UC(int width, int height);	// 2D 메모리 할당

	// 1차원 DCT 관련 메서드
	void DCT_Separable2D(double** input, int blocksize, int row, int col, bool forward);
	void DCT_1D_Row(double** data, int blocksize, bool forward);
	void DCT_1D_Col(double** data, int blocksize, bool forward);
	void ComparePerformance(UCHAR** imgbuf, int width, int height, int blocksize);
};

