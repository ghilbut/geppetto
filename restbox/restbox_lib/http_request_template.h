#ifndef HTTP_REQUEST_TEMPLATE_H_
#define HTTP_REQUEST_TEMPLATE_H_

#include <v8.h>


namespace Http {

class Request;

class RequestTemplate {
public:
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);
    static v8::Local<v8::Object> NewInstance(v8::Isolate* isolate, struct mg_connection* conn);
    static v8::Local<v8::Object> NewInstance(v8::Isolate* isolate, Request* req);

    static void HeaderGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void HeaderGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info);

private:
    template<typename T>
    static Request* Unwrap(T _t);
    static void GetMethod(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetUri(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetHttpVersion(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetQueryString(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetRemoteIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetLocalIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetRemotePort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetLocalPort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    //static void GetHeaders(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetContent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

private:
    static v8::Persistent<v8::FunctionTemplate> template_;
    static v8::Persistent<v8::ObjectTemplate> headers_t_;
    RequestTemplate(void) {}
};

}  // namespace Http

#endif  // HTTP_REQUEST_TEMPLATE_H_