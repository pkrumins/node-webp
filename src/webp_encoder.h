#ifndef WEBP_ENCODER_H
#define WEBP_ENCODER_H

#include <png.h>

#include "common.h"

class WebpEncoder {
    int width, height;
    unsigned char *data;
    uint8_t *webp;
    int webp_len, mem_len;
    buffer_type buf_type; 

public:
    WebpEncoder(unsigned char *ddata, int width, int hheight, buffer_type bbuf_type);
    ~WebpEncoder();

    void encode();
    const uint8_t *get_webp() const;
    int get_webp_len() const;
};

#endif

