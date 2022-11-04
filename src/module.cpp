#include <node.h>

#include "webp.h"
#include "dynamic_webp_stack.h"

extern "C" void
init(v8::Handle<v8::Object> target)
{
    v8::HandleScope scope;

    Webp::Initialize(target);
    DynamicWebpStack::Initialize(target);
}

NODE_MODULE(binding, init);
