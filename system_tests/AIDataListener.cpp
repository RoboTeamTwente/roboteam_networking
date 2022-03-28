#include <AIDataNetworker.hpp>

#include <iostream>
#include <chrono>

void onAIData(const rtt::AIData &data) {
    std::cout << "Data: " << std::endl;
    std::cout << "- Current play: " << data.currentPlay << std::endl;
    std::cout << "- Plays size: " << data.allPlays.size() << std::endl;
}

int main() {

    rtt::net::AIPrimaryDataSubscriber subscriber(onAIData);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Tick" << std::endl;
    }
}