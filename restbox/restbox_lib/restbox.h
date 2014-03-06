#ifndef JS_RESTBOX_H_
#define JS_RESTBOX_H_

#include <string>
#include <stdint.h>


class Storage;

class RestBox {
public:
    RestBox(const Storage& storage);
    ~RestBox(void);

    void RunShell(void);

private:
    class Impl;
    Impl* pimpl_;
};

#endif  // JS_RESTBOX_H_