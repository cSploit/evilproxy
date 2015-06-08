#pragma once

#include <memory>

#include "HTTPTransform.hpp"
#include "net/TCPSocket.hpp"

class Server
{
    public:
        Server(int port, std::shared_ptr<HTTPTransform> transformations);
        void run();
        void stop();
    private:
        net::TCPSocket socket;
        std::shared_ptr<HTTPTransform> transformations;
        bool stopped;
};
