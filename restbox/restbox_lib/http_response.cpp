#include "http_response.h"
#include "http_request.h"
#include <mongoose.h>
#include <cassert>


namespace Http {

Response::Response(Request* req) 
    : req_(req)
    , status_code_(200) {
    assert(req != 0);
}

void Response::WeakCallback(const v8::WeakCallbackData<v8::Object, Response>& data) {
  Response* pThis = data.GetParameter();
  (pThis->self_).Reset();
  delete pThis;
}

void Response::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

void Response::ClearWeak(void) {
    self_.ClearWeak();
}

void Response::SetStatusCode(int status_code) {
    status_code_ = status_code;
}

void Response::SetResponseHeader(const char* name, const char* value) {
    headers_[name] = value;
}

void Response::Send(const char* data, int data_len) const {

    struct mg_connection* conn = req_->conn();
    assert(conn != 0);

    mg_send_status(conn, status_code_);
    if (!headers_.empty()) {
        HeaderMap::const_iterator itr = headers_.begin();
        HeaderMap::const_iterator end = headers_.end();
        for (; itr != end; ++itr) {
            mg_send_header(conn, (itr->first).c_str(), (itr->second).c_str());
        }
    }
    mg_send_data(conn, data, data_len);
    req_->Notify();
}

}  // namespace Http