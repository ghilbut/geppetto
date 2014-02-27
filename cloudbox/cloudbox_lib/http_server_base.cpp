#include "http_server.h"
#include "http_server_base.h"
#include <mongoose.h>


namespace Http {

Server::Base::Base(void)
    : is_alive_(false)
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

void Server::Base::BindRequest(TriggerRequest trigger) {
    trigger_request_ = trigger;
}

void Server::Base::BindMessage(TriggerRequest trigger) {
    trigger_message_ = trigger;
}

int Server::Base::request_handler(struct mg_connection *conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);

    Base* b = static_cast<Base*>(conn->server_param);
    if (conn->is_websocket) {
        if (b->trigger_message_) {
            b->trigger_message_(conn);
        }
    } else {
        if (b->trigger_request_) {
            b->trigger_request_(conn);
        }
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

}  // namespace Http