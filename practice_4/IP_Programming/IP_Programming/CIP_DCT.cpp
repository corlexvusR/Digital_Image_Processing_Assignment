#include "pch.h"
#include "CIP_DCT.h"

#include <cmath>
#include <chrono>

#define PI 3.14159265
#define CLIP(x) ((int)(x) > 255 ? 255 : ((int)(x) < 0 ? 0 : (int)(x)))

// CIP_DCT ������
CIP_DCT::CIP_DCT(void)
	: m_pucForwardDCTbuf(NULL)
	, m_pucTempDCTbuf(NULL)
	, m_pucInverseDCTbuf(NULL)
	, FDCTImgbuf(NULL)
	, IDCTImgbuf(NULL)
{
}

// CIP_DCT �Ҹ���
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
	
// DCT_MakeBlock �Լ�
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

// DCT_ForwardDCT �Լ�
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

//DCTInverseDCT �Լ�
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

// memory_alloc2D_D �Լ�
double** CIP_DCT::memory_alloc2D_D(int width, int height) {
	double** ppMem2D = 0;
	UINT i;

	// 2���� �迭 �޸� �Ҵ�
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

// memory_alloc2D_UC �Լ�
UCHAR** CIP_DCT::memory_alloc2D_UC(int width, int height) {
	UCHAR** ppMem2D = 0;
	UINT i;
	

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

	for (i = 1; i < height; i++) {
		ppMem2D[i] = ppMem2D[i - 1] + width;
	}

	return ppMem2D;
}

// DCT_MakeFrequencytoGray �Լ�
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

// �и� ������ 2D DCT
void CIP_DCT::DCT_Separable2D(double** input, int blocksize, int row, int col, bool forward)
{
	// �ӽ� ���� �Ҵ�
	double** temp_buf = memory_alloc2D_D(blocksize, blocksize);

	// �Է� �����͸� �ӽ� ���۷� ����
	for (int i = 0; i < blocksize; i++) {
		for (int j = 0; j < blocksize; j++) {
			temp_buf[i][j] = input[i][j];
		}
	}

	if (forward) {
		// Forward DCT: �� ���� -> �� ����
		DCT_1D_Row(temp_buf, blocksize, true);
		DCT_1D_Col(temp_buf, blocksize, true);

		// ����� ���� ���۷� ����
		for (int i = 0; i < blocksize; i++) {
			for (int j = 0; j < blocksize; j++) {
				m_pucTempDCTbuf[i + row][j + col] = temp_buf[i][j];
				m_pucForwardDCTbuf[i + row][j + col] = CLIP(temp_buf[i][j]);
			}
		}
	}
	else {
		// Inverse DCT: �� ���� -> �� ����
		DCT_1D_Col(temp_buf, blocksize, false);
		DCT_1D_Row(temp_buf, blocksize, false);

		// ����� ���� ���۷� ����
		for (int i = 0; i < blocksize; i++) {
			for (int j = 0; j < blocksize; j++) {
				m_pucInverseDCTbuf[i + row][j + col] = CLIP(temp_buf[i][j]);
			}
		}
	}

	// �ӽ� ���� ����
	if (temp_buf) {
		free(temp_buf[0]);
		free(temp_buf);
	}
}

// �� ���� 1D DCT
void CIP_DCT::DCT_1D_Row(double** data, int blocksize, bool forward)
{
	double* temp_row = (double*)malloc(sizeof(double) * blocksize);

	for (int y = 0; y < blocksize; y++) {
		if (forward) {
			// Forward 1D DCT on rows
			for (int u = 0; u < blocksize; u++) {
				double Cu = (u == 0) ? 1.0 / sqrt(blocksize) : sqrt(2.0 / blocksize);
				double sum = 0.0;

				for (int x = 0; x < blocksize; x++) {
					sum += data[y][x] * cos((2.0 * x + 1.0) * u * PI / (2.0 * blocksize));
				}

				temp_row[u] = Cu * sum;
			}
		}
		else {
			// Inverse 1D DCT on rows
			for (int x = 0; x < blocksize; x++) {
				double sum = 0.0;

				for (int u = 0; u < blocksize; u++) {
					double Cu = (u == 0) ? 1.0 / sqrt(blocksize) : sqrt(2.0 / blocksize);
					sum += Cu * data[y][u] * cos((2.0 * x + 1.0) * u * PI / (2.0 * blocksize));
				}

				temp_row[x] = sum;
			}
		}

		// ����� ���� �迭�� ����
		for (int i = 0; i < blocksize; i++) {
			data[y][i] = temp_row[i];
		}
	}

	free(temp_row);
}

// �� ���� 1D DCT
void CIP_DCT::DCT_1D_Col(double** data, int blocksize, bool forward)
{
	double* temp_col = (double*)malloc(sizeof(double) * blocksize);

	for (int x = 0; x < blocksize; x++) {
		if (forward) {
			// Forward 1D DCT on columns
			for (int v = 0; v < blocksize; v++) {
				double Cv = (v == 0) ? 1.0 / sqrt(blocksize) : sqrt(2.0 / blocksize);
				double sum = 0.0;

				for (int y = 0; y < blocksize; y++) {
					sum += data[y][x] * cos((2.0 * y + 1.0) * v * PI / (2.0 * blocksize));
				}

				temp_col[v] = Cv * sum;
			}
		}
		else {
			// Inverse 1D DCT on columns
			for (int y = 0; y < blocksize; y++) {
				double sum = 0.0;

				for (int v = 0; v < blocksize; v++) {
					double Cv = (v == 0) ? 1.0 / sqrt(blocksize) : sqrt(2.0 / blocksize);
					sum += Cv * data[v][x] * cos((2.0 * y + 1.0) * v * PI / (2.0 * blocksize));
				}

				temp_col[y] = sum;
			}
		}

		// ����� ���� �迭�� ����
		for (int i = 0; i < blocksize; i++) {
			data[i][x] = temp_col[i];
		}
	}

	free(temp_col);
}

// 2D DCT�� Separable DCT�� ���� �ð��� �����Ͽ� ���� ���̸� ��
void CIP_DCT::ComparePerformance(UCHAR** imgbuf, int width, int height, int blocksize)
{
	// ǥ�� 2D DCT �ð� ����
	auto start_standard = std::chrono::high_resolution_clock::now();

	// ���� �ʱ�ȭ
	double** DCT_block = NULL;
	double** forwardDCT_std = NULL;
	double** tempDCT_std = NULL;
	double** forwardDCT_sep = NULL;
	double** tempDCT_sep = NULL;
	double* temp_row = NULL;
	double* temp_col = NULL;

	// �޸� �Ҵ�
	DCT_block = memory_alloc2D_D(blocksize, blocksize);
	if (!DCT_block) {
		AfxMessageBox(_T("�޸� �Ҵ� ����"));
		return;
	}

	forwardDCT_std = memory_alloc2D_D(width, height);
	if (!forwardDCT_std) {
		free(DCT_block[0]); free(DCT_block);
		AfxMessageBox(_T("�޸� �Ҵ� ����"));
		return;
	}

	tempDCT_std = memory_alloc2D_D(width, height);
	if (!tempDCT_std) {
		free(DCT_block[0]); free(DCT_block);
		free(forwardDCT_std[0]); free(forwardDCT_std);
		AfxMessageBox(_T("�޸� �Ҵ� ����"));
		return;
	}

	// ǥ�� DCT ����
	for (int i = 0; i < height; i += blocksize) {
		for (int j = 0; j < width; j += blocksize) {
			// ��� ���� Ȯ��
			int blockHeight = min(blocksize, height - i);
			int blockWidth = min(blocksize, width - j);

			if (blockHeight != blocksize || blockWidth != blocksize) {
				continue; // ó������ ����
			}

			// ��� ������ ����
			for (int m = 0; m < blocksize; m++) {
				for (int n = 0; n < blocksize; n++) {
					DCT_block[m][n] = (double)imgbuf[i + m][j + n];
				}
			}

			// 2D DCT ���
			for (int u = 0; u < blocksize; u++) {
				double Cu = (u == 0) ? 1.0 / sqrt((double)blocksize) : sqrt(2.0 / blocksize);

				for (int v = 0; v < blocksize; v++) {
					double Cv = (v == 0) ? 1.0 / sqrt((double)blocksize) : sqrt(2.0 / blocksize);
					double sum = 0.0;

					for (int y = 0; y < blocksize; y++) {
						for (int x = 0; x < blocksize; x++) {
							double cosVal1 = cos((2.0 * y + 1.0) * u * PI / (2.0 * blocksize));
							double cosVal2 = cos((2.0 * x + 1.0) * v * PI / (2.0 * blocksize));
							sum += DCT_block[y][x] * cosVal1 * cosVal2;
						}
					}

					if (u + i < height && v + j < width) {
						tempDCT_std[u + i][v + j] = Cu * Cv * sum;
					}
				}
			}
		}
	}

	auto end_standard = std::chrono::high_resolution_clock::now();
	auto duration_standard = std::chrono::duration_cast<std::chrono::microseconds>(end_standard - start_standard);

	// ǥ�� DCT �޸� ����
	free(forwardDCT_std[0]); free(forwardDCT_std); forwardDCT_std = NULL;
	free(tempDCT_std[0]); free(tempDCT_std); tempDCT_std = NULL;

	// �и��� DCT �ð� ����
	auto start_separable = std::chrono::high_resolution_clock::now();

	forwardDCT_sep = memory_alloc2D_D(width, height);
	if (!forwardDCT_sep) {
		free(DCT_block[0]); free(DCT_block);
		AfxMessageBox(_T("�޸� �Ҵ� ����"));
		return;
	}

	tempDCT_sep = memory_alloc2D_D(width, height);
	if (!tempDCT_sep) {
		free(DCT_block[0]); free(DCT_block);
		free(forwardDCT_sep[0]); free(forwardDCT_sep);
		AfxMessageBox(_T("�޸� �Ҵ� ����"));
		return;
	}

	// �и��� DCT ����
	for (int i = 0; i < height; i += blocksize) {
		for (int j = 0; j < width; j += blocksize) {
			// ��� ���� Ȯ��
			int blockHeight = min(blocksize, height - i);
			int blockWidth = min(blocksize, width - j);

			if (blockHeight != blocksize || blockWidth != blocksize) {
				continue; // ó������ ����
			}

			// ��� ������ ����
			for (int m = 0; m < blocksize; m++) {
				for (int n = 0; n < blocksize; n++) {
					DCT_block[m][n] = (double)imgbuf[i + m][j + n];
				}
			}

			// �� ���� 1D DCT
			temp_row = (double*)malloc(sizeof(double) * blocksize);
			if (!temp_row) {
				free(DCT_block[0]); free(DCT_block);
				free(forwardDCT_sep[0]); free(forwardDCT_sep);
				free(tempDCT_sep[0]); free(tempDCT_sep);
				AfxMessageBox(_T("�޸� �Ҵ� ����"));
				return;
			}

			for (int y = 0; y < blocksize; y++) {
				for (int u = 0; u < blocksize; u++) {
					double Cu = (u == 0) ? 1.0 / sqrt((double)blocksize) : sqrt(2.0 / blocksize);
					double sum = 0.0;

					for (int x = 0; x < blocksize; x++) {
						sum += DCT_block[y][x] * cos((2.0 * x + 1.0) * u * PI / (2.0 * blocksize));
					}

					temp_row[u] = Cu * sum;
				}

				for (int x = 0; x < blocksize; x++) {
					DCT_block[y][x] = temp_row[x];
				}
			}
			free(temp_row); temp_row = NULL;

			// �� ���� 1D DCT
			temp_col = (double*)malloc(sizeof(double) * blocksize);
			if (!temp_col) {
				free(DCT_block[0]); free(DCT_block);
				free(forwardDCT_sep[0]); free(forwardDCT_sep);
				free(tempDCT_sep[0]); free(tempDCT_sep);
				AfxMessageBox(_T("�޸� �Ҵ� ����"));
				return;
			}

			for (int x = 0; x < blocksize; x++) {
				for (int v = 0; v < blocksize; v++) {
					double Cv = (v == 0) ? 1.0 / sqrt((double)blocksize) : sqrt(2.0 / blocksize);
					double sum = 0.0;

					for (int y = 0; y < blocksize; y++) {
						sum += DCT_block[y][x] * cos((2.0 * y + 1.0) * v * PI / (2.0 * blocksize));
					}

					temp_col[v] = Cv * sum;
				}

				for (int y = 0; y < blocksize; y++) {
					DCT_block[y][x] = temp_col[y];
				}
			}
			free(temp_col); temp_col = NULL;

			// ��� ����
			for (int m = 0; m < blocksize; m++) {
				for (int n = 0; n < blocksize; n++) {
					if (i + m < height && j + n < width) {
						tempDCT_sep[i + m][j + n] = DCT_block[m][n];
					}
				}
			}
		}
	}

	auto end_separable = std::chrono::high_resolution_clock::now();
	auto duration_separable = std::chrono::duration_cast<std::chrono::microseconds>(end_separable - start_separable);

	// �޸� ����
	free(DCT_block[0]); free(DCT_block);
	free(forwardDCT_sep[0]); free(forwardDCT_sep);
	free(tempDCT_sep[0]); free(tempDCT_sep);

	// ��� ǥ��
	double standard_time_ms = duration_standard.count() / 1000.0;  // ms�� ��ȯ
	double separable_time_ms = duration_separable.count() / 1000.0;  // ms�� ��ȯ
	double speedup = standard_time_ms / separable_time_ms;

	CString result;
	result.Format(_T("���� �� ���:\n")
		_T("ǥ�� 2D DCT: %.3f ms\n")
		_T("�и��� DCT: %.3f ms\n")
		_T("�ӵ� ���: %.2f��"),
		standard_time_ms, separable_time_ms, speedup);

	AfxMessageBox(result);
}