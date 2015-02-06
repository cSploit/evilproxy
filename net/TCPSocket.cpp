#include "TCPSocket.hpp"

#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

namespace net
{
    TCPSocket::TCPSocket() : inputstream(new std::stringstream()) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error("socket failed");
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (int*)1, sizeof(int));
    }


    TCPSocket::TCPSocket(TCPSocket &&x) : fd(x.fd), inputstream(std::move(x.inputstream)) {
        x.fd = -1;
    }

    TCPSocket::TCPSocket(int fd) : fd(fd), inputstream(new std::stringstream()) {
    }

    TCPSocket::~TCPSocket() {
        if (fd != -1)
            ::close(fd);
    }

    void TCPSocket::bind(int port) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw std::runtime_error("bind failed");
    }

    void TCPSocket::listen(int n) {
        if (::listen(fd, n) < 0)
            throw std::runtime_error("listen failed");
    }

    TCPSocket TCPSocket::accept() {
        int child = ::accept(fd, nullptr, nullptr);
        if (child < 0)
            throw std::runtime_error("accept failed");
        return TCPSocket(child);
    }

    void TCPSocket::connect(const std::string &address, const std::string &port) {
        addrinfo *res, hints;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(address.c_str(), port.c_str(), nullptr, &res))
            throw std::runtime_error("cannot resolve " + address);

        if (::connect(fd, res->ai_addr, res->ai_addrlen) < 0)
            throw std::runtime_error("connect failed:" + address);
    }

    void TCPSocket::write(const std::string &data) {
        int wrote = 0, ret;
        while (wrote != data.length()) {
            ret = ::write(fd, data.substr(wrote, std::string::npos).c_str(), data.length()-wrote);
            if (ret < 0 && errno != EINTR) {
                std::cerr << "errno is " << errno << std::endl;
                throw std::runtime_error("write failed");
            }
            wrote += ret;
        }
    }

    void TCPSocket::writeLine(const std::string &data) {
        write(data + "\n");
    }

    std::string TCPSocket::read(int amount) {
        char *buffer = new char[amount+1];
        int done = 0;

        while (done < amount) {
            inputstream->read(buffer+done, amount-done);
            done += inputstream->gcount();
            if (inputstream->eof())
                inputstream->clear();
            doRead();
        }
        std::string ret = std::string(buffer, amount);
        delete buffer;
        return ret; 
    }

    std::string TCPSocket::readLine() {
        std::string ret, line;

        getline(*inputstream, ret);
 
        while(inputstream->eof()) {
            inputstream->clear();
            doRead();
            getline(*inputstream, line);
            ret += line;
        }

        if (inputstream->eof())
            inputstream->clear();

        return ret;
    }

    std::string TCPSocket::readUntilClose() {
        while(doRead()) {
        }

        return std::string(std::istreambuf_iterator<char>(*inputstream), std::istreambuf_iterator<char>());
    }

    int TCPSocket::doRead() {
        char buffer[2049];
        int ret = ::read(fd, buffer, 2048);
        if (ret < 0) {
            std::cout << "errno is " << errno << std::endl;
            throw std::runtime_error("read error");
        }
        inputstream->write(buffer, ret);
        return ret;
    }
}
