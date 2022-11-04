#include <cstdlib>
#include <iostream>
#include <fstream>

#include "webp_encoder.h"
#include "common.h"

#include "webp/encode.h"

WebpEncoder::WebpEncoder(unsigned char *ddata, int wwidth, int hheight, buffer_type bbuf_type) :
    data(ddata), width(wwidth), height(hheight), buf_type(bbuf_type),
    webp(NULL), webp_len(0) {}

WebpEncoder::~WebpEncoder() {
    WebPFree(webp);
}

/*
static int filecounter = 1;
*/

void
WebpEncoder::encode()
{
    if (buf_type == BUF_RGB)
        webp_len = WebPEncodeRGB((uint8_t*)data, width, height, width*3, 50, &webp);
    else if (buf_type == BUF_BGR)
        webp_len = WebPEncodeBGR((uint8_t*)data, width, height, width*3, 50, &webp);
    else if (buf_type == BUF_RGBA)
        webp_len = WebPEncodeRGBA((uint8_t*)data, width, height, width*4, 50, &webp);
    else if (buf_type == BUF_BGRA)
        webp_len = WebPEncodeBGRA((uint8_t*)data, width, height, width*4, 50, &webp);
    
    /*
    std::ofstream out;
    char outfile[128];

    std::sprintf(outfile, "C:\\webp\\%05d-%d-%d.webp", filecounter, width, height);
    out.open(outfile, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    out.write((char *)webp, webp_len);
    out.close();

    std::sprintf(outfile, "C:\\webp\\%05d-%d-%d.raw", filecounter, width, height);
    out.open(outfile, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    out.write((char *)data, width * height * 4);
    out.close();

    filecounter++;
    */
}

const uint8_t *
WebpEncoder::get_webp() const {
    return webp;
}

int
WebpEncoder::get_webp_len() const {
    return webp_len;
}

