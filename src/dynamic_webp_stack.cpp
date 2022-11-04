#include "webp_encoder.h"
#include "dynamic_webp_stack.h"
#include "buffer_compat.h"

using namespace v8;
using namespace node;

std::pair<Point, Point>
DynamicWebpStack::optimal_dimension()
{
    Point top(-1, -1), bottom(-1, -1);
    for (vWebpi it = webp_stack.begin(); it != webp_stack.end(); ++it) {
        Webp *webp = *it;
        if (top.x == -1 || webp->x < top.x)
            top.x = webp->x;
        if (top.y == -1 || webp->y < top.y)
            top.y = webp->y;
        if (bottom.x == -1 || webp->x + webp->w > bottom.x)
            bottom.x = webp->x + webp->w;
        if (bottom.y == -1 || webp->y + webp->h > bottom.y)
            bottom.y = webp->y + webp->h;
    }

    /*
    printf("top    x, y: %d, %d\n", top.x, top.y);
    printf("bottom x, y: %d, %d\n", bottom.x, bottom.y);
    */

    return std::make_pair(top, bottom);
}

void
DynamicWebpStack::construct_webp_data(unsigned char *data, Point &top)
{
    for (vWebpi it = webp_stack.begin(); it != webp_stack.end(); ++it) {
        Webp *webp = *it;
        int start = (webp->y - top.y)*width*4 + (webp->x - top.x)*4;
        unsigned char *webpdatap = webp->data;
        for (int i = 0; i < webp->h; i++) {
            unsigned char *datap = &data[start + i*width*4];
            for (int j = 0; j < webp->w; j++) {
                *datap++ = *webpdatap++;
                *datap++ = *webpdatap++;
                *datap++ = *webpdatap++;
                // skip check for RGB/BGR as we're only working with BGRA for now
                //*datap++ = (buf_type == BUF_RGB || buf_type == BUF_BGR) ? 0x00 : *webpdatap++;
                *datap++ = ~*webpdatap++;
            }
        }
    }
}

void
DynamicWebpStack::Initialize(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(t, "push", Push);
    NODE_SET_PROTOTYPE_METHOD(t, "encodeSync", WebpEncodeSync);
    NODE_SET_PROTOTYPE_METHOD(t, "dimensions", Dimensions);
    target->Set(String::NewSymbol("DynamicWebpStack"), t->GetFunction());
}

DynamicWebpStack::DynamicWebpStack(buffer_type bbuf_type) :
    buf_type(bbuf_type) {}

DynamicWebpStack::~DynamicWebpStack()
{
    for (vWebpi it = webp_stack.begin(); it != webp_stack.end(); ++it)
        delete *it;
}

Handle<Value>
DynamicWebpStack::Push(unsigned char *buf_data, size_t buf_len, int x, int y, int w, int h)
{
    try {
        Webp *webp = new Webp(buf_data, buf_len, x, y, w, h);
        webp_stack.push_back(webp);
        return Undefined();
    }
    catch (const char *e) {
        return VException(e);
    }
}

Handle<Value>
DynamicWebpStack::WebpEncodeSync()
{
    HandleScope scope;

    std::pair<Point, Point> optimal = optimal_dimension();
    Point top = optimal.first, bot = optimal.second;

    // printf("width, height: %d, %d\n", bot.x - top.x, bot.y - top.y);

    offset = top;
    width = bot.x - top.x;
    height = bot.y - top.y;

    unsigned char *data = (unsigned char*)malloc(sizeof(*data) * width * height * 4);
    if (!data) return VException("malloc failed in DynamicWebpStack::WebpEncode");
    memset(data, 0x00, width*height*4); // alpha is 0x00 for webp

    construct_webp_data(data, top);

    buffer_type pbt = (buf_type == BUF_BGR || buf_type == BUF_BGRA) ? BUF_BGRA : BUF_RGBA;

    try {
        WebpEncoder encoder(data, width, height, pbt);
        encoder.encode();
        free(data);
        int webp_len = encoder.get_webp_len();
        Buffer *retbuf = Buffer::New(webp_len);
        memcpy(BufferData(retbuf), encoder.get_webp(), webp_len);
        return scope.Close(retbuf->handle_);
    }
    catch (const char *err) {
        return VException(err);
    }
}

