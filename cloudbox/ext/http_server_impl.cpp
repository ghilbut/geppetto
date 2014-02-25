#define MONGOOSE_NO_DIRECTORY_LISTING

#include "dummy.h"
#include "http_server_delegate.h"
#include "http_server.h"
#include "http_server_impl.h"
#include <mongoose.h>


namespace Ext {
namespace Http {


Server::Server(void)
    : pimpl_(new Impl()) {
    // nothing
};

Server::~Server(void) {
    delete pimpl_;
};

bool Server::Listen(const std::string& document_root, uint16_t port) {
    return pimpl_->Listen(document_root, port);
}

void Server::Close(void) {
    pimpl_->Close();
}

ServerDelegate* Server::BindDelegate(ServerDelegate* delegate) {
    return pimpl_->BindDelegate(delegate);
}

ServerDelegate* Server::UnbindDelegate(void) {
    return pimpl_->UnbindDelegate();
}

Server::Impl::Impl(void) 
    : delegate_(0), server_(0), is_alive_(false), is_stop_(false) {
    // nothing
};

void* Server::Impl::thread_f(void* param) {
    Server::Impl* s = static_cast<Server::Impl*>(param);
    s->thread_main();
    return 0;
}

bool Server::Impl::Listen(const std::string& document_root, uint16_t port) {

    is_stop_ = false;

    server_ = mg_create_server(this);
    //mg_set_option(server, "document_root", document_root.c_str());
    mg_set_option(server_, "listening_port", "9000");
    mg_set_request_handler(server_, &Server::Impl::request_handler);

    //thread_.swap(boost::thread(boost::bind(&Server::Impl::thread_main, this)));
    mg_start_thread(&thread_f, this);
    return true;
}

void Server::Impl::Close(void) {
    if (!is_alive_) {
        return;
    }
    is_stop_ = true;
    //thread_.join();

    mg_destroy_server(&server_);
}

ServerDelegate* Server::Impl::BindDelegate(ServerDelegate* delegate) {
    ServerDelegate* old = delegate_;
    delegate_ = delegate;
    return old;
}

ServerDelegate* Server::Impl::UnbindDelegate(void) {
    return BindDelegate(0);
}

int Server::Impl::FireOnRequest(mg_connection *conn) {
    if (delegate_) {
        const Request req;
        const Response& res = delegate_->OnRequest(req);

        return MG_REQUEST_PROCESSED;
    }
    return MG_REQUEST_NOT_PROCESSED;
}

int Server::Impl::FireOnMessage(mg_connection *conn) {
    if (delegate_) {
        //delegate_->OnMessage(message);

        return MG_REQUEST_PROCESSED;
    }
    return MG_REQUEST_NOT_PROCESSED;
}

int Server::Impl::request_handler(struct mg_connection *conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);

    Server::Impl* s = static_cast<Server::Impl*>(conn->server_param);

    if (conn->is_websocket) {
        s->FireOnMessage(conn);
    } else {
        s->FireOnRequest(conn);
    }


    return MG_REQUEST_PROCESSED;
}

void Server::Impl::thread_main(void) {
    is_alive_ = true;

    //while(is_stop_ == false) {
    for (;;) {
        mg_poll_server(server_, 1000);
    }

    is_alive_ = false;
}


}  // namespace Http
}  // namespace Ext