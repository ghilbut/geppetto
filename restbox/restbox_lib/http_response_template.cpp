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
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "statusCode"), GetStatusCode, SetStatusCode);
        ot->Set(isolate, "getHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::GetHeader));
        ot->Set(isolate, "setHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::SetHeader));
        ot->Set(isolate, "removeHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::RemoveHeader));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "data"), GetData, SetData);

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

    v8::Local<v8::Object> object = args.Holder();
    Response* response = new Response();
    response->MakeWeak(isolate, object);
    args.GetReturnValue().Set(object);
}

template<typename T>
Response* ResponseTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    return static_cast<Response*>(object->GetAlignedPointerFromInternalField(0));
}

void ResponseTemplate::GetStatusCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    const int status_code = Unwrap(info)->status_code();
    info.GetReturnValue().Set(status_code);
}

void ResponseTemplate::SetStatusCode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    if (!value->IsNumber()) {
        // TODO(ghilbut): throw exception
        return;
    }

    Unwrap(info)->set_status_code(value->Int32Value());
}

void ResponseTemplate::GetHeader(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::String> str = args[0]->ToString();

    const int len = str->Utf8Length();
    char* name = new char[len+1];
    name[str->WriteUtf8(name, len)] = '\0';
    const char* value = Unwrap(args)->GetHeader(name);
    delete[] name;

    if (value != 0) {
        args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, value));
    }
}

void ResponseTemplate::SetHeader(const v8::FunctionCallbackInfo<v8::Value>& args) {
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

    v8::Local<v8::String> str = args[0]->ToString();

    const int name_len = str->Utf8Length();
    char* name = new char[name_len+1];
    name[str->WriteUtf8(name, name_len)] = '\0';

    str = args[1]->ToString();

    const int value_len = str->Utf8Length();
    char* value = new char[value_len+1];
    value[str->WriteUtf8(value, value_len)] = '\0';

    Unwrap(args)->SetHeader(name, value);

    delete[] value;
    delete[] name;
}

void ResponseTemplate::RemoveHeader(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::String> str = args[0]->ToString();

    const int len = str->Utf8Length();
    char* name = new char[len+1];
    name[str->WriteUtf8(name, len)] = '\0';
    Unwrap(args)->RemoveHeader(name);
    delete[] name;
}

void ResponseTemplate::GetData(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Response* res = Unwrap(info);

    const std::string& data = res->data();
    v8::Local<v8::String> str = v8::String::NewFromUtf8(info.GetIsolate(), data.c_str());
    info.GetReturnValue().Set(str);
}

void ResponseTemplate::SetData(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Response* res = Unwrap(info);

    v8::Local<v8::String> str = value->ToString();

    const int data_len = str->Utf8Length();
    if (data_len > 0) {
        char* data = new char[data_len];
        str->WriteUtf8(data, data_len);
        res->set_data(data, data_len);
        delete[] data;    
    }
}

}  // namespace Http