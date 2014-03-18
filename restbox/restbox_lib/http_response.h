#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <v8.h>
#include <map>
#include <string>
#include <vector>


struct mg_connection;

namespace Http {

class Request;

class Response {
public:
    explicit Response(Request* req);
    ~Response(void) {}

    static void WeakCallback(const v8::WeakCallbackData<v8::Object, Response>& data);
    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);
    void ClearWeak(void);

    void SetStatusCode(int status_code);
    void SetResponseHeader(const char* name, const char* value);
    void Send(const char* data, int data_len) const;

private:
    Request* req_;

    typedef std::map<std::string, std::string> HeaderMap;
    int status_code_;
    HeaderMap headers_;

    v8::Persistent<v8::Object> self_;
};

}  // namespace Http

#endif  // HTTP_RESPONSE_H_