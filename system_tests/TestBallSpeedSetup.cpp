#include <RobotCommandsNetworker.hpp>
#include <RobotFeedbackNetworker.hpp>
#include <SettingsNetworker.hpp>
#include <WorldNetworker.hpp>
#include <SimulationConfigurationNetworker.hpp>
#include <chrono>
#include <iostream>
#include <memory>

#include <roboteam_utils/Vector2.h>

using namespace rtt::net;

bool robotHasBall = false;
rtt::Vector2 robotPosition(4, 2);
double xBall = robotPosition.x - 0.085;
double dribblerSpeed = 100;
double kickSpeed = 3.0;

rtt::RobotCommands getEmptyRobotCommandToAllRobots() {
    rtt::RobotCommands commands;
    for (int i = 0; i < 16; ++i) {
        commands.push_back({.id = i});
    }
    return commands;
}

proto::Setting getRobotHubBasestationModePacket() {
    proto::Setting settings;

    settings.set_serialmode(false);

    return settings;
}

void onFeedback(const rtt::RobotsFeedback& robotsFeedback) {
//    int amountOfFeedback = robotsFeedback.feedback.size();
//    bool isYellow = robotsFeedback.team == rtt::Team::YELLOW;
//
//    std::cout << "Received " << amountOfFeedback << " feedbacks of team " << (isYellow ? "yellow" : "blue") << std::endl;
//
    bool oneHasBall = false;
    for (const auto& feedback : robotsFeedback.feedback) {
        int id = feedback.id;
        //bool xSensCalibrated = feedback.xSensIsCalibrated;
        //bool ballsensorworking = feedback.ballSensorIsWorking;
        if (feedback.hasBall) oneHasBall = true;
    }
    robotHasBall = oneHasBall;
    //std::cout << "Has ball: " << std::boolalpha << robotHasBall << std::endl;
}

void onWorld(const proto::State& world) {
//    for (const auto& robot : world.last_seen_world().yellowfeedback()) {
        // std::cout << "Robot " << robot.id() << " of team yellow has " << (robot.hasball() ? "" : "not ") << "the ball" << std::endl;
//    }

//    for (const auto& robot : world.last_seen_world().bluefeedback()) {
        // std::cout << "Robot " << robot.id() << " of team blue has " << (robot.hasball() ? "" : "not ") << "the ball" << std::endl;
//    }
}


rtt::RobotCommands getHaltCommand() {
    rtt::RobotCommand command;
    command.id = 0;
    command.dribblerSpeed = dribblerSpeed;
    command.cameraAngleOfRobotIsSet = false;
    command.kickSpeed = 0.0;
    command.kickType = rtt::KickType::KICK;
    command.targetAngle = 0.0;
    command.targetAngularVelocity = 0.0;
    command.velocity = {0.0, 0.0};
    command.waitForBall = false;
    command.useAngularVelocity = true;
    return { command };
}

rtt::RobotCommands getKickCommand(int id) {
    rtt::RobotCommand command;
    command.id = id;
    command.dribblerSpeed = dribblerSpeed;
    command.cameraAngleOfRobotIsSet = false;
    command.kickSpeed = kickSpeed;
    command.kickType = rtt::KickType::KICK;
    command.targetAngle = 0.0;
    command.targetAngularVelocity = 0.0;
    command.velocity = {0.0, 0.0};
    command.waitForBall = false;
    command.useAngularVelocity = true;
    return { command };
}

proto::SimulationRobotLocation getAbsentFieldRobotLocation(int id, bool isYellow) {
    proto::SimulationRobotLocation location;
    location.set_id(id);
    location.set_is_team_yellow(isYellow);
    location.set_present_on_field(false);
    return location;
}
proto::SimulationRobotLocation getTestRobotLocation() {
    proto::SimulationRobotLocation location;
    location.set_id(0);
    location.set_is_team_yellow(true);
    location.set_present_on_field(true);
    location.set_by_force(false);
    location.set_orientation(M_PI);
    location.set_x(robotPosition.x);
    location.set_y(robotPosition.y);
    return location;
}
proto::SimulationRobotLocation getBounceRobotLocation() {
    proto::SimulationRobotLocation location;
    location.set_id(0);
    location.set_is_team_yellow(false);
    location.set_present_on_field(true);
    location.set_by_force(false);
    location.set_orientation(-0.6);
    location.set_x(robotPosition.x - 3);
    location.set_y(robotPosition.y + 0.05);
    return location;
}
proto::SimulationRobotLocation getOutOfFieldLocation(int id, bool isYellow) {
    proto::SimulationRobotLocation location;
    location.set_id(id);
    location.set_is_team_yellow(isYellow);
    location.set_present_on_field(true);
    location.set_by_force(false);
    location.set_orientation(M_PI/2);
    location.set_x((id + 0.5) * (isYellow ? -1 : 1));
    location.set_y(-4);
    return location;
}

proto::SimulationBallLocation getKickBallStartLocation() {
    proto::SimulationBallLocation location;
    location.set_x(xBall - 0.5);
    location.set_y(robotPosition.y);
    location.set_z(0.0);
    location.set_x_velocity(1.0);
    location.set_y_velocity(0.0);
    location.set_z_velocity(0.0);
    location.set_by_force(false);
    location.set_teleport_safely(false);
    location.set_velocity_in_rolling(false);
    return location;
}

