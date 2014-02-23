#ifndef HTTP_SERVER_IMPL_H_
#define HTTP_SERVER_IMPL_H_

#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <string>
#include <stdint.h>


struct mg_server;

namespace Http {

class Server::Impl : public boost::noncopyable {
public:
    Impl(void);
    ~Impl(void) {}

    bool Start(const std::string& document_root, uint16_t port);
    void Stop(void);


private:
    void thread_main(void);

private:
    boost::atomic<mg_server*> server_;
    boost::atomic<bool> is_stop_;
    boost::thread thread_;
};

}  // namespace Http


#endif  // HTTP_SERVER_IMPL_H_