

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <string>

struct Request {
    int IPin;
    int IPout;
    int Time;
    std::string JobType;

    Request(int IPin, int IPout, int Time, std::string JobType){
        this->IPin = IPin;
        this->IPout = IPout;
        this->Time = Time;
        this->JobType = JobType;
    }
};

class WebServer {

    public:
    bool availability; 
    
    
    void processRequest(std::shared_ptr<Request> request, std::ofstream& outputFile);

    void askRequest();

};

#endif