#ifndef CLOUD_BOX_H_
#define CLOUD_BOX_H_

#include "http/http_server_delegate.h"
#include "http/http_server.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace Http;


class CloudBox : public ServerDelegate {
public:
    CloudBox(void);
    ~CloudBox(void);

    bool Start(const std::string& document_root, uint16_t port);
    void Stop(void);

    void RunShell(void);

private:
    virtual Response& OnRequest(const Request& request);
    virtual void OnMessage(const Websocket& sock, const Message& msg);
    virtual void OnError(const Error& error);

private:
    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::thread thread_;

    JS::Engine js_engine_;

    Server server_;
};

#endif  // CLOUD_BOX_H_