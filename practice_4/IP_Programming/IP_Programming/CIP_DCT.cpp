#include "pch.h"
#include "CIP_DCT.h"

#include <cmath>

#define PI 3.14159265
#define CLIP(x) ((int)(x) > 255 ? 255 : ((int)(x) < 0 ? 0 : (int)(x)))

// CIP_DCT 생성자
CIP_DCT::CIP_DCT(void)
	: m_pucForwardDCTbuf(NULL)
	, m_pucTempDCTbuf(NULL)
	, m_pucInverseDCTbuf(NULL)
	, FDCTImgbuf(NULL)
	, IDCTImgbuf(NULL)
{
}

// CIP_DCT 소멸자
CIP_DCT::~CIP_DCT(void) {
	if (m_pucForwardDCTbuf) {
		free(m_pucForwardDCTbuf[0]);
		free(m_pucForwardDCTbuf);
		m_pucForwardDCTbuf = NULL;
	}
	if (m_pucTempDCTbuf) {
		free(m_pucTempDCTbuf[0]);
		free(m_pucTempDCTbuf);
		m_pucTempDCTbuf = NULL;
	}
	if (m_pucInverseDCTbuf) {
		free(m_pucInverseDCTbuf[0]);
		free(m_pucInverseDCTbuf);
		m_pucInverseDCTbuf = NULL;
	}
	if (FDCTImgbuf) {
		free(FDCTImgbuf[0]);
		free(FDCTImgbuf);
		FDCTImgbuf = NULL;
	}
	if (IDCTImgbuf) {
		free(IDCTImgbuf[0]);
		free(IDCTImgbuf);
		IDCTImgbuf = NULL;
	}
}
	
// DCT_MakeBlock 함수
void CIP_DCT::DCT_MakeBlock(int DCT_blocksize, int select, UCHAR** imgbuf, int width, int height)
{
	if (select == 1) { // FDCT
		m_pucForwardDCTbuf = memory_alloc2D_D(width, height);
		m_pucTempDCTbuf = memory_alloc2D_D(width, height);
		FDCTImgbuf = memory_alloc2D_UC(width, height);
	}
	if (select == 0) { // IDCT
		m_pucInverseDCTbuf = memory_alloc2D_D(width, height);
		IDCTImgbuf = memory_alloc2D_UC(width, height);
	}
	
	double** DCT_block = memory_alloc2D_D(DCT_blocksize, DCT_blocksize);
	
	for (int i = 0; i < height; i += DCT_blocksize) {
		for (int j = 0; j < width; j += DCT_blocksize) {
			for (int m = 0; m < DCT_blocksize; m++) {
				for (int n = 0; n < DCT_blocksize; n++) {  
					if (select == 1) DCT_block[m][n] = (double)imgbuf[i + m][j + n];
					if (select == 0) DCT_block[m][n] = (double)m_pucTempDCTbuf[i + m][j + n];
				}
			}
			if (select == 1) DCT_ForwardDCT(DCT_block, DCT_blocksize, i, j);
			if (select == 0) DCT_InverseDCT(DCT_block, DCT_blocksize, i, j);
		}
	}
}

// DCT_ForwardDCT 함수
void CIP_DCT::DCT_ForwardDCT(double** DCT_Block, int DCT_blocksize, int row, int col)
{
	double Sum, Cu, Cv, DCT_max = 0;
	for (int u = 0; u < DCT_blocksize; u++) {
		if (u == 0)
			Cu = 1 / sqrt(DCT_blocksize);
		else
			Cu = sqrt(2.0 / DCT_blocksize);

		for (int v = 0; v < DCT_blocksize; v++) {
			Sum = 0;
			if (v == 0)
				Cv = 1 / sqrt(DCT_blocksize);
			else
				Cv = sqrt(2.0 / DCT_blocksize);
			for (int y = 0; y < DCT_blocksize; y++) {
				for (int x = 0; x < DCT_blocksize; x++) {
					double cosVal1 = Cu * cos((2 * y + 1) * v * PI / (2 * DCT_blocksize));
					double cosVal2 = Cv * cos((2 * x + 1) * u * PI / (2 * DCT_blocksize));
					Sum += DCT_Block[y][x] * cosVal1 * cosVal2;
				}
			}
			m_pucTempDCTbuf[u + row][v + col] = Sum;
			m_pucForwardDCTbuf[u + row][v + col] = CLIP(Sum);
		}
	}
}

//DCTInverseDCT 함수
void CIP_DCT::DCT_InverseDCT(double** DCT_Block, int DCT_blocksize, int row, int col)
{
	double idct = 0;
	double Sum = 0;
	double Cu, Cv;

	for (int y = 0; y < DCT_blocksize; y++) {
		for (int x = 0; x < DCT_blocksize; x++) {
			Sum = 0;
			for (int u = 0; u < DCT_blocksize; u++) {
				if (u == 0)
					Cu = 1 / sqrt(DCT_blocksize);
				else
					Cu = sqrt(2.0 / DCT_blocksize);
				for (int v = 0; v < DCT_blocksize; v++) {
					if (v == 0)
						Cv = 1 / sqrt(DCT_blocksize);
					else
						Cv = sqrt(2.0 / DCT_blocksize);
					double cosVal1 = Cu * cos((2 * y + 1) * v * PI / (2 * DCT_blocksize));
					double cosVal2 = Cv * cos((2 * x + 1) * u * PI / (2 * DCT_blocksize));
					Sum += DCT_Block[u][v] * cosVal1 * cosVal2;
				}
			}
			m_pucInverseDCTbuf[y + row][x + col] = CLIP(Sum);
		}
	}
}

// memory_alloc2D_D 함수
double** CIP_DCT::memory_alloc2D_D(int width, int height) {
	double** ppMem2D = 0;
	UINT i;

	// 2차원 배열 메모리 할당
	ppMem2D = (double**)calloc(sizeof(double*), height);
	if (ppMem2D == 0) {
		return 0;
	}
	
	*ppMem2D = (double*)calloc(sizeof(double), height * width);
	if ((*ppMem2D) == 0) {
		free(ppMem2D);
		return 0;
	}
	
	for (i = 1; i < height; i++) {
		ppMem2D[i] = ppMem2D[i - 1] + width;
	}

	return ppMem2D;
}

// memory_alloc2D_UC 함수
UCHAR** CIP_DCT::memory_alloc2D_UC(int width, int height) {
	UCHAR** ppMem2D = 0;
	UINT i;
	

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

	for (i = 1; i < height; i++) {
		ppMem2D[i] = ppMem2D[i - 1] + width;
	}

	return ppMem2D;
}

// DCT_MakeFrequencytoGray 함수
void CIP_DCT::DCT_MakeFrequencytoGray(double** imgbuf, UCHAR** buf, int width, int height) {
	int i, j, idx = 0;
	if (!buf)
		buf = memory_alloc2D_UC(width, height);
	for (i = height - 1; i >= 0; i--) {
		for (j = 0; j < width; j++) {
			buf[i][j] = imgbuf[i][j];
		}
	}
}
