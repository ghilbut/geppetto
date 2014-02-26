#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>



namespace Ext {
    namespace Http {
        class Websocket;
        class Request;
        class Response;
        class Message;
        class Error;
    }
}



struct mg_server;
struct mg_connection;

namespace Http {

class Server {
public:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    void DoListen(const std::string& document_root, uint16_t port);
    void DoClose(void);

    void FireRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
    void FireMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    void FireError(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    Server(boost::asio::io_service& io_service);
    ~Server(void);

    void handle_listen(const std::string& document_root, uint16_t port);
    void handle_close(void);

    void handle_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    void handle_message(const v8::FunctionCallbackInfo<v8::Value>& args);
    void handle_error(const v8::FunctionCallbackInfo<v8::Value>& args);


    class Base {
    public:
        Base(Server& server);
        ~Base(void);

        bool Listen(const std::string& document_root, uint16_t port);
        void Close(void);

    private:
        static int request_handler(struct mg_connection *conn);
        void thread_main(void);

    private:
        Server& server_;

        std::string document_root_;
        uint16_t port_;

        boost::atomic_bool is_alive_;
        boost::atomic_bool is_stop_;
        boost::thread thread_;
    };


    class Wrapper {
    public:
        static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

        template<typename T>
        static Server* Unwrap(T _t);
        static void GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
        static void GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

        static void Listen(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);

    private:
        Wrapper(void) {}

    private:
        static v8::Persistent<v8::ObjectTemplate> template_;
    };


private:
    Base base_;
    boost::asio::io_service& io_service_;
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_message_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_