#include "http_server.h"
#include "http_request.h"
#include "dummy.h"
#include <mongoose.h>


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
    //strand_.post(boost::bind(&Server::handle_request, this, conn));

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
    : base_(*this)
    , isolate_(isolate)
    , io_service_(io_service)
    , strand_(io_service_) {
        // nothing
}

Server::~Server(void) {
    //onload_.Dispose();
}

void Server::handle_listen(const std::string& document_root, uint16_t port, boost::function<void(const bool&)> ret_setter) {
    ret_setter(base_.Listen(document_root, port));
}

void Server::handle_close() {
    base_.Close();
}

void Server::handle_request(struct mg_connection *conn, boost::function<void(const bool&)> ret_setter) {
    if (on_request_.IsEmpty()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = Http::Request::New(isolate_, conn);

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



Server::Base::Base(Server& server)
    : server_(server)
    , is_alive_(false)
    , is_stop_(false) {
    // nothing
}

Server::Base::~Base() {
    Close();
}

bool Server::Base::Listen(const std::string& document_root, uint16_t port) {
    if (is_alive_) {
        return false;
    }

    is_alive_ = true;
    is_stop_ = false;
    document_root_ = document_root;
    port_ = port;

    if (thread_.joinable()) {
        thread_.join();
    }
    thread_.swap(boost::thread(boost::bind(&Server::Base::thread_main, this)));
    return true;
}

void Server::Base::Close(void) {
    is_stop_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
}

int Server::Base::request_handler(struct mg_connection *conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);

    Server& s = static_cast<Base*>(conn->server_param)->server_;
    if (conn->is_websocket) {
        //s.FireMessage(conn);
    } else {
        //s.FireRequest(conn);               
    }

    return MG_REQUEST_PROCESSED;
}

void Server::Base::thread_main(void) {

    mg_server* server = mg_create_server(this);
    if (server == 0) {
        goto ERROR0;
    }

    const char* error_msg = 0;
    //error_msg = mg_set_option(server, "document_root", document_root_.c_str());
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

    mg_set_request_handler(server, &Server::Base::request_handler);
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


v8::Persistent<v8::ObjectTemplate> Server::Wrapper::template_;

void Server::Wrapper::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function."));
        args.GetReturnValue().Set(e);
        return;
    }

    if (args.Length() > 0) {
        v8::Handle<v8::Value> e = isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor with any parameter(s)."));
        args.GetReturnValue().Set(e);
        return;
    }

    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::ObjectTemplate> object_t;
    if (template_.IsEmpty()) {
        object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        object_t->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Listen));
        object_t->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, Server::Wrapper::Close));
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), Server::Wrapper::GetEventRequest, Server::Wrapper::SetEventRequest);
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), Server::Wrapper::GetEventMessage, Server::Wrapper::SetEventMessage);
        template_.Reset(isolate, object_t);
    } else {
        object_t = v8::Local<v8::ObjectTemplate>::New(isolate, template_);
    }

    boost::asio::io_service* io_service = static_cast<boost::asio::io_service*>(isolate->GetData(0));

    Server* server = new Server(isolate, *io_service);
    v8::Handle<v8::Object> object = object_t->NewInstance();
    object->SetInternalField(0, v8::External::New(isolate, server));
    server->object_.Reset(isolate, object);

    args.GetReturnValue().Set(object);
}

template<typename T>
Server* Server::Wrapper::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Server*>(ptr);
}

void Server::Wrapper::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_request_);
}

void Server::Wrapper::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_request_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_message_);
}

void Server::Wrapper::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_message_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::GetEventError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->on_error_);
}

void Server::Wrapper::SetEventError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (Unwrap(info)->on_error_).Reset(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void Server::Wrapper::Listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    if (args.Length() < 2) {
        const char* text = "Invalid usgae.";
        args.GetReturnValue().Set(v8::Exception::SyntaxError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }

    v8::Handle<v8::Value> param0 = args[0];
    if (!param0->IsString()) {
        const char* text = "Document root should be string.";
        args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }
    const v8::String::Utf8Value utf8(param0); 
    const std::string document_root = (utf8.length() > 0 ? *utf8 : "");
    // TODO(ghilbut): check path is exists

    v8::Handle<v8::Value> param1 = args[1];
    if (!param1->IsNumber() || !param1->IsUint32()) {
        const char* text = "Port should be a positive number.";
        args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }
    const uint32_t port = param1->Uint32Value();
    if (port > 65535) {
        const char* text = "Max port number is 65535.";
        args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(isolate, text)));
        return;
    }

    Server* s = Unwrap(args);
    bool result = s->DoListen(document_root, static_cast<uint16_t>(port));
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void Server::Wrapper::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    //Ext::Http::Server& s = Unwrap(args)->server();
    //s.Close();
    Server* s = Unwrap(args);
    s->DoClose();
}


}  // namespace Http