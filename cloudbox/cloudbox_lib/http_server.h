#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <v8.h>
#include <boost/asio.hpp>


struct mg_server;
struct mg_connection;

namespace Http {

class Server {
public:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);

    bool DoListen(const std::string& document_root, uint16_t port);
    void DoClose(void);

    void FireRequest(struct mg_connection *conn);
    void FireMessage(struct mg_connection *conn);
    void FireError(void);

private:
    Server(v8::Isolate* isolate, boost::asio::io_service& io_service);
    ~Server(void);

    void handle_listen(const std::string& document_root, uint16_t port, boost::function<void(const bool&)> ret_setter);
    void handle_close(void);

    void handle_request(struct mg_connection *conn, boost::function<void(const bool&)> ret_setter);
    void handle_message(struct mg_connection *conn);
    void handle_error(void);

    class Base;
    class Wrapper;

private:
    Base* base_;

    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> object_;
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_message_;
    v8::Persistent<v8::Function> on_error_;

    boost::asio::io_service& io_service_;
    boost::asio::strand strand_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_