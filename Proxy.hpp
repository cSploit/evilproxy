#pragma once
#include <string>
#include <map>
#include <memory>

#include "net/TCPSocket.hpp"
#include "HTTPTransform.hpp"

class Proxy
{
    public:
        Proxy(net::TCPSocket &&socket, std::shared_ptr<HTTPTransform> transformations);
        void run();
    private:

        static std::vector<std::string> modifyableTypes;
        net::TCPSocket client;
        net::TCPSocket server;
        std::shared_ptr<HTTPTransform> transformations;
};
