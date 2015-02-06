#include "Proxy.hpp"

#include <map>
#include <iostream>
#include <sstream>

#include "net/HTTP.hpp"

std::vector<std::string> Proxy::modifyableTypes = {"/html", "/css", "/javascript", "/x-javascript"};

Proxy::Proxy(net::TCPSocket &&socket, HTTPTransform &transformations) :
        client(std::move(socket)), transformations(transformations) {
}

void Proxy::run() {
    net::HTTP request, response;

    request.read(client);

    request.getHeaders()["accept-encoding"] = "identity";
    request.getHeaders()["connection"] = "close";

    net::TCPSocket server;
    server.connect(request.getHeaders()["host"], "80");

    request.write(server);

    response.readRequestLine(server);
    response.readHeaders(server);

    std::string type = response.getHeaders()["content-type"];
    bool found = false;
    for (auto &modType : modifyableTypes) {
        if (type.find(modType) != std::string::npos) {
            found = true;
            break;
        }
    }

    if (found) {
        response.readContent(server);
        transformations.transformResponse(response);
        response.fixContentLength();
        response.write(client);
    } else {
        response.writeRequestLine(client);
        response.writeHeaders(client);

        response.streamContent(server, client);
    }

}
