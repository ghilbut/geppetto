#include "fake_browser.h"
#include "mongoose_mock.h"
#include <mongoose/mongoose.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>


using ::testing::_;
using ::testing::WithArgs;
using ::testing::Invoke;

extern std::string kStoragePath;

class FakeBrowserTest : public testing::Test {
public:
    FakeBrowserTest(void)
        : mock_("", 80) {
        // nothing
    }

    ~FakeBrowserTest(void) {}

protected:
    virtual void SetUp(void) {
        browser_.Connect("127.0.0.1", "80");
    }

    virtual void TearDown(void) {
        browser_.Disconnect();
    }

public:
    FakeBrowser browser_;
    MongooseMock mock_;
};



ACTION_P2(DoBeginGetRequest, headers, params) {

    struct mg_connection* conn = arg0;
    struct mg_request_info* ri = mg_get_request_info(conn);

    const char* method = ri->request_method;
    EXPECT_STREQ("GET", method);

    const char* const uri  = ri->uri;
    const char* const data = ri->query_string;
    const size_t data_len = strlen(data);

    std::map<std::string, std::string>::const_iterator itr = headers.begin();
    std::map<std::string, std::string>::const_iterator end = headers.end();
    for (; itr != end; ++itr) {
        const char* value = mg_get_header(conn, (itr->first).c_str());
        EXPECT_STREQ((itr->second).c_str(), value);
    }

    itr = params.begin();
    end = params.end();
    for (; itr != end; ++itr) {
        char param[256];
        const int param_len = mg_get_var(data, data_len, (itr->first).c_str(), param, sizeof(param));
        EXPECT_GT(param_len, 0);
        EXPECT_STREQ((itr->second).c_str(), param);
    }

    return 0;
}

ACTION(DoEndGetRequest) {
    const struct mg_connection* conn = arg0;
    const int reply_status_code = arg1;
}

TEST_F(FakeBrowserTest, request_simple_get) {

    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;

    EXPECT_CALL(mock_, OnBeginRequest(_))
        .Times(1)
        .WillOnce(WithArgs<0>(DoBeginGetRequest(headers, params)));
    EXPECT_CALL(mock_, OnEndRequest(_, _))
        .Times(1)
        .WillOnce(WithArgs<0, 1>(DoEndGetRequest()));

    RequestGet r("/");
    r.set_host("localhost");
    EXPECT_TRUE(browser_.Send(r));
}

TEST_F(FakeBrowserTest, request_get_with_parameters) {

    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    params["a"] = "A";
    params["b"] = "B";
    params["c"] = "C";

    RequestGet r("/");
    r.set_host("localhost");
    std::map<std::string, std::string>::const_iterator itr = headers.begin();
    std::map<std::string, std::string>::const_iterator end = headers.end();
    for (; itr != end; ++itr) {
        r.SetHeader(itr->first, itr->second);
    }
    itr = params.begin();
    end = params.end();
    for (; itr != end; ++itr) {
        r.SetParam(itr->first, itr->second);
    }

    EXPECT_CALL(mock_, OnBeginRequest(_))
        .Times(1)
        .WillOnce(WithArgs<0>(DoBeginGetRequest(headers, params)));
    EXPECT_CALL(mock_, OnEndRequest(_, _))
        .Times(1)
        .WillOnce(WithArgs<0, 1>(DoEndGetRequest()));

    EXPECT_TRUE(browser_.Send(r));
}



ACTION_P2(DoBeginPostRequest, headers, data) {

    struct mg_connection* conn = arg0;
    struct mg_request_info* ri = mg_get_request_info(conn);

    const char* method = ri->request_method;
    EXPECT_STREQ("POST", method);

    const char* const uri  = ri->uri;

    std::map<std::string, std::string>::const_iterator itr = headers.begin();
    std::map<std::string, std::string>::const_iterator end = headers.end();
    for (; itr != end; ++itr) {
        const char* value = mg_get_header(conn, (itr->first).c_str());
        EXPECT_STREQ((itr->second).c_str(), value);
    }

    const size_t buf_len = data.size();
    char* const buf = new char[buf_len+1];
    const int read_bytes = mg_read(conn, buf, buf_len+1);
    EXPECT_EQ(buf_len, read_bytes);
    for (int index = 0; index < read_bytes; ++index) {
        EXPECT_EQ(data[index], buf[index]);
    }
    delete[] buf;
    
    return 0;
}

ACTION(DoEndPostRequest) {
    const struct mg_connection* conn = arg0;
    const int reply_status_code = arg1;
}

TEST_F(FakeBrowserTest, request_simple_post) {

    std::map<std::string, std::string> headers;
    std::vector<char> data;

    EXPECT_CALL(mock_, OnBeginRequest(_))
        .Times(1)
        .WillOnce(WithArgs<0>(DoBeginPostRequest(headers, data)));
    EXPECT_CALL(mock_, OnEndRequest(_, _))
        .Times(1)
        .WillOnce(WithArgs<0, 1>(DoEndPostRequest()));

    RequestPost r("/");
    r.set_host("localhost");
    EXPECT_TRUE(browser_.Send(r));
}

TEST_F(FakeBrowserTest, request_post_with_data) {

    std::map<std::string, std::string> headers;

    boost::uuids::random_generator gen;
    const std::string uuid = to_string(gen());
    std::vector<char> data(uuid.begin(), uuid.end());

    EXPECT_CALL(mock_, OnBeginRequest(_))
        .Times(1)
        .WillOnce(WithArgs<0>(DoBeginPostRequest(headers, data)));
    EXPECT_CALL(mock_, OnEndRequest(_, _))
        .Times(1)
        .WillOnce(WithArgs<0, 1>(DoEndPostRequest()));

    RequestPost r("/");
    r.set_host("localhost");
    r.SetData(data.begin(), data.end());
    EXPECT_TRUE(browser_.Send(r));
}



// TODO(ghilbut): test websocket API