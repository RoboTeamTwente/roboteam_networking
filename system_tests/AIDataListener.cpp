#include <AIDataNetworker.hpp>

#include <roboteam_utils/Format.hpp>

#include <iostream>
#include <chrono>
#include <algorithm>
#include <sstream>

int previousLineCount = 0;

int dataReceived = 0;

void cursorUp(int lines) {
    std::stringstream ss;
    for (int i = 0; i < lines; i++) {
        ss << "\033[F";
    }
    std::cout << ss.str();
}

void clearLines(int lines) {
    std::stringstream ss;
    for (int line = 0; line < lines; line++) {
        ss << "                                                                                                           " << std::endl;
    }
    std::cout << ss.str();
}

void onAIData(const rtt::AIData &data) {

    std::stringstream ss;

    cursorUp(previousLineCount);
    clearLines(previousLineCount);
    cursorUp(previousLineCount);

    int lineCount = 0;
    dataReceived++;

    ss << "Received: " << dataReceived << std::endl; lineCount++;

    ss << "Plays (" << data.allPlays.size() << "):";
    for (const auto& playName : data.allPlays) {
        ss << " " << playName;
    }
    ss << std::endl; lineCount++;


    ss << "Current play: " << data.currentPlay << std::endl; lineCount++;

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

            ss << id << ") R: " << rtt::formatString("%-17s : %7s", status.roleName.c_str(), status.roleStatus.c_str()) << " | ";
        }
        ss << std::endl; lineCount++;

        // print tactics
        for (int j = i; j < std::min((int)mapKeys.size(), i + MAX_COLUMNS); j++) {
            int id = mapKeys.at(j);
            rtt::RobotSTPStatus status = data.robotStpStatus.at(id);
            ss << "   T: " << rtt::formatString("%-17s : %7s", status.tacticName.c_str(), status.tacticStatus.c_str()) << " | ";
        }
        ss << std::endl; lineCount++;

        // print skills
        for (int j = i; j < std::min((int)mapKeys.size(), i + MAX_COLUMNS); j++) {
            int id = mapKeys.at(j);
            rtt::RobotSTPStatus status = data.robotStpStatus.at(id);
            ss << "   S: " << rtt::formatString("%-17s : %7s", status.skillName.c_str(), status.skillStatus.c_str()) << " | ";
        }
        ss << std::endl; lineCount++;
    }



    previousLineCount = lineCount;
    std::cout << ss.str() << std::flush;
}

int main() {

    rtt::net::AIPrimaryDataSubscriber subscriber(onAIData);

    //std::cout << "Hello there!" << std::flush;
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    //std::cout << "\rGeneral kenobi!" << std::flush;

//    printf("\\033[F test");
//    fflush(std::cout);

    //std::cout << "Hello" << std::endl << "There" << std::endl << "General" << std::endl << "Kenobi" << std::flush;
//    std::cout << "\033[F\033[Fhello there" << std::flush;
//    std::cout << "\033[A" << "Test" << std::flush;



    int i = 0;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //printf("\33[2K\r");
        //printf("Hello there %d \n", i);

//        rtt::AIData testData = {
//            .currentPlay = "test!"
//        };
//
//        onAIData(testData);

        i++;
    }
}