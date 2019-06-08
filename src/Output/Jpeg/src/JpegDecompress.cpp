#include <OutputJpeg/JpegDecompress.hpp>
#include <Core/Exceptions/Exception.hpp>

JpegDecompress::JpegDecompress()
{
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = ErrorHandler;

    if (setjmp(jerr.setjmp_buffer))
    {
        throw Exception("Error decoding JPEG");
    }

    jpeg_create_decompress(&cinfo);
}

JpegDecompress::~JpegDecompress()
{
    jpeg_destroy_decompress(&cinfo);
}

void JpegDecompress::ErrorHandler(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr)cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message)(cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

void JpegDecompress::SetMemSrc(void* data, int size)
{
    jpeg_mem_src(&cinfo, (const unsigned char*)data, size);
}

void JpegDecompress::ReadHeader()
{
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
        throw Exception("Failed reading JPEG headers");

    for (int i = 0; i < cinfo.num_components; i++)
    {
        auto comp = cinfo.comp_info[i];
        auto width = cinfo.image_width / cinfo.max_h_samp_factor * comp.h_samp_factor;
        auto height = cinfo.image_height / cinfo.max_v_samp_factor * comp.v_samp_factor;

        components.push_back(JpegComponent(width, height));
    }
}

void JpegDecompress::DecodeRawData()
{
    for (int i = 0; i < cinfo.num_components; i++)
        components[i].data = std::vector<unsigned char>(components[i].width * components[i].height);

    JSAMPROW yrow_pointer[16];
    JSAMPROW cbrow_pointer[16];
    JSAMPROW crrow_pointer[16];
    JSAMPROW *plane_pointer[3];

    plane_pointer[0] = yrow_pointer;
    plane_pointer[1] = cbrow_pointer;
    plane_pointer[2] = crrow_pointer;

    cinfo.raw_data_out = TRUE;
    cinfo.do_fancy_upsampling = FALSE;
    /* JPEG optimization tools like mozjpeg (based on libjpeg) assume a
        specific DCT implementation when doing rate-distortion trellis
        optimization for coefficient coding.
        The JDCT_ISLOW is the default DCT method in mozjpeg and is also the
        most accurate. */
    cinfo.dct_method = JDCT_ISLOW;

    jpeg_start_decompress(&cinfo);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        for (int i = 0; i < cinfo.num_components; i++)
        {
            auto comp = cinfo.comp_info[i];
            auto maxj = 16 / cinfo.max_h_samp_factor * comp.h_samp_factor;

            for (auto j = 0; j < maxj; j++)
            {
                plane_pointer[i][j] = &components[i].data.data()[(cinfo.output_scanline + j) * components[i].width];
            }
        }

        jpeg_read_raw_data(&cinfo, plane_pointer, 16);
    }

    jpeg_finish_decompress(&cinfo);
}