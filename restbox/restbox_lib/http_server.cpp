#include "http_server.h"
#include "http_request.h"
#include "http_request_template.h"
#include "http_response.h"
#include "http_response_template.h"
#include <mongoose.h>



namespace Http {

Server::Server(v8::Isolate* isolate, boost::asio::io_service& io_service)
    : isolate_(isolate)
    , io_service_(io_service)
    , strand_(io_service_)
    , is_alive_(false)
    , is_stop_(false){
   // nothing
}

Server::~Server(void) {
    DoClose();
}

void Server::WeakCallback(const v8::WeakCallbackData<v8::Object, Server>& data) {
  Server* pThis = data.GetParameter();
  (pThis->self_).Reset();
  delete pThis;
}

void Server::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

void Server::ClearWeak(void) {
    self_.ClearWeak();
}

bool Server::DoListen(uint16_t port) {
    boost::promise<bool> promise;

    void (boost::promise<bool>::*setter)(const bool&) = &boost::promise<bool>::set_value;
    boost::function<void(const bool&)> promise_setter = boost::bind(setter, &promise, _1);
    strand_.post(boost::bind(&Server::handle_listen, this, port, promise_setter));

    return promise.get_future().get();
}

void Server::DoClose(void) {
    if (!is_stop_) {
        strand_.post(boost::bind(&Server::handle_close, this));
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

v8::Local<v8::Function> Server::request_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_request_);
}

void Server::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_request_.Reset(isolate, trigger);
}

v8::Local<v8::Function> Server::message_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_message_);
}

void Server::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_message_.Reset(isolate, trigger);
}

v8::Local<v8::Function> Server::error_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_error_);
}

void Server::set_error_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_error_.Reset(isolate, trigger);
}

void Server::FireRequest(struct mg_connection *conn, Request* req) {
    strand_.post(boost::bind(&Server::handle_request, this, conn, req));
}

void Server::FireMessage(struct mg_connection *conn) {
    strand_.post(boost::bind(&Server::handle_message, this, conn));
}

void Server::FireError(void) {
    strand_.post(boost::bind(&Server::handle_error, this));
}

void Server::handle_listen(uint16_t port, boost::function<void(const bool&)> ret_setter) {
    if (is_alive_) {
        ret_setter(false);
        return;
    }

    is_alive_ = true;
    is_stop_ = false;
    port_ = port;

    if (thread_.joinable()) {
        thread_.join();
    }
    thread_.swap(boost::thread(boost::bind(&Server::thread_main, this)));
    ret_setter(true);
}

void Server::handle_close() {
    is_stop_ = true;
}

void Server::handle_request(struct mg_connection *conn, Request* req) {
    if (on_request_.IsEmpty()) {
        // TODO(ghilbut): error handling
        mg_send_data(conn, 0, 0);
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_request_);
    if (!func->IsCallable()) {
        // TODO(ghilbut): error handling
        mg_send_data(conn, 0, 0);
        return;
    }

    v8::Handle<v8::Value> params[1];
    //params[0] = Request::NewRecvRequest(isolate_, conn);
    params[0] = RequestTemplate::NewInstance(isolate_, req);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    func->Call(object, 1, params);
}

void Server::handle_message(struct mg_connection *conn) {
    if (on_message_.IsEmpty()) {
        return;
    }
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_message_);
    if (!func->IsCallable()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = v8::Integer::New(isolate_, 2);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    func->Call(object, 1, params);
}

void Server::handle_error(void) {
    if (on_error_.IsEmpty()) {
        return;
    }
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_error_);
    if (func->IsCallable()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = v8::Integer::New(isolate_, 2);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    func->Call(object, 1, params);
}

int Server::request_handler(struct mg_connection *conn) {

    Server* s = static_cast<Server*>(conn->server_param);
    if (!conn->is_websocket) {
        Request* req = new Request(conn);
        s->FireRequest(conn, req);
        req->Wait();
    } else {
        s->FireMessage(conn);
    }

    return MG_REQUEST_PROCESSED;
}

void Server::thread_main(void) {

    mg_server* server = mg_create_server(this);
    if (server == 0) {
        goto ERROR0;
    }

    const char* error_msg = 0;
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    char sport[10];  
    sprintf(sport, "%d", port_);
    error_msg = mg_set_option(server, "listening_port", sport);
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    mg_set_request_handler(server, &Server::request_handler);
    while(is_stop_ == false) {
        if (mg_poll_server(server, 1000) == 0) {
            // TODO(ghilbut): delegate error
            is_stop_ = true;
            goto ERROR1;
        }
    }

ERROR1:
    mg_destroy_server(&server);
ERROR0:
    is_alive_ = false;
}

}  // namespace Http