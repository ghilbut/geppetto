#ifndef FAKE_BROWSER_H_
#define FAKE_BROWSER_H_

#include "codebase/boost_lib_fwd.h"
#include <map>
#include <vector>
#include <string>


class Request {
protected:
    Request(const std::string& method, const std::string& uri);
    virtual ~Request(void) {}

public:
    void SetHeader(const std::string& key, const std::string& value);

    virtual bool MakeProtocol(std::string& protocol) const;

    // TODO(ghilbut): make websocket API

    const char* mothod(void) const;
    const char* uri(void) const;
    const char* host(void) const;
    void set_host(const std::string& host);
    const char* referer(void) const;
    void set_referer(const std::string& referer);


protected:
    const std::string method_;
    const std::string uri_;
    std::string host_;
    std::string referer_;
    std::map<std::string, std::string> headers_;
    mutable std::vector<char> data_;
};

class RequestGet : public Request {
public:
    RequestGet(const std::string& uri);
    ~RequestGet(void) {}

    void SetParam(const std::string& key, const std::string& value);

    virtual bool MakeProtocol(std::string& protocol) const;

private:
    std::map<std::string, std::string> params_;
};

class RequestPost : public Request {
public:
    RequestPost(const std::string& uri);
    ~RequestPost(void) {}

    template<class _Itr>
    void SetData(_Itr begin, _Itr end) {
        data_.swap(std::vector<char>(begin, end));
    }
};

class Response {
};


class FakeBrowser {
public:
    FakeBrowser(void);
    ~FakeBrowser(void);

    bool Connect(const char* host, const char* port);
    void Disconnect(void);
    bool Send(const Request& request);


private:
    IOService io_service_;
    boost::thread thread_;
    TCP::socket socket_;
    Response response_;
};

#endif  // FAKE_BROWSER_H_