#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <stdint.h>


struct mg_server;
struct mg_connection;

namespace Http {

class ServerBase;

class Server {
public:
    Server(v8::Isolate* isolate, boost::asio::io_service& io_service);
    ~Server(void);

    bool DoListen(uint16_t port);
    void DoClose(void);

    void set_self(v8::Isolate* isolate, v8::Handle<v8::Object> self);
    v8::Local<v8::Function> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger);
    v8::Local<v8::Function> message_trigger(v8::Isolate* isolate) const;
    void set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger);
    v8::Local<v8::Function> error_trigger(v8::Isolate* isolate) const;
    void set_error_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger);

private:
    void FireRequest(struct mg_connection *conn);
    void FireMessage(struct mg_connection *conn);
    void FireError(void);

    void handle_listen(uint16_t port, boost::function<void(const bool&)> ret_setter);
    void handle_close(void);

    void handle_request(struct mg_connection *conn, boost::function<void(const bool&)> ret_setter);
    void handle_message(struct mg_connection *conn, boost::function<void(const bool&)> ret_setter);
    void handle_error(void);

    static int request_handler(struct mg_connection *conn);
    void thread_main(void);

private:
    uint16_t port_;

    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_message_;
    v8::Persistent<v8::Function> on_error_;

    boost::asio::io_service& io_service_;
    boost::asio::strand strand_;
    boost::atomic_bool is_alive_;
    boost::atomic_bool is_stop_;
    boost::thread thread_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_