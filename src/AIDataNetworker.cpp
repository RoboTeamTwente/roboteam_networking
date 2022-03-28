#include <proto/AIData.pb.h>

#include <AIDataNetworker.hpp>

namespace rtt::net {

proto::AIData AIDataToProto(const rtt::AIData& data) {
    proto::AIData proto;

    proto.set_current_play(data.currentPlay);

    for (const auto &playName : data.allPlays) {
        proto.add_all_plays(playName);
    }

    return proto;
}

AIData protoToAIData(const proto::AIData& proto) {
    AIData aiData;

    aiData.currentPlay = proto.current_play();

    for (const auto &playName : proto.all_plays()) {
        aiData.allPlays.push_back(playName);
    }

    return aiData;
}

// Primary AI data publisher
AIPrimaryDataPublisher::AIPrimaryDataPublisher() : utils::Publisher(utils::ChannelType::AI_PRIMARY_DATA_CHANNEL) {}

bool AIPrimaryDataPublisher::publish(const rtt::AIData& data) {
    auto protoRobotCommands = AIDataToProto(data);
    return this->send(protoRobotCommands.SerializeAsString());
}

// Primary AI data subscriber
AIPrimaryDataSubscriber::AIPrimaryDataSubscriber(const std::function<void(const rtt::AIData&)>& callback)
    : utils::Subscriber(utils::ChannelType::AI_PRIMARY_DATA_CHANNEL, [&](const std::string& message) { this->onPublishedMessage(message); }), callback(callback) {
    if (callback == nullptr) {
        throw utils::InvalidCallbackException("Callback was nullptr");
    }
}

void AIPrimaryDataSubscriber::onPublishedMessage(const std::string& message) {
    proto::AIData aiData;
    aiData.ParseFromString(message);
    this->callback(protoToAIData(aiData));
}

// Secondary AI data publisher
AISecondaryDataPublisher::AISecondaryDataPublisher() : utils::Publisher(utils::ChannelType::AI_SECONDARY_DATA_CHANNEL) {}

bool AISecondaryDataPublisher::publish(const rtt::AIData& data) {
    auto protoRobotCommands = AIDataToProto(data);
    return this->send(protoRobotCommands.SerializeAsString());
}

// Secondary AI data subscriber
AISecondaryDataSubscriber::AISecondaryDataSubscriber(const std::function<void(const rtt::AIData&)>& callback)
    : utils::Subscriber(utils::ChannelType::AI_SECONDARY_DATA_CHANNEL, [&](const std::string& message) { this->onPublishedMessage(message); }), callback(callback) {
    if (callback == nullptr) {
        throw utils::InvalidCallbackException("Callback was nullptr");
    }
}

void AISecondaryDataSubscriber::onPublishedMessage(const std::string& message) {
    proto::AIData aiData;
    aiData.ParseFromString(message);
    this->callback(protoToAIData(aiData));
}

}  // namespace rtt::net