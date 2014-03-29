#include "http_request.h"
#include "http_request_template.h"
#include "http_response_template.h"
#include <mongoose.h>


namespace Http {

v8::Persistent<v8::FunctionTemplate> RequestTemplate::template_;
v8::Persistent<v8::ObjectTemplate> RequestTemplate::headers_t_;

v8::Local<v8::FunctionTemplate> RequestTemplate::Get(v8::Isolate* isolate) {
    
    v8::Local<v8::FunctionTemplate> ft;
    if (template_.IsEmpty()) {
        ft = v8::FunctionTemplate::New(isolate);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Request"));

        v8::Handle<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "request_method")
            , RequestTemplate::GetMethod
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "uri")
            , RequestTemplate::GetUri
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "http_version")
            , RequestTemplate::GetHttpVersion
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "query_string")
            , RequestTemplate::GetQueryString
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "remote_ip")
            , RequestTemplate::GetRemoteIP
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "local_ip")
            , RequestTemplate::GetLocalIP
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "remote_port")
            , RequestTemplate::GetRemotePort
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "local_port")
            , RequestTemplate::GetLocalPort
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "content")
            , RequestTemplate::GetContent
            , 0
            , v8::Handle<v8::Value>()
            , static_cast<v8::AccessControl>(v8::PROHIBITS_OVERWRITING));
        ot->SetInternalFieldCount(1);

        template_.Reset(isolate, ft);
    } else {
        ft = v8::Local<v8::FunctionTemplate>::New(isolate, template_);
    }

    return ft;
}

v8::Local<v8::Object> RequestTemplate::NewInstance(v8::Isolate* isolate, struct mg_connection* conn) {
    v8::Local<v8::FunctionTemplate> ft = RequestTemplate::Get(isolate);
    v8::Local<v8::Function> f = ft->GetFunction();
    v8::Local<v8::Object> i = f->NewInstance();

    v8::Local<v8::ObjectTemplate> headers_t;
    if (headers_t_.IsEmpty()) {
        headers_t = v8::ObjectTemplate::New(isolate);
        headers_t->SetNamedPropertyHandler(RequestTemplate::HeaderGetter);
        headers_t->SetIndexedPropertyHandler(RequestTemplate::HeaderGetter);
        headers_t->SetInternalFieldCount(1);
        headers_t_.Reset(isolate, headers_t);
    } else {
        headers_t = v8::Local<v8::ObjectTemplate>::New(isolate, headers_t_);
    }

    Request* request = new Request(conn);

    v8::Local<v8::Object> headers = headers_t->NewInstance();
    headers->SetInternalField(0, v8::External::New(isolate, request));

    i->Set(v8::String::NewFromUtf8(isolate, "headers"), headers);
    i->SetInternalField(0, v8::External::New(isolate, request));
    return i;
}

v8::Local<v8::Object> RequestTemplate::NewInstance(v8::Isolate* isolate, Request* req) {

    v8::Local<v8::ObjectTemplate> headers_t;
    if (headers_t_.IsEmpty()) {
        headers_t = v8::ObjectTemplate::New(isolate);
        headers_t->SetNamedPropertyHandler(RequestTemplate::HeaderGetter);
        headers_t->SetIndexedPropertyHandler(RequestTemplate::HeaderGetter);
        headers_t->SetInternalFieldCount(1);
        headers_t_.Reset(isolate, headers_t);
    } else {
        headers_t = v8::Local<v8::ObjectTemplate>::New(isolate, headers_t_);
    }

    v8::Local<v8::Object> headers = headers_t->NewInstance();
    //headers->SetInternalField(0, v8::External::New(isolate, req));
    headers->SetAlignedPointerInInternalField(0, req);

    v8::Local<v8::FunctionTemplate> ft = RequestTemplate::Get(isolate);
    v8::Local<v8::Function> f = ft->GetFunction();
    v8::Local<v8::Object> i = f->NewInstance();
    i->Set(v8::String::NewFromUtf8(isolate, "headers"), headers);
    //i->SetInternalField(0, v8::External::New(isolate, req));
    req->MakeWeak(isolate, i);
    return i;
}

template<typename T>
Request* RequestTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<Request*>(ptr);
    return static_cast<Request*>(object->GetAlignedPointerFromInternalField(0));
}

void RequestTemplate::GetMethod(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->method_).c_str()));
}

void RequestTemplate::GetUri(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->uri_).c_str()));
}

void RequestTemplate::GetHttpVersion(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->http_version_).c_str()));
}

void RequestTemplate::GetQueryString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->query_string_).c_str()));
}

void RequestTemplate::GetRemoteIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->remote_ip_).c_str()));
}

void RequestTemplate::GetLocalIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), (r->local_ip_).c_str()));
}

void RequestTemplate::GetRemotePort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::Uint32::New(info.GetIsolate(), r->remote_port_));
}

void RequestTemplate::GetLocalPort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Request* r = Unwrap(info);
    info.GetReturnValue().Set(v8::Uint32::New(info.GetIsolate(), r->local_port_));
}

void RequestTemplate::HeaderGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Request* r = Unwrap(info);
    const std::map<std::string, std::string>& headers = r->headers_;

    v8::String::Utf8Value key(property);
    if (std::string("length").compare(*key) == 0) {
        info.GetReturnValue().Set(v8::Uint32::New(isolate, headers.size()));
        return;
    }

    std::map<std::string, std::string>::const_iterator itr = headers.find(*key);
    if (itr == headers.end()) {
        v8::Local<v8::Object> holder = info.Holder();
        v8::Local<v8::Value> real = holder->GetRealNamedProperty(v8::String::NewFromUtf8(isolate, *key));
        info.GetReturnValue().Set(real);
        return;
    }

    const std::string& value = itr->second;
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, value.c_str()));
}

void RequestTemplate::HeaderGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Request* r = Unwrap(info);
    const std::map<std::string, std::string>& headers = r->headers_;

    if (index >= headers.size()) {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
    }

    std::map<std::string, std::string>::const_iterator itr = headers.begin();
    for (int i = 0 ; i < index; ++i) {
        ++itr;
    }

    v8::Local<v8::Object> instance = v8::Object::New(isolate);
    instance->Set(v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, (itr->first).c_str()));
    instance->Set(v8::String::NewFromUtf8(isolate, "value"), v8::String::NewFromUtf8(isolate, (itr->second).c_str()));
    info.GetReturnValue().Set(instance);
}

void RequestTemplate::GetContent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    Request* r = Unwrap(info);
    const std::string& c = r->content_;
    if (c.empty()) {
        info.GetReturnValue().Set(v8::Null(isolate));
    } else {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, c.c_str()));
    }
}

}  // namespace Http