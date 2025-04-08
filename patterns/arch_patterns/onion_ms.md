# Tiny Onion Microservice Architecture in C++ (No External Libraries)

Here's a minimal implementation of an onion architecture for microservices using only standard C++:

```cpp
#include <iostream>
#include <string>
#include <map>
#include <functional>

// ========================
// Core Domain Layer (Innermost)
// ========================
namespace Domain {
    struct User {
        int id;
        std::string name;
        std::string email;
    };

    class UserRepository {
    public:
        virtual ~UserRepository() = default;
        virtual User getUser(int id) = 0;
        virtual void saveUser(const User& user) = 0;
    };
}

// ========================
// Application Service Layer
// ========================
namespace Application {
    class UserService {
        Domain::UserRepository& repository;
    public:
        UserService(Domain::UserRepository& repo) : repository(repo) {}

        Domain::User getUser(int id) {
            return repository.getUser(id);
        }

        void createUser(const std::string& name, const std::string& email) {
            static int nextId = 1;
            Domain::User user{nextId++, name, email};
            repository.saveUser(user);
        }
    };
}

// ========================
// Infrastructure Layer (Outermost)
// ========================
namespace Infrastructure {
    class InMemoryUserRepository : public Domain::UserRepository {
        std::map<int, Domain::User> users;
    public:
        Domain::User getUser(int id) override {
            auto it = users.find(id);
            if (it == users.end()) {
                throw std::runtime_error("User not found");
            }
            return it->second;
        }

        void saveUser(const Domain::User& user) override {
            users[user.id] = user;
            std::cout << "Saved user: " << user.id << "\n";
        }
    };
}

// ========================
// API/Entry Point Layer
// ========================
class Microservice {
    Infrastructure::InMemoryUserRepository repository;
    Application::UserService service;
public:
    Microservice() : service(repository) {}

    void run() {
        // Simulate API endpoints
        std::cout << "Microservice running...\n";
        
        // Create some users
        service.createUser("Alice", "alice@example.com");
        service.createUser("Bob", "bob@example.com");
        
        // Get a user
        try {
            auto user = service.getUser(1);
            std::cout << "Retrieved user: " << user.name << " (" << user.email << ")\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
};

int main() {
    Microservice service;
    service.run();
    return 0;
}
```

## Key Features of This Onion Architecture Implementation:

1. **Layered Structure**:
   - Domain Layer (innermost): Contains core business entities (User) and interfaces (UserRepository)
   - Application Layer: Contains business logic (UserService)
   - Infrastructure Layer (outermost): Contains implementation details (InMemoryUserRepository)
   - API Layer: Entry point that wires everything together

2. **Dependency Direction**:
   - Inner layers know nothing about outer layers
   - Outer layers depend on inner layers through interfaces

3. **Testability**:
   - Easy to mock repositories for testing
   - Business logic is isolated from infrastructure concerns

4. **Microservice Characteristics**:
   - Self-contained component
   - Clear boundaries
   - Single responsibility (user management)

5. **No External Dependencies**:
   - Uses only standard C++ libraries
   - No framework requirements

## Sample Output:
```
Microservice running...
Saved user: 1
Saved user: 2
Retrieved user: Alice (alice@example.com)
```

This minimal example shows the core principles of onion architecture in a microservice context. In a real implementation, you would:
1. Add proper error handling
2. Implement actual persistence instead of in-memory
3. Add network communication for remote calls
4. Include more complex business logic
5. Add logging and monitoring

The architecture keeps the domain pure and separates concerns effectively while remaining simple and dependency-free.

---

Here's a simple ASCII diagram representing the onion architecture of the C++ microservice example:

```
┌─────────────────────────────────────────────────────┐
│                  API/Entry Point                   │
│                                                   │
│   ┌─────────────────────────────────────────────┐ │
│   │              Microservice Class             │ │
│   │                                             │ │
│   │   * Wires all layers together               │ │
│   │   * Provides run() entry point             │ │
│   └─────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
                         ▲
                         │
┌─────────────────────────────────────────────────────┐
│               Infrastructure Layer                  │
│                                                   │
│   ┌─────────────────────────────────────────────┐ │
│   │          InMemoryUserRepository             │ │
│   │                                             │ │
│   │   * Implements UserRepository interface     │ │
│   │   * Handles data persistence (in-memory)    │ │
│   └─────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
                         ▲
                         │
┌─────────────────────────────────────────────────────┐
│             Application Service Layer               │
│                                                   │
│   ┌─────────────────────────────────────────────┐ │
│   │               UserService                    │ │
│   │                                             │ │
│   │   * Contains business logic                 │ │
│   │   * Depends only on Repository interface    │ │
│   └─────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘
                         ▲
                         │
┌─────────────────────────────────────────────────────┐
│                 Domain Layer                        │
│                                                   │
│   ┌─────────────────────────────┐ ┌─────────────┐ │
│   │           User              │ │ UserRepository│ │
│   │                             │ │  Interface   │ │
│   │   * Core business entity    │ │              │ │
│   └─────────────────────────────┘ └─────────────┘ │
└─────────────────────────────────────────────────────┘
```

### Dependency Flow:
```
API → Infrastructure → Application → Domain
```

### Key Characteristics:
1. **Innermost Layer (Domain)**:
   - Contains pure business entities and interfaces
   - No dependencies on other layers
   - `User` struct and `UserRepository` interface

2. **Application Layer**:
   - Contains business logic (`UserService`)
   - Depends only on domain interfaces
   - Doesn't know about implementation details

3. **Infrastructure Layer**:
   - Implements domain interfaces (`InMemoryUserRepository`)
   - Handles technical details (persistence)
   - Can be swapped without changing other layers

4. **API/Entry Point**:
   - Wires all components together
   - Provides entry point (`Microservice` class)
   - Knows about all layers but delegates work inward

This diagram shows the classic onion architecture pattern where:
- Dependencies flow inward (outer layers depend on inner layers)
- Inner layers define contracts (interfaces)
- Outer layers implement those contracts
- Domain remains pure and framework-independent
