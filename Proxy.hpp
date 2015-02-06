#pragma once
#include <string>
#include <map>

#include "net/TCPSocket.hpp"
#include "HTTPTransform.hpp"

class Proxy
{
    public:
        Proxy(net::TCPSocket &&socket, HTTPTransform &transformations);
        void run();
    private:

        static std::vector<std::string> modifyableTypes;
        net::TCPSocket client;
        HTTPTransform &transformations;
};
