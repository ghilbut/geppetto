#include "http_request.h"
#include "http_request_template.h"
#include "http_server.h"
#include <mongoose.h>


namespace Http {


v8::Local<v8::Object> Request::NewRecvRequest(v8::Isolate* isolate, struct mg_connection* conn) {

    v8::Local<v8::Object> request = v8::Object::New(isolate);
    request->Set(
        v8::String::NewFromUtf8(isolate, "request_method")
        , v8::String::NewFromUtf8(isolate, conn->request_method));
    request->Set(
        v8::String::NewFromUtf8(isolate, "uri")
        , v8::String::NewFromUtf8(isolate, conn->uri));
    request->Set(
        v8::String::NewFromUtf8(isolate, "http_version")
        , v8::String::NewFromUtf8(isolate, conn->http_version));
    request->Set(
        v8::String::NewFromUtf8(isolate, "query_string")
        , v8::String::NewFromUtf8(isolate, conn->remote_ip));
    request->Set(
        v8::String::NewFromUtf8(isolate, "remote_ip")
        , v8::String::NewFromUtf8(isolate, conn->local_ip));
    request->Set(
        v8::String::NewFromUtf8(isolate, "local_ip")
        , v8::String::NewFromUtf8(isolate, conn->local_ip));
    request->Set(
        v8::String::NewFromUtf8(isolate, "remote_port")
        , v8::Uint32::New(isolate, conn->remote_port));
    request->Set(
        v8::String::NewFromUtf8(isolate, "local_port")
        , v8::Uint32::New(isolate, conn->local_port));

    const int headers_len = conn->num_headers;
    v8::Local<v8::Object> headers = v8::Object::New(isolate);
    for (int i = 0; i < headers_len; ++i) {

        const char* name = conn->http_headers[i].name;
        const char* value = conn->http_headers[i].value;
        headers->Set(
            v8::String::NewFromUtf8(isolate, name)
            , v8::String::NewFromUtf8(isolate, value));

        v8::Local<v8::Object> header = v8::Object::New(isolate);
        header->Set(
            v8::String::NewFromUtf8(isolate, "name")
            , v8::String::NewFromUtf8(isolate, name));
        header->Set(
            v8::String::NewFromUtf8(isolate, "value")
            , v8::String::NewFromUtf8(isolate, value));
        headers->Set(i, header);
    }
    headers->Set(v8::String::NewFromUtf8(isolate, "length"), v8::Int32::New(isolate, headers_len));
    request->Set(v8::String::NewFromUtf8(isolate, "headers"), headers);

    std::string content(conn->content, conn->content+conn->content_len);
    request->Set(
        v8::String::NewFromUtf8(isolate, "content")
        , (content.empty() ? v8::Null(isolate) : v8::String::NewFromUtf8(isolate, content.c_str())));

    return request;
}

Request::Request(struct mg_connection* conn)
    : method_(conn->request_method)
    , uri_(conn->uri)
    , http_version_(conn->http_version)
    , query_string_(conn->query_string ? conn->query_string : "")
    , remote_ip_(conn->remote_ip)
    , local_ip_(conn->local_ip)
    , remote_port_(conn->remote_port)
    , local_port_(conn->local_port)
    , content_(conn->content, conn->content + conn->content_len)
    , server_(*static_cast<Server*>(conn->server_param))
{
    for (int i = 0; i < conn->num_headers; ++i) {
        typedef struct mg_connection::mg_header Header;
        Header& header = conn->http_headers[i];
        headers_[header.name] = header.value;
    }
}

Request::~Request(void) {
    // nothing
}

}  // namespace Http