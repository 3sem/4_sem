// Inspired by: https://github.com/alexyer/lamport_clock
#ifndef LAMPORT_CLOCK_LAMPORT_CLOCK_H
#define LAMPORT_CLOCK_LAMPORT_CLOCK_H

#include <atomic>

class LamportClock {
public:
    // Timestamp
    typedef unsigned int LamportTime;

    LamportClock() : time_(0) {}

    /**
     * Get current Lamport timestamp.
     *
     * @return LamportTime value.
     */
    LamportTime get_time() const {
        return time_.load();
    }

    /**
     * Handle local event.
     * Increment timer and return the new value.
     *
     * @return LamportTime value;
     */
    LamportTime local_event() {
        return time_.fetch_add(1); // Atomically replaces the current value with the result of arithmetic addition of the value and arg
    }

    /**
     * Handle send event.
     * Increment local time and return the new value.
     *
     * @return LamportTime value;
     */
    LamportTime send_event() {
        return local_event();
    }

    /**
     * Handle receive event.
     * Formula:
     * local_time <-- max(local_time+1, received_time)
     * @param received_time Sender's time.
     * @return LamportTime value;
     */
    LamportTime receive_event(LamportTime received_time) {
        do {
            //RECEIVE_EVENT:
            auto current_time = get_time();
            // If received time is old, do nothing.
            if (received_time < current_time) {
                return time_.fetch_add(1);;
            }
        // Ensure that local timer is at least one ahead.
        } while (!time_.compare_exchange_strong(current_time, received_time + 1));  // https://vk.com/@habr-kak-rabotat-s-atomarnymi-tipami-dannyh-v-c

        return received_time + 1;
    }

private:
    std::atomic<LamportTime> time_; // cppreference.com : If one thread writes to an atomic object while another thread reads from it,
                                    // the behavior is well-defined (see memory model for details on data races).
                                    // Non-functional side effect: operations with atomics are much slower, than with ordinary types
};

#endif //LAMPORT_CLOCK_LAMPORT_CLOCK_H
