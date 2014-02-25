#ifndef EXT_HTTP_SERVER_IMPL_H_
#define EXT_HTTP_SERVER_IMPL_H_

#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <string>
#include <stdint.h>


struct mg_server;
struct mg_connection;

namespace Ext {
namespace Http {

class ServerDelegate;

class Server::Impl : public boost::noncopyable {
    static void* thread_f(void*);

public:
    Impl(void);
    ~Impl(void) {}

    bool Listen(const std::string& document_root, uint16_t port);
    void Close(void);

    ServerDelegate* BindDelegate(ServerDelegate* delegate);
    ServerDelegate* UnbindDelegate(void);

    int FireOnRequest(mg_connection *conn);
    int FireOnMessage(mg_connection *conn);


public:
    static int request_handler(struct mg_connection *conn);
    void thread_main(void);

private:
    ServerDelegate* delegate_;
    mg_server* server_;

    boost::atomic_bool is_alive_;
    boost::atomic_bool is_stop_;
    boost::thread thread_;
};

}  // namespace Http
}  // namespace Ext


#endif  // EXT_HTTP_SERVER_IMPL_H_