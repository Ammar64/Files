#include "Server.h"

Server::Server() {
}

void Server::Start() {
    thread = new std::thread(&Server::__start_server, this);
}

void Server::Restart() {
    Stop();
    Start();
}

void Server::Stop() {
    if( thread != nullptr ){
        svr.stop();
        thread->join();
        delete thread;
        thread = nullptr;
    }
}

void Server::__start_server(){
    svr.listen("0.0.0.0", port);
}