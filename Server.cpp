#include "Server.hpp"

#include <thread>

#include "Proxy.hpp"

Server::Server(int port, std::shared_ptr<HTTPTransform> transformations) : transformations(transformations), stopped(false) {
    socket.bind(port);
    socket.listen(5);
}

void Server::run() {
    while(!stopped) {
        std::thread t(&Proxy::run, Proxy(socket.accept(), transformations));
        t.detach();
    }
}

void Server::stop() {
    stopped = true;
}
