#include <gmock/gmock.h>
#include "http/http_server.h"


extern std::string kStoragePath;

class HttpServerTest : public testing::Test {
public:
    HttpServerTest(void) {
        server_.Start(kStoragePath, 80);
    }

    ~HttpServerTest(void) {
        server_.Stop();
    }

protected:
    virtual void SetUp(void) {
    }

    virtual void TearDown(void) {
    }

public:
    Http::Server server_;
};


TEST_F(HttpServerTest, empty) {
}