#include "http_server.h"
#include "http_server_base.h"
#include "http_server_wrapper.h"
#include "http_request.h"



namespace Http {

void Server::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Server::Wrapper::Constructor(args);
}

bool Server::DoListen(const std::string& document_root, uint16_t port) {
    boost::promise<bool> promise;

    void (boost::promise<bool>::*setter)(const bool&) = &boost::promise<bool>::set_value;
    boost::function<void(const bool&)> promise_setter = boost::bind(setter, &promise, _1);
    strand_.post(boost::bind(&Server::handle_listen, this, document_root, port, promise_setter));

    return promise.get_future().get();
}

void Server::DoClose(void) {
    strand_.post(boost::bind(&Server::handle_close, this));
}

void Server::FireRequest(struct mg_connection *conn) {
    boost::promise<bool> promise;

    void (boost::promise<bool>::*setter)(const bool&) = &boost::promise<bool>::set_value;
    boost::function<void(const bool&)> promise_setter = boost::bind(setter, &promise, _1);
    strand_.post(boost::bind(&Server::handle_request, this, conn, promise_setter));

    promise.get_future().get();
}

void Server::FireMessage(struct mg_connection *conn) {
    strand_.post(boost::bind(&Server::handle_message, this, conn));
}

void Server::FireError(void) {
    strand_.post(boost::bind(&Server::handle_error, this));
}



Server::Server(v8::Isolate* isolate, boost::asio::io_service& io_service)
    : base_(new Base())
    , isolate_(isolate)
    , io_service_(io_service)
    , strand_(io_service_) {
    base_->BindRequest(boost::bind(&Server::FireRequest, this, _1));
    base_->BindMessage(boost::bind(&Server::FireMessage, this, _1));
}

Server::~Server(void) {
    base_->BindRequest(0);
    base_->BindMessage(0);
    delete base_;
}

void Server::handle_listen(const std::string& document_root, uint16_t port, boost::function<void(const bool&)> ret_setter) {
    ret_setter(base_->Listen(document_root, port));
}

void Server::handle_close() {
    base_->Close();
}

void Server::handle_request(struct mg_connection *conn, boost::function<void(const bool&)> ret_setter) {
    if (on_request_.IsEmpty()) {
        ret_setter(false);
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    //params[0] = v8::Integer::New(isolate_, 10);
    params[0] = Http::Request::New(isolate_, conn);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_request_);
    if (func->IsCallable()) {
        v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, object_);
        v8::Handle<v8::Value> js_result = func->Call(object, 1, params);
        // do something with the result
    }
    ret_setter(true);
    return;
}

void Server::handle_message(struct mg_connection *conn) {
    if (on_message_.IsEmpty()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = v8::Integer::New(isolate_, 2);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_message_);
    if (func->IsCallable()) {
        v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, object_);
        func->Call(object, 1, params);
    }
}

void Server::handle_error(void) {
    if (on_error_.IsEmpty()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = v8::Integer::New(isolate_, 2);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_error_);
    if (func->IsCallable()) {
        v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, object_);
        func->Call(object, 1, params);
    }
}

}  // namespace Http