Handle<Value>
DynamicWebpStack::Dimensions()
{
    HandleScope scope;

    Local<Object> dim = Object::New();
    dim->Set(String::NewSymbol("x"), Integer::New(offset.x));
    dim->Set(String::NewSymbol("y"), Integer::New(offset.y));
    dim->Set(String::NewSymbol("width"), Integer::New(width));
    dim->Set(String::NewSymbol("height"), Integer::New(height));

    return scope.Close(dim);
}

Handle<Value>
DynamicWebpStack::New(const Arguments &args)
{
    HandleScope scope;

    buffer_type buf_type = BUF_RGB;
    if (args.Length() == 1) {
        if (!args[0]->IsString())
            return VException("First argument must be 'rgb', 'bgr', 'rgba' or 'bgra'.");

        String::AsciiValue bts(args[0]->ToString());
        if (!(str_eq(*bts, "rgb") || str_eq(*bts, "bgr") ||
            str_eq(*bts, "rgba") || str_eq(*bts, "bgra")))
        {
            return VException("First argument must be 'rgb', 'bgr', 'rgba' or 'bgra'.");
        }
        
        if (str_eq(*bts, "rgb"))
            buf_type = BUF_RGB;
        else if (str_eq(*bts, "bgr"))
            buf_type = BUF_BGR;
        else if (str_eq(*bts, "rgba"))
            buf_type = BUF_RGBA;
        else if (str_eq(*bts, "bgra"))
            buf_type = BUF_BGRA;
        else
            return VException("First argument wasn't 'rgb', 'bgr', 'rgba' or 'bgra'.");
    }

    DynamicWebpStack *webp_stack = new DynamicWebpStack(buf_type);
    webp_stack->Wrap(args.This());
    return args.This();
}

Handle<Value>
DynamicWebpStack::Push(const Arguments &args)
{
    HandleScope scope;

    if (!Buffer::HasInstance(args[0]))
        return VException("First argument must be Buffer.");
    if (!args[1]->IsInt32())
        return VException("Second argument must be integer x.");
    if (!args[2]->IsInt32())
        return VException("Third argument must be integer y.");
    if (!args[3]->IsInt32())
        return VException("Fourth argument must be integer w.");
    if (!args[4]->IsInt32())
        return VException("Fifth argument must be integer h.");

    int x = args[1]->Int32Value();
    int y = args[2]->Int32Value();
    int w = args[3]->Int32Value();
    int h = args[4]->Int32Value();

    if (x < 0)
        return VException("Coordinate x smaller than 0.");
    if (y < 0)
        return VException("Coordinate y smaller than 0.");
    if (w < 0)
        return VException("Width smaller than 0.");
    if (h < 0)
        return VException("Height smaller than 0.");

    DynamicWebpStack *webp_stack = ObjectWrap::Unwrap<DynamicWebpStack>(args.This());

    Local<Object> buf_obj = args[0]->ToObject();
    char *buf_data = BufferData(buf_obj);
    size_t buf_len = BufferLength(buf_obj);

    return scope.Close(webp_stack->Push((unsigned char*)buf_data, buf_len, x, y, w, h));
}

Handle<Value>
DynamicWebpStack::Dimensions(const Arguments &args)
{
    HandleScope scope;

    DynamicWebpStack *webp_stack = ObjectWrap::Unwrap<DynamicWebpStack>(args.This());
    return scope.Close(webp_stack->Dimensions());
}

Handle<Value>
DynamicWebpStack::WebpEncodeSync(const Arguments &args)
{
    HandleScope scope;

    DynamicWebpStack *webp_stack = ObjectWrap::Unwrap<DynamicWebpStack>(args.This());
    return scope.Close(webp_stack->WebpEncodeSync());
}

