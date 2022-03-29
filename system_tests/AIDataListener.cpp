#include <AIDataNetworker.hpp>

#include <roboteam_utils/Format.hpp>

#include <iostream>
#include <chrono>
#include <algorithm>

void onAIData(const rtt::AIData &data) {

    // Line 0
    std::cout << "Plays (" << data.allPlays.size() << "):";
    for (const auto& playName : data.allPlays) {
        std::cout << " " << playName;
    }
    std::cout << std::endl;
    // Line 1
    std::cout << "Current play: " << data.currentPlay << std::endl;

    std::vector<int> mapKeys;
    for (const auto& [id, _] : data.robotStpStatus) {
        mapKeys.push_back(id);
    }

    constexpr int MAX_COLUMNS = 3;
    for (int i = 0; i < mapKeys.size(); i+=MAX_COLUMNS) {
        // print roles
        for (int j = i; j < std::min((int)mapKeys.size(), i + MAX_COLUMNS); j++) {
            int id = mapKeys.at(j);
            rtt::RobotSTPStatus status = data.robotStpStatus.at(id);

            std::cout << id << ") R: " << rtt::formatString("%-17s : %7s", status.roleName.c_str(), status.roleStatus.c_str()) << " | ";
        }
        std::cout << std::endl;

        // print tactics
        for (int j = i; j < std::min((int)mapKeys.size(), i + MAX_COLUMNS); j++) {
            int id = mapKeys.at(j);
            rtt::RobotSTPStatus status = data.robotStpStatus.at(id);
            std::cout << "   T: " << rtt::formatString("%-17s : %7s", status.tacticName.c_str(), status.tacticStatus.c_str()) << " | ";
        }
        std::cout << std::endl;

        // print skills
        for (int j = i; j < std::min((int)mapKeys.size(), i + MAX_COLUMNS); j++) {
            int id = mapKeys.at(j);
            rtt::RobotSTPStatus status = data.robotStpStatus.at(id);
            std::cout << "   S: " << rtt::formatString("%-17s : %7s", status.skillName.c_str(), status.skillStatus.c_str()) << " | ";
        }
        std::cout << std::endl;
    }
}

int main() {

    rtt::net::AIPrimaryDataSubscriber subscriber(onAIData);

    //std::cout << "Hello there!" << std::flush;
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    //std::cout << "\rGeneral kenobi!" << std::flush;

//    printf("\\033[F test");
//    fflush(std::cout);

    std::cout << "Hello" << std::endl << "There" << std::endl << "General" << std::endl << "Kenobi" << std::flush;
//    std::cout << "\033[F\033[Fhello there" << std::flush;
//    std::cout << "\033[A" << "Test" << std::flush;



    int i = 0;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //printf("\33[2K\r");
        //printf("Hello there %d \n", i);

        i++;
    }
}