# Vector Clocks in C++

Here's a complete implementation of vector clocks (also called vector logical clocks), which extend Lamport's logical clocks to capture causality more accurately in distributed systems:

```cpp
#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <algorithm>

class VectorClock {
private:
    std::vector<int> clock;
    int process_id;
    int total_processes;
    std::mutex mtx;

public:
    VectorClock(int id, int num_processes) : process_id(id), total_processes(num_processes) {
        clock.resize(num_processes, 0);
    }

    // Get current vector clock
    std::vector<int> get_clock() {
        std::lock_guard<std::mutex> lock(mtx);
        return clock;
    }

    // Increment local process counter
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        clock[process_id]++;
    }

    // Update clock based on received message
    void receive(const std::vector<int>& received_clock) {
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < total_processes; ++i) {
            clock[i] = std::max(clock[i], received_clock[i]);
        }
        clock[process_id]++;
    }

    // Check if this clock is causally before another clock
    bool is_before(const std::vector<int>& other) const {
        bool at_least_one_less = false;
        for (int i = 0; i < total_processes; ++i) {
            if (clock[i] > other[i]) return false;
            if (clock[i] < other[i]) at_least_one_less = true;
        }
        return at_least_one_less;
    }

    // Check for concurrent events
    bool is_concurrent(const std::vector<int>& other) const {
        return !is_before(other) && !other.is_before(clock);
    }

    // Print the vector clock
    void print() const {
        std::cout << "[";
        for (int i = 0; i < total_processes; ++i) {
            std::cout << clock[i];
            if (i != total_processes - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
};

// Simulate a process with vector clock
void process(int id, VectorClock& clock, std::vector<std::string>& log, 
             std::map<int, VectorClock*>& all_clocks) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);
    
    for (int i = 0; i < 3; ++i) {
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen) * 100));
        
        // Local event
        clock.increment();
        auto current_clock = clock.get_clock();
        
        {
            std::lock_guard<std::mutex> lock(std::mutex{});
            log.push_back("Process " + std::to_string(id) + ": Local event, clock = ");
            // Store clock separately since we can't print directly in the log
            std::string clock_str = "[";
            for (int val : current_clock) {
                clock_str += std::to_string(val) + ",";
            }
            clock_str.back() = ']'; // Replace last comma
            log.back() += clock_str;
        }
        
        // Simulate sending message to another process (randomly chosen)
        if (dis(gen) > 3) {
            int receiver = (id + 1) % all_clocks.size();  // Choose another process
            auto send_clock = clock.get_clock();
            
            {
                std::lock_guard<std::mutex> lock(std::mutex{});
                log.push_back("Process " + std::to_string(id) + ": Sending message to " + 
                            std::to_string(receiver) + ", clock = ");
                std::string clock_str = "[";
                for (int val : send_clock) {
                    clock_str += std::to_string(val) + ",";
                }
                clock_str.back() = ']';
                log.back() += clock_str;
            }
            
            // Simulate message receive (in real system this would be network communication)
            all_clocks[receiver]->receive(send_clock);
        }
    }
}

int main() {
    const int num_processes = 3;
    std::vector<VectorClock> clocks;
    std::map<int, VectorClock*> all_clocks;
    
    // Initialize clocks
    for (int i = 0; i < num_processes; ++i) {
        clocks.emplace_back(i, num_processes);
        all_clocks[i] = &clocks[i];
    }
    
    std::vector<std::string> event_log;
    std::vector<std::thread> processes;
    
    // Start all processes
    for (int i = 0; i < num_processes; ++i) {
        processes.emplace_back(process, i, std::ref(clocks[i]), 
                             std::ref(event_log), std::ref(all_clocks));
    }
    
    // Wait for all processes to finish
    for (auto& t : processes) {
        t.join();
    }
    
    // Print event log
    std::cout << "Event Log:" << std::endl;
    for (const auto& entry : event_log) {
        std::cout << entry << std::endl;
    }
    
    // Print final clock values
    std::cout << "\nFinal Clock Values:" << std::endl;
    for (int i = 0; i < num_processes; ++i) {
        std::cout << "Process " << i << ": ";
        clocks[i].print();
        std::cout << std::endl;
    }
    
    // Demonstrate causality check
    std::cout << "\nCausality Checks:" << std::endl;
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_processes; ++j) {
            if (i == j) continue;
            bool before = clocks[i].is_before(clocks[j].get_clock());
            std::cout << "Clock " << i << (before ? " -> " : " !-> ") << "Clock " << j << std::endl;
        }
    }
    
    return 0;
}
```

## Key Features of This Vector Clock Implementation:

1. **VectorClock Class**:
   - Maintains a vector of integers (one counter per process)
   - Each process increments its own component on local events
   - On message receive, takes element-wise maximum with received clock and increments own counter

2. **Causality Tracking**:
   - `is_before()` implements the "happened-before" relation (v ≤ w iff v[i] ≤ w[i] for all i)
   - `is_concurrent()` detects when events are causally unrelated

3. **Process Simulation**:
   - Each process thread performs local events and occasionally sends messages
   - Message sends include the current vector clock
   - Receiving processes update their clocks using the element-wise maximum

4. **Synchronization**:
   - Uses mutexes to protect clock access in multi-threaded environment
   - Protects the shared event log

5. **Example Output**:
   - Shows event log with vector clock values
   - Displays final clock states
   - Demonstrates causality checks between processes

## Differences from Lamport Clocks:

1. **Causality Capture**:
   - Lamport clocks only provide partial ordering
   - Vector clocks provide complete causal history (can detect concurrent events)

2. **Storage Overhead**:
   - Lamport: single integer per process
   - Vector: N integers per process (where N is number of processes)

3. **Comparison Operations**:
   - Lamport: simple integer comparison
   - Vector: element-wise comparison of vectors

This implementation demonstrates the key advantage of vector clocks - the ability to precisely determine causal relationships between events in a distributed system, not just a total ordering.
