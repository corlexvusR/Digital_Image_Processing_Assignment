#pragma once

class CIP_Morphology
{
public:
    // 멤버 변수
    UCHAR** m_MorphologyBuf;  // 형태학 연산 결과를 저장할 버퍼

public:
    // 생성자 및 소멸자
    CIP_Morphology(void);
    ~CIP_Morphology(void);

    // 메모리 관리 함수
    UCHAR** memory_alloc2D(int height, int width);

    // 형태학 연산 함수
    void Morphology_Dilation(int width, int height);   // 팽창
    void Morphology_Erosion(int width, int height);    // 침식
    void Morphology_Closing(UCHAR** input, int width, int height);  // 닫힘
    void Morphology_Opening(UCHAR** input, int width, int height);  // 열림
};