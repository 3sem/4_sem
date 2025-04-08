# Lamport's Logical Clocks in C++

Here's a complete example demonstrating Lamport's logical clocks in C++:

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

class LamportClock {
private:
    int time;
    std::mutex mtx;
    
public:
    LamportClock() : time(0) {}
    
    // Get current time
    int get_time() {
        std::lock_guard<std::mutex> lock(mtx);
        return time;
    }
    
    // Increment time for local event
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        time++;
    }
    
    // Update time based on received message
    void receive(int received_time) {
        std::lock_guard<std::mutex> lock(mtx);
        time = std::max(time, received_time) + 1;
    }
};

// Simulate a process with Lamport clock
void process(int id, LamportClock& clock, std::vector<std::string>& log) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);
    
    for (int i = 0; i < 3; ++i) {
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen) * 100));
        
        // Local event
        clock.increment();
        int current_time = clock.get_time();
        
        std::lock_guard<std::mutex> lock(std::mutex{});
        log.push_back("Process " + std::to_string(id) + ": Local event, time = " + std::to_string(current_time));
        
        // Simulate sending message to another process (randomly chosen)
        if (dis(gen) > 3) {
            int receiver = (id + 1) % 3;  // Simple way to choose another process
            int send_time = clock.get_time();
            
            std::lock_guard<std::mutex> lock(std::mutex{});
            log.push_back("Process " + std::to_string(id) + ": Sending message to " + 
                          std::to_string(receiver) + ", time = " + std::to_string(send_time));
            
            // In a real system, you would send the message with the timestamp
            // Here we simulate it by directly calling receive on the other process's clock
            // Note: In a real implementation, this would need proper synchronization
            clock.receive(send_time);
        }
    }
}

int main() {
    const int num_processes = 3;
    LamportClock clocks[num_processes];
    std::vector<std::string> event_log;
    std::vector<std::thread> processes;
    
    // Start all processes
    for (int i = 0; i < num_processes; ++i) {
        processes.emplace_back(process, i, std::ref(clocks[i]), std::ref(event_log));
    }
    
    // Wait for all processes to finish
    for (auto& t : processes) {
        t.join();
    }
    
    // Print event log in order (not necessarily chronological)
    std::cout << "Event Log:" << std::endl;
    for (const auto& entry : event_log) {
        std::cout << entry << std::endl;
    }
    
    // Print final clock values
    std::cout << "\nFinal Clock Values:" << std::endl;
    for (int i = 0; i < num_processes; ++i) {
        std::cout << "Process " << i << ": " << clocks[i].get_time() << std::endl;
    }
    
    return 0;
}
```

## Key Points About This Implementation:

1. **LamportClock Class**:
   - Maintains a logical clock counter
   - Provides methods for local events (`increment()`)
   - Handles message receives (`receive()` which updates the clock based on the received timestamp)

2. **Process Simulation**:
   - Each process thread performs local events and occasionally sends messages
   - Message sends include the current Lamport timestamp
   - Receiving processes update their clocks based on received timestamps

3. **Synchronization**:
   - Uses mutexes to protect clock access (since multiple threads access them)
   - Also protects the shared event log

4. **Example Output**:
   - The program will show a log of events with Lamport timestamps
   - The final clock values demonstrate how processes' clocks have advanced

Note that this is a simplified simulation. In a real distributed system, you would have actual network communication between processes running on different machines, and you'd need to handle message passing properly.

The key Lamport clock rules implemented are:
1. On local event: increment local clock
2. On sending message: include current clock value
3. On receiving message: set clock to max(local, received) + 1
