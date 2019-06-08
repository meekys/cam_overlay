#pragma once

#include <vector>
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

struct JpegComponent
{
    JpegComponent(int width, int height) : width(width), height(height) {}

    unsigned int width;
    unsigned int height;
    std::vector<unsigned char> data;
};

class JpegDecompress
{
public:
    JpegDecompress();
    ~JpegDecompress();

    static void ErrorHandler(j_common_ptr cinfo);

    void SetMemSrc(void* data, int size);
    void ReadHeader();
    void DecodeRawData();

    std::vector<JpegComponent> components;

private:
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
};