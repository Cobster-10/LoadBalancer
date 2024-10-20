#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <fstream>
#include "WebServer.h"

using namespace std;

void WebServer::processRequest(std::shared_ptr<Request> request, std::ofstream& outputFile) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    outputFile << "Start processing request for " << request->IPin << " to " << request->IPout
         << " at time " << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << endl;
    
    for (int i = 0; i < request->Time; i++) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    now = std::chrono::system_clock::now();
    currentTime = std::chrono::system_clock::to_time_t(now);
    localTime = std::localtime(&currentTime);
    outputFile << "Finish processing request for " << request->IPin << " to " << request->IPout << "at time " << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << endl;
     

    this->askRequest();
}

void WebServer::askRequest() {
    availability = true;
}
