#ifndef JS_HTTP_H_
#define JS_HTTP_H_

#include <v8.h>


namespace JS {

class Http {
public:
    static void Bind(v8::Handle<v8::Context> context);

private:
    Http(void) { }
    ~Http(void) {
        //onload_.Dispose();
    }

    static v8::Handle<v8::ObjectTemplate> MakeObjectTemplate(v8::Handle<v8::Context> context);

    static Http* Unwrap(v8::Handle<v8::Object> object);

    static void GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    void FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
    void FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_message_;
};





}  // namespace JS

#endif  // JS_HTTP_H_