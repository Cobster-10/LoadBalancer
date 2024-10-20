#include <iostream>
#include <thread>
#include <mutex>
#include "LoadBalancer.h"

using namespace std;

LoadBalancer::LoadBalancer(int numServers, std::ofstream& outputFile) : outputFile(outputFile) {
    for (int i = 0; i < numServers; i++) {
        auto server = std::make_shared<WebServer>();
        available_WebServers.push_back(server);
    }
}

void LoadBalancer::addServer(int num) {
    for (int i = 0; i < num; i++) {
        auto server = std::make_shared<WebServer>();
        available_WebServers.push_back(server);
    }
}

void LoadBalancer::removeServer(int num) {
    
    int len = std::min(num, static_cast<int>(available_WebServers.size()));
    for (int i = 0; i < len; i++) {
        available_WebServers.erase(available_WebServers.begin());  // Smart pointers manage memory
    }
}

void LoadBalancer::run() {
    int count = 0;
    while (!shutdown || !requestQueue.empty()) {
        count++;
        {
            std::lock_guard<std::mutex> lock(mtx);  // Protect the entire block

            // Adjust the number of servers based on the request queue size
            if (count % 10 == 0) {
                outputFile << "request queue size " << requestQueue.size() << endl;
                outputFile << "available servers " << available_WebServers.size() << " unavailable servers " <<  unavailable_WebServers.size() << " sum "<< available_WebServers.size() + unavailable_WebServers.size() << endl;
                int num_servers = available_WebServers.size() + unavailable_WebServers.size();
                int ideal_num_servers = (requestQueue.size() / 3) +1;
                if (num_servers < ideal_num_servers) {
                
                    this->addServer(ideal_num_servers - num_servers);
                } else if (num_servers > ideal_num_servers) {
                  
                    this->removeServer(num_servers - ideal_num_servers);
                }
            }

            // Move available servers from the unavailable list back to available list
            for (auto it = unavailable_WebServers.begin(); it != unavailable_WebServers.end();) {
                if ((*it)->availability) {
                    available_WebServers.push_back(*it);
                    it = unavailable_WebServers.erase(it);
                } else {
                    ++it;
                }
            }
        }

        if (requestQueue.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Avoid busy-waiting
            continue;
        }

        std::shared_ptr<Request> request;
        std::shared_ptr<WebServer> server;

        {
            // Protect shared resources with a lock
            std::lock_guard<std::mutex> lock(mtx);
            request = requestQueue.front();
            requestQueue.pop();

            if (!available_WebServers.empty()) {
                server = available_WebServers.back();
                available_WebServers.pop_back();
                unavailable_WebServers.push_back(server);
            }
        }

        if (server) {
            // Process request in a new thread without locking
            std::thread t(&WebServer::processRequest, server, request, std::ref(outputFile));
            t.detach();  // Detached thread
        }
    }
    
}
