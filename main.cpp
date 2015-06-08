#include <iostream>
#include <thread>
#include <regex>
#include <memory>

#include "Server.hpp"
#include "HTTPTransform.hpp"

int main(int argc, char **argv) {
    auto transformations = std::make_shared<HTTPTransform>();
    std::thread st(&Server::run, Server(8080, transformations));
    st.detach();

    std::string input;

    for (;;) {
        std::cin >> input;

        if (input == "CLEAR_COOKIES") {
            std::string host;
            std::cin >> host;
            auto fn = [host] (net::HTTP &http) {
                if (http.getType() == net::HTTP::Type::Response || http.getUrl().find(host) != std::string::npos) {
                    http.getHeaders().erase("cookie");
                    http.getHeaders().erase("set-cookie");
                }
            };
            transformations->addRequestTransform(fn);
            transformations->addResponseTransform(fn);

        } else if (input.compare(0, 4, "EDIT") == 0) {
            std::string reg_str, replace;
            std::cin >> reg_str >> replace;
            std::regex reg(reg_str);
            auto fn = [reg, replace] (net::HTTP &http) {
                http.getContent() = std::regex_replace(http.getContent(), reg, replace);
            };
            if (input == "EDIT_REQUEST") {
                transformations->addRequestTransform(fn);
            } else if (input == "EDIT_RESPONSE") {
                transformations->addResponseTransform(fn);
            }
        } else if (input == "WATCH") {
            std::string host;
            std::cin >> host;
            auto fn = [host] (net::HTTP &http) {
                auto url = http.getUrl();
                if (url.find(host) != std::string::npos) {
                    std::cout << "VISITING " << url << std::endl;
                    auto cookie = http.getHeaders().find("cookie");
                    if (cookie != http.getHeaders().end()) {
                        std::cout << "COOKIE " << cookie->second << std::endl;
                    }
                }
            };
            transformations->addRequestTransform(fn);
        }
    }

    return 0;
}
