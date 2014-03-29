#include "http_response.h"
#include "http_request.h"
#include <mongoose.h>
#include <cassert>


namespace Http {

Response::Response(void) 
    : status_code_(200) {
    // nothing
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

int Response::status_code(void) const {
    return status_code_;
}

void Response::set_status_code(int status_code) {
    status_code_ = status_code;
}

const char* Response::GetHeader(const char* name) const {
    HeaderMap::const_iterator itr = headers_.find(name);
    if (itr == headers_.end()) {
        return 0;
    }
    return (itr->second).c_str();
}

void Response::SetHeader(const char* name, const char* value) {
    headers_[name] = value;
}

void Response::RemoveHeader(const char* name) {
    HeaderMap::const_iterator itr = headers_.find(name);
    if (itr != headers_.end()) {
        headers_.erase(itr);
    }
}

const std::string& Response::data(void) const {
    return data_;
}

void Response::set_data(const char* data, int data_len) {
    data_.swap(std::string(data, data + data_len));
}

void Response::Send(struct mg_connection* conn) const {

    mg_send_status(conn, status_code_);

    if (!headers_.empty()) {
        HeaderMap::const_iterator itr = headers_.begin();
        HeaderMap::const_iterator end = headers_.end();
        for (; itr != end; ++itr) {
            mg_send_header(conn, (itr->first).c_str(), (itr->second).c_str());
        }
    }

    if (data_.empty()) {
        mg_send_data(conn, 0, 0);
    } else {
        mg_send_data(conn, data_.c_str(), data_.length());
    }
}

}  // namespace Http