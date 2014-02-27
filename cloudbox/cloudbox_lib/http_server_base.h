#ifndef HTTP_SERVER_BASE_H_
#define HTTP_SERVER_BASE_H_

//#include "http_server.h"
#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <string>
#include <stdint.h>


struct mg_connection;

namespace Http {

class Server::Base {
public:
    typedef boost::function<void(struct mg_connection*)> TriggerRequest;
    typedef boost::function<void(struct mg_connection*)> TriggerMessage;

    Base(void);
    ~Base(void);

    bool Listen(const std::string& document_root, uint16_t port);
    void Close(void);

    void BindRequest(TriggerRequest trigger);
    void BindMessage(TriggerRequest trigger);

private:
    static int request_handler(struct mg_connection *conn);
    void thread_main(void);

private:
    std::string document_root_;
    uint16_t port_;

    TriggerRequest trigger_request_;
    TriggerMessage trigger_message_;

    boost::atomic_bool is_alive_;
    boost::atomic_bool is_stop_;
    boost::thread thread_;
};

}  // namespace Http

#endif  // HTTP_SERVER_BASE_H_