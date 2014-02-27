#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <v8.h>
#include <boost/asio.hpp>


struct mg_connection;

namespace Http {

class Request {
public:
    static v8::Handle<v8::Object> New(v8::Isolate* isolate, struct mg_connection* conn = 0);
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    const char* method(void) const;
    void set_method(const std::string& method);


private:
    Request(v8::Isolate* isolate);
    ~Request(void);


    class Wrapper {
    public:
        static v8::Handle<v8::Object> New(v8::Isolate* isolate, struct mg_connection* conn = 0);
        static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

        template<typename T>
        static Request* Unwrap(T _t);
        static void GetMethod(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetMethod(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    private:
        Wrapper(void) {}

    private:
        static v8::Persistent<v8::ObjectTemplate> template_;
    };



private:
    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> object_;

    std::string method_;
};

}  // namespace Http

#endif  // HTTP_REQUEST_H_