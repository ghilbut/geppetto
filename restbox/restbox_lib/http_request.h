#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <map>
#include <string>


struct mg_connection;

namespace Http {

class Server;

class Request {    
public:
    static v8::Local<v8::Object> NewRecvRequest(v8::Isolate* isolate, struct mg_connection* conn);

    explicit Request(struct mg_connection* conn);
    ~Request(void);

private:
    friend class RequestTemplate;

    std::string method_;
    std::string uri_;
    std::string http_version_;
    std::string query_string_;

    std::string remote_ip_;
    std::string local_ip_;
    unsigned short remote_port_;
    unsigned short local_port_;

    std::map<std::string, std::string> headers_;

    std::string content_;
    Server& server_;
};

}  // namespace Http

#endif  // HTTP_REQUEST_H_