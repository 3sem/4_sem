#include <iostream>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <typeindex>

class EventBus {
public:
    template<typename EventType>
    using Handler = std::function<void(const EventType&)>;

    // Subscribe to an event type
    template<typename EventType>
    void subscribe(Handler<EventType> handler) {
        auto wrapper = [handler](const void* event) {
            handler(*static_cast<const EventType*>(event));
        };
        subscribers_[typeid(EventType)].push_back(wrapper);
    }

    // Publish an event
    template<typename EventType>
    void publish(const EventType& event) {
        auto it = subscribers_.find(typeid(EventType));
        if (it != subscribers_.end()) {
            for (auto& subscriber : it->second) {
                subscriber(&event);
            }
        }
    }

private:
    std::map<std::type_index, std::vector<std::function<void(const void*)>>> subscribers_;
};

// Example Events
struct UserLoggedInEvent { std::string username; };
struct OrderPlacedEvent { int orderId; double amount; };

// Example Subscriber
class Logger {
public:
    explicit Logger(EventBus& bus) {
        bus.subscribe<UserLoggedInEvent>([](const UserLoggedInEvent& e) {
            std::cout << "[LOG] User logged in: " << e.username << std::endl;
        });
    }
};

class Analytics {
public:
    explicit Analytics(EventBus& bus) {
        bus.subscribe<OrderPlacedEvent>([](const OrderPlacedEvent& e) {
            std::cout << "[ANALYTICS] Order #" << e.orderId 
                      << " for $" << e.amount << std::endl;
        });
    }
};

int main() {
    EventBus bus;
    
    Logger logger(bus);
    Analytics analytics(bus);
    
    bus.publish(UserLoggedInEvent{"john_doe"});
    bus.publish(OrderPlacedEvent{1001, 49.99});
    
    return 0;
}
