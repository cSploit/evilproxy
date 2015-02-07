#include "HTTP.hpp"

#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

void net::HTTP::read(net::TCPSocket &socket) {
    readRequestLine(socket);
    readHeaders(socket);
    readContent(socket);
}

void net::HTTP::readRequestLine(net::TCPSocket &socket) {
    requestLine = trim(socket.readLine());
}

void net::HTTP::readHeaders(net::TCPSocket &socket) {
    std::string line;

    for (;;) {
        line = trim(socket.readLine());

        if (line == "")
            break;

        int pos = line.find_first_of(":");
        if (pos == std::string::npos)
            throw std::runtime_error("Invalid header (" + line + ")");

        std::string header = trim(line.substr(0, pos));

        for (auto &x : header)
            x = tolower(x);
        std::string value = trim(line.substr(pos+1));

        headers[header] = value;
    }
}

void net::HTTP::readContent(net::TCPSocket &socket) {
    int size = 1;

    if (isChunkedEncoded()) {
//     Changing chunked transfer to Content-Length - easier content resizing
        while (size) {
            std::string line = trim(socket.readLine());
            std::stringstream converter(line);
            converter >> std::hex >> size;
            content += socket.read(size);
            socket.read(2);
        }
        headers.erase("transfer-encoding");
        std::stringstream converter;
        converter << content.length();
        headers["content-length"] = converter.str();

    } else if (headers.count("content-length")) {

        std::stringstream converter(headers["content-length"]);
        converter >> size;

        content = socket.read(size);
    } else if (getVersion() == "1.0") {
        content = socket.readUntilClose();
    }
}

void net::HTTP::write(net::TCPSocket &socket) {
    writeRequestLine(socket);
//    std::cout << requestLine << std::endl;
    writeHeaders(socket);
//    for (auto &it : headers)
//        std::cout << it.first + ": " + it.second << std::endl;
    writeContent(socket);
//    std::cout << std::endl;
}

void net::HTTP::writeRequestLine(net::TCPSocket &socket) {
    socket.writeLine(requestLine + "\r");
}

void net::HTTP::writeHeaders(net::TCPSocket &socket) {
    for (auto &it : headers)
        socket.writeLine(it.first + ": " + it.second + "\r");
    socket.writeLine("\r");
}

void net::HTTP::writeContent(net::TCPSocket &socket) {
    socket.write(content);
}

void net::HTTP::streamContent(net::TCPSocket &from, net::TCPSocket &to) {
    int size = 1;

    if (isChunkedEncoded()) {
        std::cout << "CHUNKED " << requestLine << std::endl;
        while (size) {

            std::string line = trim(from.readLine());
            std::stringstream converter(line);
            converter >> std::hex >> size;
            std::string chunk = from.read(size+2);

            to.write(line + "\r\n" + chunk);
        }

    } else if (headers.count("content-length")) {
        std::stringstream converter(headers["content-length"]);
        converter >> size;

        while(size > 0) {
            to.write(from.read(std::min(8192, size)));
            size -= 8192;
        }

    } else if (getVersion() == "1.0") {
     //TODO add HTTP 1.0 streaming   
    }
}

void net::HTTP::fixContentLength() {
    std::stringstream converter;
    converter << content.length();
    headers["content-length"] = converter.str();
}

std::map<std::string, std::string> & net::HTTP::getHeaders() {
    return headers;
}

std::string & net::HTTP::getContent() {
    return content;
}

bool net::HTTP::isChunkedEncoded() {
    auto transfer = headers.find("transfer-encoding");
    return (transfer != headers.end() && transfer->second == "chunked");
}

std::string net::HTTP::getVersion() {
    return requestLine.substr(requestLine.find("HTTP/")+5, 3);
}

std::string net::HTTP::trim(std::string str) {
    std::string whitespaces = "\r\n\t ";
    str.erase(0, str.find_first_not_of(whitespaces));
    int pos = str.find_last_not_of(whitespaces);
    if (pos != std::string::npos)
        str.erase(pos+1);
    return str;
}
