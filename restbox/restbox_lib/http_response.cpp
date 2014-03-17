#include "http_response.h"
#include <mongoose.h>


namespace Http {

Response::Response(void) 
    : status_code_(200) {

}

int Response::GetStatusCode(void) const {
    return status_code_;
}

void Response::SetStatusCode(int status_code) {
    status_code_ = status_code;
}

void Response::InsertHeader(const char* name, const char* value) {
    headers_[name] = value;
}

void Response::RemoveHeader(const char* name) {
    std::map<std::string, std::string>::iterator itr = headers_.find(name);
    if (itr != headers_.end()) {
        headers_.erase(itr);
    }
}

int Response::GetDataLength(void) const {
    return data_.size();
}

int Response::GetData(char* buffer, int buffer_len) const {
    int data_len = data_.size();
    memcpy(buffer, &data_[0], std::min(buffer_len, data_len));
    return data_len;
}

void Response::SetData(const char* data, int data_len) {
    data_.swap(std::vector<char>(data, data + data_len));
}

}  // namespace Http