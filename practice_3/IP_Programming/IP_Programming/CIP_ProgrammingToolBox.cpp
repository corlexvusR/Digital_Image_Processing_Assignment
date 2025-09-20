#include "pch.h"

#include "CIP_ProgrammingToolBox.h"

CIP_ProgrammingToolBox::CIP_ProgrammingToolBox(void)
    : m_ReferenceImage(nullptr)
    , m_RefWidth(0)
    , m_RefHeight(0)
{
}


CIP_ProgrammingToolBox::~CIP_ProgrammingToolBox(void)
{
    if (m_ReferenceImage) {
        free(m_ReferenceImage[0]);
        free(m_ReferenceImage);
    }
}

void CIP_ProgrammingToolBox::SetReferenceImage(UCHAR** refImg, int width, int height)
{
    // 기존 참조 영상 해제
    if (m_ReferenceImage) {
        free(m_ReferenceImage[0]);
        free(m_ReferenceImage);
    }

    // 새 참조 영상 복사
    m_RefWidth = width;
    m_RefHeight = height;
    m_ReferenceImage = io.memory_alloc2D(width, height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            m_ReferenceImage[i][j] = refImg[i][j];
        }
    }
}
