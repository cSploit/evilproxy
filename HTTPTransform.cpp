#include "HTTPTransform.hpp"

#include <vector>

void HTTPTransform::addRequestTransform(std::function <void (net::HTTP &request)> transform) {
    std::lock_guard <std::shared_timed_mutex> lock(requestMutex);

    requestTransformations.push_back(transform);
}

void HTTPTransform::addResponseTransform(std::function <void (net::HTTP &response)> transform) {
    std::lock_guard <std::shared_timed_mutex> lock(responseMutex);

    responseTransformations.push_back(transform);
}

void HTTPTransform::clearRequestTransform() {
    std::lock_guard <std::shared_timed_mutex> lock(requestMutex);

    requestTransformations.clear();
}

void HTTPTransform::clearResponseTransform() {
    std::lock_guard <std::shared_timed_mutex> lock(responseMutex);

    responseTransformations.clear();
}

void HTTPTransform::transformRequest(net::HTTP &request) {
    std::shared_lock <std::shared_timed_mutex> lock(requestMutex);

    for (auto &fun : requestTransformations)
        fun(request);
}

void HTTPTransform::transformResponse(net::HTTP &response) {
    std::shared_lock <std::shared_timed_mutex> lock(responseMutex);

    for (auto &fun : responseTransformations)
        fun(response);
}
