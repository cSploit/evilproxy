#pragma once

#include <vector>
#include <shared_mutex>

#include "net/HTTP.hpp"

class HTTPTransform
{
    public:
        void addRequestTransform(std::function <void (net::HTTP &request)> transform);
        void addResponseTransform(std::function <void (net::HTTP &response)> transform);
        void clearRequestTransform();
        void clearResponseTransform();

        void transformRequest(net::HTTP &request);
        void transformResponse(net::HTTP &response);
    private:
        std::vector <std::function <void (net::HTTP &request)>> requestTransformations;
        std::vector <std::function <void (net::HTTP &response)>> responseTransformations;

        std::shared_timed_mutex requestMutex;
        std::shared_timed_mutex responseMutex;
};
