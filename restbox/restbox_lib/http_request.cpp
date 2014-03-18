#include "http_request.h"
#include "http_request_template.h"
#include "http_response.h"
#include "http_server.h"
#include <mongoose.h>
#include <cassert>


namespace Http {

Request::Request(struct mg_connection* conn)
    : conn_(conn)
    , method_(conn->request_method)
    , uri_(conn->uri)
    , http_version_(conn->http_version)
    , query_string_(conn->query_string ? conn->query_string : "")
    , remote_ip_(conn->remote_ip)
    , local_ip_(conn->local_ip)
    , remote_port_(conn->remote_port)
    , local_port_(conn->local_port)
    , content_(conn->content, conn->content + conn->content_len) {
    assert(conn != 0);
    for (int i = 0; i < conn->num_headers; ++i) {
        typedef struct mg_connection::mg_header Header;
        Header& header = conn->http_headers[i];
        headers_[header.name] = header.value;
    }
}

Request::~Request(void) {
    // nothing
}

void Request::WeakCallback(const v8::WeakCallbackData<v8::Object, Request>& data) {
    Request* pThis = data.GetParameter();
    (pThis->self_).Reset();
    delete pThis;
}

void Request::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

void Request::ClearWeak(void) {
    self_.ClearWeak();
}

void Request::Wait(void) const {
    boost::unique_lock<boost::mutex> lock(mutex_);
    wait_.wait(lock);
}

void Request::Notify(void) const {
    boost::unique_lock<boost::mutex> lock(mutex_);
    wait_.notify_one();
}

struct mg_connection* Request::conn(void) const {
    return conn_;
}

}  // namespace Http