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

    try {
        std::string host;

        for (;;) {
            net::HTTP request, response;

            request.read(client);

            // Client could close connection in the meantime
            if (request.getRequestLine().empty())
                return;

            request.getHeaders()["accept-encoding"] = "identity";

            if (host.empty()) {
                host = request.getHeaders()["host"];
                server.connect(host, "80");
                std::cout << host << std::endl;
            }

            request.write(server);

            response.readRequestLine(server);
            // Server could close connection in the meantime
            if (response.getRequestLine().empty())
                return;

            response.readHeaders(server);

            auto contentTypeIt = response.getHeaders().find("content-type");
            if (contentTypeIt != response.getHeaders().end() && std::any_of(modifyableTypes.begin(),
                        modifyableTypes.end(),
                        [&contentTypeIt] (std::string type) {return contentTypeIt->second.find(type) != std::string::npos;})) {
                response.readContent(server);
                transformations.transformResponse(response);
                response.fixContentLength();
                response.write(client);
            } else {
                response.writeRequestLine(client);
                response.writeHeaders(client);

                response.streamContent(server, client);
        }

        if (response.getHeaders()["connection"] == "close" ||
                request.getHeaders()["connection"] == "close" ||
                !server.canWrite())
            break;
        }

    } catch (std::runtime_error er) {
        std::cerr << er.what() << std::endl;
    }
}
