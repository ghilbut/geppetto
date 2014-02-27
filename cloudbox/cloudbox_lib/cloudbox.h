#ifndef JS_CLOUDBOX_H_
#define JS_CLOUDBOX_H_

#include <string>
#include <stdint.h>


class Storage;

class CloudBox {
public:
    CloudBox(const Storage& storage);
    ~CloudBox(void);

    void RunShell(void);

private:
    class Impl;
    Impl* pimpl_;
};

#endif  // JS_CLOUDBOX_H_