proto::SimulationConfiguration getYellowStraightBallSetup() {
    proto::SimulationConfiguration cmd;
    // Setup robot location
    cmd.add_robot_locations()->CopyFrom(getTestRobotLocation());
    // Get other robots out of the field
    for (int i = 1; i < 11; i++) {
        cmd.add_robot_locations()->CopyFrom(getAbsentFieldRobotLocation(i, true));
    }
    // Setup ball location
    cmd.mutable_ball_location()->CopyFrom(getKickBallStartLocation());
    return cmd;
}
proto::SimulationConfiguration getBlueStraightBallSetup() {
    proto::SimulationConfiguration cmd;
    // Get all robots out of the field, except for one
    for (int i = 1; i < 11; i++) {
        cmd.add_robot_locations()->CopyFrom(getAbsentFieldRobotLocation(i, false));
    }
    cmd.add_robot_locations()->CopyFrom(getOutOfFieldLocation(0, false));
    return cmd;
}

proto::SimulationConfiguration getBlueBounceBallSetup() {
    proto::SimulationConfiguration cmd;
    // Get all robots out of the field, except for one
    for (int i = 1; i < 11; i++) {
        cmd.add_robot_locations()->CopyFrom(getAbsentFieldRobotLocation(i, true));
    }
    // Get blue in the trajectory of the ball
    cmd.add_robot_locations()->CopyFrom(getBounceRobotLocation());

    return cmd;
}


int main() {
    auto settingsPub = std::make_unique<SettingsPublisher>();
    auto commandsBluePub = std::make_unique<RobotCommandsBluePublisher>();
    auto commandsYellowPub = std::make_unique<RobotCommandsYellowPublisher>();
    auto simulationConfigPub = std::make_unique<SimulationConfigurationPublisher>();
    auto feedbackSub = std::make_unique<RobotFeedbackSubscriber>(onFeedback);

    auto straightBallTest = [&](double _kickPower, double _dribblerSpeed) {
        std::cout << std::endl << " == Starting straight ball test with kickPower: " << _kickPower << ", dribblerSpeed: " << _dribblerSpeed << std::endl;
        std::cout << "Setting up test by teleporting robots and ball" << std::endl;
        dribblerSpeed = _dribblerSpeed;
        kickSpeed = _kickPower;
        simulationConfigPub->publish(getYellowStraightBallSetup());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        simulationConfigPub->publish(getBlueStraightBallSetup());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Trying to kick ball 20 times";
        for (int i = 0; i < 20; i++) {
            commandsYellowPub->publish(getKickCommand(0));
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Done" << std::endl;
    };

    auto bounceBallTest = [&](double _kickPower, double _dribblerSpeed) {
        std::cout << std::endl << " == Starting bounce ball test with kickPower: " << _kickPower << ", dribblerSpeed: " << _dribblerSpeed << std::endl;
        std::cout << "Setting up test by teleporting robots and ball" << std::endl;
        dribblerSpeed = _dribblerSpeed;
        kickSpeed = _kickPower;
        simulationConfigPub->publish(getYellowStraightBallSetup());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        simulationConfigPub->publish(getBlueBounceBallSetup());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Trying to kick ball 20 times";
        for (int i = 0; i < 20; i++) {
            commandsYellowPub->publish(getKickCommand(0));
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Done" << std::endl;
    };


    //auto worldSub = std::make_unique<WorldSubscriber>(onWorld);

    //auto teamCommand = getEmptyRobotCommandToAllRobots();

    // First, send settings message to robothub that it needs to forward messages to the basestation
    std::cout << "Started test..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Sending robothub mode settings message" << std::endl;
    settingsPub->publish(getRobotHubBasestationModePacket());
    std::this_thread::sleep_for(std::chrono::seconds(2));

    bool sendDribbler = true;

    std::thread dribblerThread([&]() {
        auto lastUpdate = std::chrono::steady_clock::now();
        std::cout << "Starting dribbler spam" << std::endl;
        while (sendDribbler) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
            if (duration > 500) {
                commandsYellowPub->publish(getHaltCommand());
                lastUpdate = now;
            }
        }
        std::cout << "Stopping dribbler spam" << std::endl;
    });


//    straightBallTest(1, 300);
//    straightBallTest(2, 300);
    straightBallTest(3, 300);
//    straightBallTest(4, 300);
//    straightBallTest(5, 300);

//    bounceBallTest(1, 300);
//    bounceBallTest(2, 300);
//    bounceBallTest(3, 300);
//    bounceBallTest(4, 300);
//    bounceBallTest(5, 300);

    /*
    std::cout << "Starting loop." << std::endl << std::endl;
    std::string userCmd;
    while (true) {

        std::cout << "What do you want to do? (robot does " << (robotHasBall ? "" : "not ") << "have the ball)" << std::endl;
        std::cin >> userCmd;

        if (userCmd == "reset") {
            std::cout << "Resetting simulator" << std::endl;
            simulationConfigPub->publish(getResetCommand());
        } else if (userCmd == "exit") {
            std::cout << "Stopping test. " << std::endl;
            break;
        } else if (userCmd == "kick") {
            std::cout << "Sending kick command. (dribbler: " << dribblerSpeed << ", kick: " << kickSpeed <<")" << std::endl;
            commandsYellowPub->publish(getKickCommand());
        } else if (userCmd == "dribblerSpeed") {
            int val = 0;
            std::cout << "Enter number for dribbler speed: ";
            std::cin >> val;
            dribblerSpeed = static_cast<double>(val);
            std::cout << "Changed dribblerSpeed to: " << dribblerSpeed << std::endl;
            continue;
        } else if (userCmd == "kickSpeed") {
            int val = 0;
            std::cout << "Enter number for kick speed: ";
            std::cin >> val;
            kickSpeed = static_cast<double>(val);
            std::cout << "Changed kickSpeed to: " << dribblerSpeed << std::endl;
            continue;
        } else {
            std::cout << "Unknown command" << std::endl;
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    */

    sendDribbler = false;
    dribblerThread.join();
}