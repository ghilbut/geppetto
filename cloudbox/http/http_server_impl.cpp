#include "http_server.h"
#include "http_server_impl.h"

#define MONGOOSE_NO_DIRECTORY_LISTING
#include <mongoose.h>



namespace Http {


Server::Server(void)
    : pimpl_(new Impl()) {
    // nothing
};

Server::~Server(void) {
    delete pimpl_;
};

bool Server::Start(const std::string& document_root, uint16_t port) {
    return pimpl_->Start(document_root, port);
}

void Server::Stop(void) {
    pimpl_->Stop();
}


static int request_handler(struct mg_connection *conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
    return MG_REQUEST_PROCESSED;
}

Server::Impl::Impl(void) 
    : server_(0), is_stop_(false) {
    // nothing
};

bool Server::Impl::Start(const std::string& document_root, uint16_t port) {
    if (server_ != 0) {
        return false;
    }
    server_ = mg_create_server(this);
    thread_.swap(boost::thread(boost::bind(&Server::Impl::thread_main, this)));
    return true;
}

void Server::Impl::Stop(void) {
    if (server_ == 0) {
        return;
    }
    is_stop_ = true;
    thread_.join();
}


void Server::Impl::thread_main(void) {

    //mg_set_option(server_, "document_root", "");
    mg_set_option(server_, "listening_port", "8080");
    mg_set_request_handler(server_, request_handler);

    while(!is_stop_) {
        mg_poll_server(server_, 1000);
    }

    mg_server* tmp = server_;
    mg_destroy_server(&tmp);
    server_ = 0;
}


}  // namespace Http