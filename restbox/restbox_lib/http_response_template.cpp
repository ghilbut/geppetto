#include "http_response.h"
#include "http_response_template.h"
#include "http_request_template.h"


namespace Http {

static v8::Persistent<v8::FunctionTemplate> template_;

v8::Local<v8::FunctionTemplate> ResponseTemplate::Get(v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> ft;
    if (template_.IsEmpty()) {
        ft = v8::FunctionTemplate::New(isolate, Constructor);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Response"));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->SetInternalFieldCount(1);
        ot->Set(isolate, "setStatusCode", v8::FunctionTemplate::New(isolate, &Send));
        ot->Set(isolate, "setResponseHeader", v8::FunctionTemplate::New(isolate, &Send));
        ot->Set(isolate, "send", v8::FunctionTemplate::New(isolate, &Send));

        template_.Reset(isolate, ft);
    } else {
        ft = v8::Local<v8::FunctionTemplate>::New(isolate, template_);
    }
    return ft;
}

void ResponseTemplate::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (!args.IsConstructCall()) {
        const char* msg = "Failed to construct 'http.Response': Please use the 'new' operator, this DOM object constructor cannot be called as a function.";
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    if (args.Length() == 0) {
        // TODO(ghilbut): throw exception
        return;
    }


    if (!args[0]->IsObject()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::Object> obj = args[0]->ToObject();
    if (obj->GetConstructor() != RequestTemplate::Get(isolate)->GetFunction()) {
        // TODO(ghilbut): throw exception
        return;
    }

    Request* req = static_cast<Request*>(obj->GetAlignedPointerFromInternalField(0));

    v8::Local<v8::Object> object = args.Holder();
    Response* response = new Response(req);
    response->MakeWeak(isolate, object);
    args.GetReturnValue().Set(object);
}

template<typename T>
Response* ResponseTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    return static_cast<Response*>(object->GetAlignedPointerFromInternalField(0));
}

void ResponseTemplate::SetStatusCode(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() == 0) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsNumber()) {
        // TODO(ghilbut): throw exception
        return;
    }

    Unwrap(args)->SetStatusCode(args[0]->Int32Value());
}

void ResponseTemplate::SetResponseHeader(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() < 2) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsString() || !args[1]->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    char name[512];
    char value[512];
    name[args[0]->ToString()->WriteUtf8(name, 512)] = '\0';
    value[args[1]->ToString()->WriteUtf8(value, 512)] = '\0';

    Unwrap(args)->SetResponseHeader(name, value);
}

void ResponseTemplate::Send(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    const Response* res = Unwrap(args);

    if (args.Length() == 0) {
        res->Send(0, 0);
        return;
    }

    v8::Local<v8::String> data = args[0]->ToString();

    const int len = data->Utf8Length();
    char* buf = new char[len];
    data->WriteUtf8(buf, len);
    res->Send(buf, len);
    delete[] buf;
}

}  // namespace Http