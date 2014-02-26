#ifndef JS_COUDBOX_IMPL_H_
#define JS_COUDBOX_IMPL_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>


namespace Ext {
namespace Http {
class Server;
}
}

namespace JS {
namespace Http {
class Server;
}
}

class CloudBox::Impl {
public:
    Impl(void);
    ~Impl(void);

    void RunShell(void);

private:
    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::thread thread_;

    v8::Isolate* isolate_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;
};

#endif  // JS_COUDBOX_IMPL_H_