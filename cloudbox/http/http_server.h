#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <string>
#include <stdint.h>


namespace Http {

class Server {
public:
    Server(void);
    ~Server(void);

    bool Start(const std::string& document_root, uint16_t port);
    void Stop(void);


private:
    class Impl;
    Impl* pimpl_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_