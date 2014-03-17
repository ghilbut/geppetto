#include "http_response.h"
#include "http_response_template.h"


namespace Http {

v8::Persistent<v8::FunctionTemplate> ResponseTemplate::template_;

v8::Local<v8::FunctionTemplate> ResponseTemplate::Get(v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> ft;
    if (template_.IsEmpty()) {
        ft = v8::FunctionTemplate::New(isolate, Constructor);
        //ft->Set(isolate, "listen", v8::FunctionTemplate::New(isolate, ResponseTemplate::Listen));
        //ft->Set(isolate, "close", v8::FunctionTemplate::New(isolate, ResponseTemplate::Close));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        //ot->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, ResponseTemplate::Listen));
        //ot->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, ResponseTemplate::Close));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "statusCode"), ResponseTemplate::GetStatusCode, ResponseTemplate::SetStatusCode);
        //ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), ResponseTemplate::GetEventMessage, ResponseTemplate::SetEventMessage);
        ot->SetInternalFieldCount(1);

        template_.Reset(isolate, ft);
    } else {
        ft = v8::Local<v8::FunctionTemplate>::New(isolate, template_);
    }
    return ft;
}

void ResponseTemplate::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        const char* msg = "Failed to construct 'http.Response': Please use the 'new' operator, this DOM object constructor cannot be called as a function.";
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    Response* response = new Response();
    v8::Local<v8::Object> object = args.Holder();
    object->SetInternalField(0, v8::External::New(isolate, response));
    args.GetReturnValue().Set(object);
}

template<typename T>
Response* ResponseTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Response*>(ptr);
}

void ResponseTemplate::GetStatusCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(Unwrap(info)->GetStatusCode());
}

void ResponseTemplate::SetStatusCode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsNumber()) {
        v8::Handle<v8::Number> code = v8::Handle<v8::Number>::Cast(value);
        Unwrap(info)->SetStatusCode(code->Int32Value());
    } else {
        // TODO(ghilbut): throw js exception
    }
}

}  // namespace Http