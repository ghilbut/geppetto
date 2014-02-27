#include "http_request.h"
#include <mongoose.h>


namespace Http {

v8::Handle<v8::Object> Request::New(v8::Isolate* isolate, struct mg_connection* conn) {
    return Wrapper::New(isolate, conn);
}

void Request::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Wrapper::Constructor(args);
}

const char* Request::method(void) const {
    return method_.c_str();
}

void Request::set_method(const std::string& method) {
    method_ = method;
}



Request::Request(v8::Isolate* isolate)
    : isolate_(isolate)
    , method_("GET") {
    // nothing
}

Request::~Request(void) {
    //onload_.Dispose();
}





v8::Persistent<v8::ObjectTemplate> Request::Wrapper::template_;

v8::Handle<v8::Object> Request::Wrapper::New(v8::Isolate* isolate, struct mg_connection* conn) {
    //v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::ObjectTemplate> object_t;
    if (template_.IsEmpty()) {
        object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        //object_t->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Listen));
        //object_t->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Close));
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "method"), Request::Wrapper::GetMethod, Request::Wrapper::SetMethod);
        //object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), Server::Wrapper::GetEventMessage, Server::Wrapper::SetEventMessage);
        template_.Reset(isolate, object_t);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, template_);
    }

    boost::asio::io_service* io_service = static_cast<boost::asio::io_service*>(isolate->GetData(0));

    Request* request = new Request(isolate);
    if (conn) {
        request->set_method(conn->request_method);
    }
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, request));
    request->object_.Reset(isolate, object);

    return object;
}

void Request::Wrapper::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        args.GetReturnValue().Set(e);
        return;
    }

    if (args.Length() > 0) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with any parameter(s)."));
        args.GetReturnValue().Set(e);
        return;
    }

    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::Object> object = New(isolate);
    args.GetReturnValue().Set(object);
}

template<typename T>
Request* Request::Wrapper::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Request*>(ptr);
}

void Request::Wrapper::GetMethod(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), r->method_.c_str()));
}

void Request::Wrapper::SetMethod(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsString()) {
        std::string& m = Unwrap(info)->method_;
        v8::Handle<v8::String> method = v8::Handle<v8::String>::Cast(value);

        const int len = method->Utf8Length();
        m.resize(len);
        method->WriteUtf8(&m[0], len);
    } else {
        // TODO(ghilbut): throw js exception
    }
}


}  // namespace Http