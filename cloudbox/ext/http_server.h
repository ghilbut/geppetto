#ifndef EXT_HTTP_SERVER_H_
#define EXT_HTTP_SERVER_H_

#include <string>
#include <stdint.h>


namespace Ext {
namespace Http {

class ServerDelegate;

class Server {
public:
    Server(void);
    ~Server(void);

    bool Listen(const std::string& document_root, uint16_t port);
    void Close(void);

    ServerDelegate* BindDelegate(ServerDelegate* delegate);
    ServerDelegate* UnbindDelegate(void);

private:
    class Impl;
    Impl* pimpl_;
};

}  // namespace Http
}  // namespace Ext

#endif  // EXT_HTTP_SERVER_H_