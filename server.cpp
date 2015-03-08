#include <iostream>
#include <thread>
#include <regex>

#include "net/TCPSocket.hpp"
#include "net/HTTP.hpp"
#include "HTTPTransform.hpp"
#include "Proxy.hpp"

HTTPTransform transformations;

class Server
{
    public:
        Server(int port) {
            socket.bind(port);
            socket.listen(5);
        }

        void run() {
            for (;;) {

                std::thread t(&Proxy::run, Proxy(socket.accept(), transformations));
                t.detach();
            }
        }
    private:
        net::TCPSocket socket;
};


int main(int argc, char **argv) {
    if (argc != 1 && argc != 3)
        return -1;

    if (argc == 3) {
        std::regex reg(argv[1]);
        std::string replace = argv[2];
        transformations.addResponseTransform([reg, replace] (net::HTTP &response) {
                response.getContent() = std::regex_replace(response.getContent(), reg, replace);
                });
    }

    Server s(8080);
    s.run();
    return 0;
}
