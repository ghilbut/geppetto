#ifndef HTTP_RESPONSE_TEMPLATE_H_
#define HTTP_RESPONSE_TEMPLATE_H_

#include <v8.h>


namespace Http {

class Response;

class ResponseTemplate {
public:
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    template<typename T>
    static Response* Unwrap(T _t);
    static void GetStatusCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetStatusCode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    //static void GetData(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    //static void SetData(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

private:
    static v8::Persistent<v8::FunctionTemplate> template_;
    ResponseTemplate(void) {}
};

}

#endif  // HTTP_RESPONSE_TEMPLATE_H_