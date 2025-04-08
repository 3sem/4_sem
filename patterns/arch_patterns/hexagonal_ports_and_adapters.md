# Hexagonal (Ports & Adapters) Microservice Architecture in C++

Here's a rewrite of the previous example following Hexagonal Architecture principles:

```cpp
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include <functional>

// ========================
// Domain Layer (Center)
// ========================
namespace Domain {
    struct User {
        int id;
        std::string name;
        std::string email;
        
        bool isValid() const {
            return !name.empty() && email.find('@') != std::string::npos;
        }
    };

    // Port (Interface)
    class IUserRepository {
    public:
        virtual ~IUserRepository() = default;
        virtual User getUser(int id) const = 0;
        virtual void saveUser(const User& user) = 0;
        virtual bool exists(const std::string& email) const = 0;
    };

    // Port (Interface)
    class IUserNotifier {
    public:
        virtual ~IUserNotifier() = default;
        virtual void notifyUserCreated(const User& user) = 0;
    };
}

// ========================
// Application Layer
// ========================
namespace Application {
    class UserService {
        std::unique_ptr<Domain::IUserRepository> repository;
        std::unique_ptr<Domain::IUserNotifier> notifier;
        
        int generateId() const {
            static int counter = 0;
            return ++counter;
        }
        
    public:
        UserService(std::unique_ptr<Domain::IUserRepository> repo,
                   std::unique_ptr<Domain::IUserNotifier> notify)
            : repository(std::move(repo)), notifier(std::move(notify)) {}
        
        Domain::User getUser(int id) const {
            return repository->getUser(id);
        }
        
        Domain::User createUser(const std::string& name, const std::string& email) {
            if (repository->exists(email)) {
                throw std::runtime_error("Email already registered");
            }
            
            Domain::User newUser{
                generateId(),
                name,
                email
            };
            
            if (!newUser.isValid()) {
                throw std::runtime_error("Invalid user data");
            }
            
            repository->saveUser(newUser);
            notifier->notifyUserCreated(newUser);
            return newUser;
        }
    };
}

// ========================
// Adapters (Left Side - Driving)
// ========================
namespace Adapters {
    // CLI Controller (Driving Adapter)
    class CommandLineInterface {
        Application::UserService& service;
        
    public:
        explicit CommandLineInterface(Application::UserService& srv) : service(srv) {}
        
        void run() {
            std::cout << "User Service CLI\n";
            std::cout << "1. Create user\n2. Get user\nChoice: ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) {
                std::string name, email;
                std::cout << "Name: ";
                std::cin >> name;
                std::cout << "Email: ";
                std::cin >> email;
                
                try {
                    auto user = service.createUser(name, email);
                    std::cout << "Created user ID: " << user.id << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
            } else if (choice == 2) {
                int id;
                std::cout << "User ID: ";
                std::cin >> id;
                
                try {
                    auto user = service.getUser(id);
                    std::cout << "User: " << user.name << " <" << user.email << ">\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << "\n";
                }
            }
        }
    };

    // HTTP Controller (Alternative Driving Adapter)
    class HttpController {
        Application::UserService& service;
        
    public:
        explicit HttpController(Application::UserService& srv) : service(srv) {}
        
        void handleRequest(const std::string& path, const std::map<std::string, std::string>& params) {
            if (path == "/users" && params.find("name") != params.end()) {
                try {
                    auto user = service.createUser(params.at("name"), params.at("email"));
                    std::cout << "[HTTP] Created user: " << user.id << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[HTTP] Error: " << e.what() << "\n";
                }
            }
            // ... other routes
        }
    };
}

// ========================
// Adapters (Right Side - Driven)
// ========================
namespace Adapters {
    // InMemory Repository (Driven Adapter)
    class InMemoryUserRepository : public Domain::IUserRepository {
        std::map<int, Domain::User> users;
        std::map<std::string, int> emailIndex;
        
    public:
        Domain::User getUser(int id) const override {
            auto it = users.find(id);
            if (it == users.end()) {
                throw std::runtime_error("User not found");
            }
            return it->second;
        }
        
        void saveUser(const Domain::User& user) override {
            users[user.id] = user;
            emailIndex[user.email] = user.id;
        }
        
        bool exists(const std::string& email) const override {
            return emailIndex.find(email) != emailIndex.end();
        }
    };

    // Console Notifier (Driven Adapter)
    class ConsoleNotifier : public Domain::IUserNotifier {
    public:
        void notifyUserCreated(const Domain::User& user) override {
            std::cout << "Notification: User created - " << user.name << "\n";
        }
    };

    // Email Notifier (Alternative Driven Adapter)
    class EmailNotifier : public Domain::IUserNotifier {
    public:
        void notifyUserCreated(const Domain::User& user) override {
            std::cout << "[EMAIL] Sent welcome to: " << user.email << "\n";
        }
    };
}

// ========================
// Main Composition
// ========================
class UserMicroservice {
    std::unique_ptr<Application::UserService> service;
    std::unique_ptr<Adapters::CommandLineInterface> cli;
    std::unique_ptr<Adapters::HttpController> http;
    
public:
    UserMicroservice() {
        // Instantiate driven adapters (right side)
        auto repository = std::make_unique<Adapters::InMemoryUserRepository>();
        auto notifier = std::make_unique<Adapters::ConsoleNotifier>();
        
        // Create core service
        service = std::make_unique<Application::UserService>(
            std::move(repository),
            std::move(notifier)
        );
        
        // Instantiate driving adapters (left side)
        cli = std::make_unique<Adapters::CommandLineInterface>(*service);
        http = std::make_unique<Adapters::HttpController>(*service);
    }
    
    void runCLI() {
        cli->run();
    }
    
    void simulateHttpRequest() {
        std::map<std::string, std::string> params{
            {"name", "HTTP User"},
            {"email", "http@example.com"}
        };
        http->handleRequest("/users", params);
    }
};

int main() {
    UserMicroservice microservice;
    
    // Run CLI interface
    microservice.runCLI();
    
    // Simulate HTTP request
    microservice.simulateHttpRequest();
    
    return 0;
}
```

## Key Differences from Onion Architecture

1. **Explicit Ports & Adapters**:
   - **Ports** (interfaces) are defined in the domain layer
   - **Adapters** implement these interfaces on both sides:
     - *Driving* (left side): CLI, HTTP controllers (initiate actions)
     - *Driven* (right side): Repositories, notifiers (respond to actions)

2. **Symmetrical Architecture**:
   ```
   [Driving Adapters] → [Application Core] ← [Driven Adapters]
          (CLI/HTTP)       (UserService)      (Database/Notification)
   ```

3. **Technology-Agnostic Core**:
   - Domain and application layers have no knowledge of:
     - How users interact with the system (CLI vs HTTP)
     - How data is stored (in-memory vs database)
     - How notifications are sent (console vs email)

4. **Multiple Entry Points**:
   - Easily support different interfaces (CLI, HTTP, etc.) simultaneously
   - New adapters can be added without changing core logic

5. **Clear Dependency Flow**:
   - All dependencies point inward toward the domain
   - Adapters depend on ports, not vice versa

## Hexagonal Architecture Benefits

1. **Framework Independence**:
   - Swap web frameworks or databases without business logic changes

2. **Testability**:
   - Mock any port for isolated testing
   - Test core logic without infrastructure

3. **Flexibility**:
   - Add new interfaces (gRPC, message queues) via new adapters
   - Change notification system without touching business rules

4. **Clear Boundaries**:
   - Explicit separation of concerns
   - Each component has a single responsibility

This implementation maintains the core functionality while being more adaptable to different technologies and interfaces than the original Onion Architecture version.
