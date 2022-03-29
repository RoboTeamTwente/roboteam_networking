#include <proto/AIData.pb.h>

#include <AIDataNetworker.hpp>

namespace rtt::net {

proto::AIData AIDataToProto(const rtt::AIData& data) {
    proto::AIData proto;

    proto.set_current_play(data.currentPlay);

    for (const auto &playName : data.allPlays) {
        proto.add_all_plays(playName);
    }

    for (const auto& [robotId, status] : data.robotStpStatus) {
        auto protoStatus = proto.add_stp_status();
        protoStatus->set_robot_id(robotId);
        protoStatus->set_role_name(status.roleName);
        protoStatus->set_role_status(status.roleStatus);
        protoStatus->set_tactic_name(status.tacticName);
        protoStatus->set_tactic_status(status.tacticStatus);
        protoStatus->set_skill_name(status.skillName);
        protoStatus->set_skill_status(status.skillStatus);
    }

    for (const auto& [robotId, path] : data.robotPaths) {
        auto protoPath = proto.add_robot_paths();
        protoPath->set_robot_id(robotId);

        for (const auto& point : path) {
            auto protoPoint = protoPath->add_points();
            protoPoint->set_x(point.x);
            protoPoint->set_y(point.y);
        }
    }

    return proto;
}

AIData protoToAIData(const proto::AIData& proto) {
    AIData aiData;

    aiData.currentPlay = std::string(proto.current_play());

    for (const auto &playName : proto.all_plays()) {
        aiData.allPlays.push_back(std::string(playName));
    }

    for (const auto& protoStatus : proto.stp_status()) {
        rtt::RobotSTPStatus status = {
            .roleName = std::string(protoStatus.role_name()),
            .roleStatus = std::string(protoStatus.role_status()),
            .tacticName = std::string(protoStatus.tactic_name()),
            .tacticStatus = std::string(protoStatus.tactic_status()),
            .skillName = std::string(protoStatus.skill_name()),
            .skillStatus = std::string(protoStatus.skill_status())
        };
        aiData.robotStpStatus.insert({ protoStatus.robot_id(), status });
    }

    for (const auto& protoPath : proto.robot_paths()) {
        std::vector<Vector2> points;
        for (const auto& protoPoint : protoPath.points()) {
            points.emplace_back(Vector2(protoPoint.x(), protoPoint.y()));
        }
        aiData.robotPaths.insert({ protoPath.robot_id(), points});
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