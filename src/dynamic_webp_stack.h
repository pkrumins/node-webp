#ifndef DYNAMIC_WEBP_STACK_H
#define DYNAMIC_WEBP_STACK_H

#include <node.h>
#include <node_buffer.h>

#include <utility>
#include <vector>

#include <cstdlib>

#include "common.h"

class DynamicWebpStack : public node::ObjectWrap {
    struct Webp {
        int len, x, y, w, h;
        unsigned char *data;

        Webp(unsigned char *ddata, int llen, int xx, int yy, int ww, int hh) :
            len(llen), x(xx), y(yy), w(ww), h(hh)
        {
            data = (unsigned char *)malloc(sizeof(*data)*len);
            if (!data) throw "malloc failed in DynamicWebpStack::Webp::Webp";
            memcpy(data, ddata, len);
        }

        ~Webp() {
            free(data);
        }
    };

    typedef std::vector<Webp *> vWebp;
    typedef vWebp::iterator vWebpi;
    vWebp webp_stack;
    Point offset;
    int width, height;
    buffer_type buf_type;

    std::pair<Point, Point> optimal_dimension();

    void construct_webp_data(unsigned char *data, Point &top);

public:
    static void Initialize(v8::Handle<v8::Object> target);
    DynamicWebpStack(buffer_type bbuf_type);
    ~DynamicWebpStack();

    v8::Handle<v8::Value> Push(unsigned char *buf_data, size_t buf_len, int x, int y, int w, int h);
    v8::Handle<v8::Value> Dimensions();
    v8::Handle<v8::Value> WebpEncodeSync();

    static v8::Handle<v8::Value> New(const v8::Arguments &args);
    static v8::Handle<v8::Value> Push(const v8::Arguments &args);
    static v8::Handle<v8::Value> Dimensions(const v8::Arguments &args);
    static v8::Handle<v8::Value> WebpEncodeSync(const v8::Arguments &args);
};

#endif

