#ifndef JS_CLOUDBOX_H_
#define JS_CLOUDBOX_H_

#include <string>
#include <stdint.h>


class CloudBox {
public:
    CloudBox(void);
    ~CloudBox(void);

    bool Start(const std::string& document_root, uint16_t port);
    void RunShell(void);
    void Stop(void);

private:
    class Impl;
    Impl* pimpl_;
};

#endif  // JS_CLOUDBOX_H_