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
    static void SetStatusCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetResponseHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Send(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    ResponseTemplate(void) {}
};

}

#endif  // HTTP_RESPONSE_TEMPLATE_H_