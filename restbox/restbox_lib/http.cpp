#include "http.h"
#include "http_server_template.h"
#include "http_request_template.h"
#include "http_response_template.h"


namespace Http {

void NotConstruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::String> msg = v8::String::NewFromUtf8(isolate, "Http is singleton. use http.<something>.");
    v8::Local<v8::Value> e = v8::Exception::Error(msg);
    args.GetReturnValue().Set(isolate->ThrowException(e));
}


v8::Persistent<v8::FunctionTemplate> Template::template_;

v8::Local<v8::FunctionTemplate> Template::New(v8::Isolate* isolate) {
    return v8::FunctionTemplate::New(isolate, NotConstruct);
}

v8::Local<v8::FunctionTemplate> Template::Get(v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> ft;
    if (template_.IsEmpty()) {
        ft = v8::FunctionTemplate::New(isolate);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Http"));

        v8::Handle<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->Set(v8::String::NewFromUtf8(isolate, "Server"), ServerTemplate::Get(isolate));
        ot->Set(v8::String::NewFromUtf8(isolate, "Request"), RequestTemplate::Get(isolate));
        ot->Set(v8::String::NewFromUtf8(isolate, "Response"), ResponseTemplate::Get(isolate));
        ot->SetInternalFieldCount(1);

        template_.Reset(isolate, ft);
    } else {
        ft = v8::Local<v8::FunctionTemplate>::New(isolate, template_);
    }
    return ft;
}

v8::Local<v8::Object> NewInstance(v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> ft = Template::Get(isolate);
    v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    v8::Local<v8::Object> instance = ot->NewInstance();
    return instance;
}

}  // namespace Http