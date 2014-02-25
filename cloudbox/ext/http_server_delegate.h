#ifndef EXT_HTTP_SERVER_DELEGATE_H_
#define EXT_HTTP_SERVER_DELEGATE_H_


namespace Ext {
namespace Http {

class Request;
class Response;
class Websocket;
class Message;
class Error;

class ServerDelegate {
public:
    virtual Response& OnRequest(const Request& request) = 0;
    virtual void OnMessage(const Websocket& sock, const Message& msg) = 0;
    virtual void OnError(const Error& error) = 0;

protected:
    ~ServerDelegate(void) {}
};

}  // namespace Http
}  // namespace Ext

#endif  // EXT_SHTTP_SERVER_DELEGATE_H_