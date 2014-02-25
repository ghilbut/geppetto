#ifndef JS_HTTP_SERVER_H_
#define JS_HTTP_SERVER_H_

#include "ext/http_server_delegate.h"
#include "ext/http_server.h"
#include <v8.h>


namespace Ext {
namespace Http {
class Websocket;
class Request;
class Response;
class Message;
class Error;
}
}


namespace JS {
namespace Http {

class Server : public Ext::Http::ServerDelegate {
public:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    Ext::Http::Server server(void);

private:
    Server(void);
    ~Server(void);

    static void Listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);

    //static Server* Unwrap(v8::Handle<v8::Object> object);
    static void GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    void FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
    void FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args);


    virtual Ext::Http::Response& OnRequest(const Ext::Http::Request& request);
    virtual void OnMessage(const Ext::Http::Websocket& sock, const Ext::Http::Message& msg);
    virtual void OnError(const Ext::Http::Error& error);


private:
    static v8::Persistent<v8::ObjectTemplate> template_;
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_message_;
    Ext::Http::Server server_;
};

}  // namespace Http
}  // namespace JS

#endif  // JS_HTTP_H_