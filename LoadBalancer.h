#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <fstream>

#include "WebServer.h"
using namespace std;

class LoadBalancer {
public:
    std::atomic<bool> shutdown{false};
    std::queue<std::shared_ptr<Request>> requestQueue;  // Use smart pointers
    std::vector<std::shared_ptr<WebServer>> available_WebServers;
    std::vector<std::shared_ptr<WebServer>> unavailable_WebServers;
    std::ofstream& outputFile;

    std::mutex mtx;  // Mutex to protect shared resources

    LoadBalancer(int numServers, std::ofstream& outputFile);
    
    void addServer(int num);
    void removeServer(int num);
    void run();

};