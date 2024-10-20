#include <iostream>
#include <thread>
#include "LoadBalancer.h"
#include <mutex>
#include <fstream>

using namespace std;

std::mutex mtx;  // Mutex to protect shared resources

using namespace std;

int generateRandomIPv4() {
    int firstOctet = 192 + (rand() % 32); // Class C first octet (192 to 223)
    int secondOctet = rand() % 256; // Random number between 0 and 255
    int thirdOctet = rand() % 256; // Random number between 0 and 255
    int fourthOctet = rand() % 256; // Random number between 0 and 255

    return (firstOctet << 24) | (secondOctet << 16) | (thirdOctet << 8) | fourthOctet;
}

int generateRandomTime() {
    return rand() % 500 + 1; // Random time between 1 and 500
}

std::string generateRandomJobType() {
    return rand() % 2 == 0 ? "Streaming" : "Processing"; // Random job type
}

int main(int argc, char *argv[]) {
    if (argc != 3) {  // Expecting two arguments: time and numServers
        std::cerr << "Usage: " << argv[0] << " <time> <numServers>" << std::endl;
        return 1;
    }

    ofstream outputFile("output_log.txt");
    if (!outputFile) {
        std::cerr << "Error: failed to open output file - output_log.txt" << std::endl;
        return 1;
    }

    int time = std::atoi(argv[1]);       // Convert first argument to int (time)
    int numServers = std::atoi(argv[2]);
    
    // Create the load balancer with the specified number of servers
    LoadBalancer lb1(numServers, std::ref(outputFile));
    
    // Create a thread to run the load balancer
    std::thread lbThread(&LoadBalancer::run, &lb1);
    
    // Populate the request queue initially with 100*numServers requests
    for(int i = 0; i < 100 * numServers; i++) {
        int IPin = generateRandomIPv4();
        int IPout = generateRandomIPv4();
        int Time = generateRandomTime();
        string JobType = generateRandomJobType();

        std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex for thread safety
        Request* request = new Request(IPin, IPout, Time, JobType);     
        lb1.requestQueue.push(std::shared_ptr<Request>(request));  // Use shared_ptr
    }

    // Continue to send requests for 'time' seconds
    for(int i = 0; i < time; i++) {
        int IPin = generateRandomIPv4();
        int IPout = generateRandomIPv4();
        int Time = generateRandomTime();
        string JobType = generateRandomJobType();

        std::lock_guard<std::mutex> lock(mtx);  // Protect request queue access
        Request* request = new Request(IPin, IPout, Time, JobType);
        lb1.requestQueue.push(std::shared_ptr<Request>(request));  // Use smart pointer to manage memory

        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Sleep for 100 ms
    }    

    // Signal shutdown to the load balancer
    lb1.shutdown = true;

    outputFile << "Time is up, waiting for all requests to be processed..." << endl;
    // Wait for the load balancer thread to finish
    lbThread.join();  // Wait for the load balancer to complete its task

    outputFile << "All requests processed, exiting..." << std::endl;

    outputFile << "Starting Queue Size: " << 100 * numServers << std::endl;
    outputFile << "Final Queue Size: " << lb1.requestQueue.size() << std::endl;
    
    
    return 0;
}
