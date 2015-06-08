#pragma once

#include <map>

#include "TCPSocket.hpp"

namespace net
{
    class HTTP {
        public:
            void read(TCPSocket &socket);
            void readRequestLine(TCPSocket &socket);
            void readHeaders(TCPSocket &socket);
            void readContent(TCPSocket &socket);

            void write(TCPSocket &socket);
            void writeRequestLine(TCPSocket &socket);
            void writeHeaders(TCPSocket &socket);
            void writeContent(TCPSocket &socket);

            void streamContent(TCPSocket &from, TCPSocket &to);

            void fixContentLength();

            std::map<std::string, std::string> & getHeaders();
            std::string & getRequestLine();
            std::string & getContent();
            std::string getUrl() const;

            enum class Type {Request, Response};
            Type getType() const;
        private:

            bool isChunkedEncoded() const;

            std::string trim(std::string str);

            std::string requestLine;
            std::map<std::string, std::string> headers;
            std::string content;

    };

}
