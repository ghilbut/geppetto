#ifndef HTTP_SERVER_WRAPPER_H_
#define HTTP_SERVER_WRAPPER_H_

#include <v8.h>


namespace Http {

class Server;

class ServerTemplate {
public:
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    template<typename T>
    static Server* Unwrap(T _t);
    static void GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEventError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    static void Listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    static v8::Persistent<v8::FunctionTemplate> template_;
    ServerTemplate(void) {}
};

}  // namespace Http

#endif  // HTTP_SERVER_WRAPPER_H_