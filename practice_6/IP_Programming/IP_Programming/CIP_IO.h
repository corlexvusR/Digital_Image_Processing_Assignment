#pragma once

class CIP_IO
{
public:
    BITMAPINFOHEADER m_BMPheader;
    UCHAR* m_BMPbuf;
    UCHAR** m_Inputbuf;
    UCHAR** m_Outputbuf;

    int              m_Width;
    int              m_Height;

public:
    CIP_IO(void);
    ~CIP_IO(void);

    void IO_MakeGrayImagetoBMP(UCHAR** imgbuf);
    UCHAR** memory_alloc2D(int width, int height);
};