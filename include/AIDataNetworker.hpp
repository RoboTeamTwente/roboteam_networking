#pragma once

#include <functional>
#include <string>

#include <utils/Publisher.hpp>
#include <utils/Subscriber.hpp>

#include <roboteam_utils/AIData.hpp>

namespace rtt::net {

class AIPrimaryDataPublisher : private utils::Publisher {
public:
    AIPrimaryDataPublisher();

    // Publishes the given AI data of the primary AI. Returns success
    bool publish(const rtt::AIData& data);
};

class AIPrimaryDataSubscriber : private utils::Subscriber {
public:
    AIPrimaryDataSubscriber(const std::function<void(const rtt::AIData&)>& callback);

private:
    void onPublishedMessage(const std::string& message);
    const std::function<void(const rtt::AIData&)> callback;
};

class AISecondaryDataPublisher : private utils::Publisher {
public:
    AISecondaryDataPublisher();

    // Publishes the given AI data of the secondary AI. Returns success
    bool publish(const rtt::AIData& data);
};

class AISecondaryDataSubscriber : private utils::Subscriber {
public:
    AISecondaryDataSubscriber(const std::function<void(const rtt::AIData&)>& callback);

private:
    void onPublishedMessage(const std::string& message);
    const std::function<void(const rtt::AIData&)> callback;
};





} // namespace rtt::net