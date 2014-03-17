#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <map>
#include <string>
#include <vector>


namespace Http {

class Response {
public:
    Response(void);
    ~Response(void) {}

    int GetStatusCode(void) const;
    void SetStatusCode(int status_code);

    void InsertHeader(const char* name, const char* value);
    void RemoveHeader(const char* name);

    int GetDataLength(void) const;
    int GetData(char* buffer, int buffer_len) const;
    void SetData(const char* data, int data_len);

private:
    int status_code_;
    std::map<std::string, std::string> headers_;
    std::vector<char> data_;
};

}  // namespace Http

#endif  // HTTP_RESPONSE_H_