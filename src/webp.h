#ifndef NODE_WEBP_H
#define NODE_WEBP_H

#include <node.h>
#include <node_buffer.h>

#include "common.h"

class Webp : public node::ObjectWrap {
    int width;
    int height;
    buffer_type buf_type;

public:
    static void Initialize(v8::Handle<v8::Object> target);
    Webp(int wwidth, int hheight, buffer_type bbuf_type);
    v8::Handle<v8::Value> WebpEncodeSync();

    static v8::Handle<v8::Value> New(const v8::Arguments &args);
    static v8::Handle<v8::Value> WebpEncodeSync(const v8::Arguments &args);
};

#endif

