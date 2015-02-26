#include "Proxy.hpp"

#include <map>
#include <iostream>
#include <algorithm>
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

    auto contentTypeIt = response.getHeaders().find("content-type");
    if (contentTypeIt != response.getHeaders().end() && std::any_of(modifyableTypes.begin(),
                modifyableTypes.end(),
                [&contentTypeIt] (std::string type) {return type == contentTypeIt->second;})) {
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
