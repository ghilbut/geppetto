#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>
#include <string>


struct mg_connection;

namespace Http {

class Response;

class Request {    
public:
    explicit Request(struct mg_connection* conn);
    ~Request(void);

    static void WeakCallback(const v8::WeakCallbackData<v8::Object, Request>& data);
    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);
    void ClearWeak(void);

    void Wait(void) const;
    void Notify(void) const;

    struct mg_connection* conn(void) const;

private:
    friend class RequestTemplate;

    struct mg_connection* conn_;

    const std::string method_;
    const std::string uri_;
    const std::string http_version_;
    const std::string query_string_;

    const std::string remote_ip_;
    const std::string local_ip_;
    const unsigned short remote_port_;
    const unsigned short local_port_;

    std::map<std::string, std::string> headers_;
    const std::string content_;

    mutable boost::mutex mutex_;
    mutable boost::condition_variable wait_;

    v8::Persistent<v8::Object> self_;
};

}  // namespace Http

#endif  // HTTP_REQUEST_H_