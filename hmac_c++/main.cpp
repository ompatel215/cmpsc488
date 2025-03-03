#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <random>
#include <vector>

#define PORT 8080
#define ITERATIONS 1000
#define NUM_MESSAGES 100
#define MESSAGE_LENGTH 20

using namespace std;

string compute_hash(const string& key, const string& data) {
    string combined = key + data;
    hash<string> hasher;
    size_t hash = hasher(combined);
    return to_string(hash);
}

void benchmark_hash(const string& key, const string& data) {
    auto start = chrono::high_resolution_clock::now();
    
    for(int i = 0; i < ITERATIONS; i++) {
        compute_hash(key, data);
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;
    
    cout << "Benchmark Results:" << endl;
    cout << "Total time: " << elapsed.count() << " ms" << endl;
    cout << "Average time per hash: " << elapsed.count() / ITERATIONS << " ms" << endl;
}

void server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket creation failed" << endl;
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "setsockopt failed" << endl;
        return;
    }
    
    if (::bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed: " << strerror(errno) << endl;
        return;
    }
    
    if (listen(server_fd, 3) < 0) {
        cerr << "Listen failed" << endl;
        return;
    }
    
    cout << "Server listening on port " << PORT << endl;
    
    while(true) {
        int socket = accept(server_fd, nullptr, nullptr);
        
        while(true) {
            char buffer[1024] = {0};
            ssize_t bytes_read = read(socket, buffer, 1024);
            
            if (bytes_read <= 0) break;  // Client disconnected
            
            string data(buffer);
            string key = "secret-key";
            string hash = compute_hash(key, data);
            string response = data + ":" + hash;
            
            send(socket, response.c_str(), response.length(), 0);
        }
        
        close(socket);
    }
}

// Function to generate random string
string generate_random_string(int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, sizeof(charset) - 2);
    
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += charset[distribution(generator)];
    }
    return result;
}

void benchmark_client() {
    vector<double> round_trip_times;
    vector<string> messages;
    
    // Generate random messages beforehand
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        messages.push_back(generate_random_string(MESSAGE_LENGTH));
    }
    
    // Create single connection
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket creation failed" << endl;
        return;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address" << endl;
        close(sock);
        return;
    }
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection failed" << endl;
        close(sock);
        return;
    }
    
    auto total_start = chrono::high_resolution_clock::now();
    
    // Send all messages over same connection
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        auto start = chrono::high_resolution_clock::now();
        
        send(sock, messages[i].c_str(), messages[i].length(), 0);
        
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed = end - start;
        round_trip_times.push_back(elapsed.count());
    }
    
    close(sock);
    
    // Calculate statistics
    double avg_time = 0;
    double min_time = round_trip_times[0];
    double max_time = round_trip_times[0];
    
    for (double time : round_trip_times) {
        avg_time += time;
        min_time = min(min_time, time);
        max_time = max(max_time, time);
    }
    avg_time /= NUM_MESSAGES;
    
    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> total_elapsed = total_end - total_start;
    
    // Print results
    cout << "\nBenchmark Results:" << endl;
    cout << "Total messages sent: " << NUM_MESSAGES << endl;
    cout << "Total time: " << total_elapsed.count() << " ms" << endl;
    cout << "Average round-trip time: " << avg_time << " ms" << endl;
    cout << "Min round-trip time: " << min_time << " ms" << endl;
    cout << "Max round-trip time: " << max_time << " ms" << endl;
    cout << "Throughput: " << (NUM_MESSAGES / (total_elapsed.count() / 1000)) << " messages/second" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <server|client>" << endl;
        return 1;
    }
    
    string mode = argv[1];
    if (mode == "server") {
        server();
    } else if (mode == "client") {
        benchmark_client();
    } else {
        cerr << "Invalid mode. Use 'server' or 'client'" << endl;
        return 1;
    }
    return 0;
}
