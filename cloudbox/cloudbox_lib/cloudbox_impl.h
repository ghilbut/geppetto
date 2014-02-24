#ifndef JS_COUDBOX_IMPL_H_
#define JS_COUDBOX_IMPL_H_

#include "http/http_server_delegate.h"
#include "http/http_server.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>



class CloudBox::Impl : public Http::ServerDelegate {
public:
    Impl(void);
    ~Impl(void);

    void RunShell(void);

    bool Start(const std::string& document_root, uint16_t port);
    void Stop(void);

private:
    virtual Http::Response& OnRequest(const Http::Request& request);
    virtual void OnMessage(const Http::Websocket& sock, const Http::Message& msg);
    virtual void OnError(const Http::Error& error);

private:
    v8::Isolate* isolate_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::thread thread_;
    Http::Server server_;
};

#endif  // JS_COUDBOX_IMPL_H_