#pragma once

#include <string>
#include <sstream>
#include <memory>

namespace net
{

    class TCPSocket
    {
        public:
            TCPSocket();
            TCPSocket(TCPSocket &&x);
            TCPSocket(int fd);
            ~TCPSocket();
            bool canRead() const;
            bool canWrite() const;
            void close();
            void bind(int port);
            void listen(int n);
            TCPSocket accept();
            void connect(const std::string &address, const std::string &port);
            void write(const std::string &data);
            void writeLine(const std::string &data);
            std::string read(int amount);
            std::string readLine();

        private:
            int fd;
            std::unique_ptr<std::stringstream> inputstream;

            int doRead();
    };
}
