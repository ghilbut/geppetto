#include "cloudbox.h"
#include "http/dummy.h"


CloudBox::CloudBox(void) {
    work_ = new boost::asio::io_service::work(io_service_);
    thread_.swap(boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
}

CloudBox::~CloudBox(void) {
    delete work_;
    thread_.join();
}

bool CloudBox::Start(const std::string& document_root, uint16_t port) {
    server_.BindDelegate(this);
    return server_.Start(document_root, port);
}

void CloudBox::Stop(void) {
    server_.Stop();
    server_.UnbindDelegate();
}

void CloudBox::RunShell(void) {
    js_engine_.RunShell();
}

Response& CloudBox::OnRequest(const Request& request) {
    Response res;
    return res;
}

void CloudBox::OnMessage(const Websocket& sock, const Message& msg) {

}

void CloudBox::OnError(const Error& error) {

}