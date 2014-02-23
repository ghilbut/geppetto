#ifndef HTTP_SERVER_DELEGATE_H_
#define HTTP_SERVER_DELEGATE_H_


namespace Http {

class Request;
class Response;
class Websocket;
class Message;
class Error;

class ServerDelegate {
    virtual Response& OnRequest(const Request& request) = 0;
    virtual void OnMessage(const Websocket& sock, const Message& msg) = 0;
    virtual void OnError(const Error& error) = 0;
};

}  // namespace Http

#endif  // HTTP_SERVER_DELEGATE_H_