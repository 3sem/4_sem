# Clean Microservice Architecture in C++ (No External Libraries)

Here's a rewritten version following clean architecture principles with improved separation of concerns:

```cpp
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>

// ========================
// Domain Layer (Enterprise Business Rules)
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

    class IUserRepository {
    public:
        virtual ~IUserRepository() = default;
        virtual User getUser(int id) const = 0;
        virtual void saveUser(const User& user) = 0;
        virtual bool exists(const std::string& email) const = 0;
    };
}

// ========================
// Application Layer (Use Cases)
// ========================
namespace Application {
    struct UserDTO {
        std::string name;
        std::string email;
    };
    
    class UserService {
        Domain::IUserRepository& repository;
        
        int generateId() const {
            static int counter = 0;
            return ++counter;
        }
        
    public:
        explicit UserService(Domain::IUserRepository& repo) : repository(repo) {}
        
        Domain::User getUser(int id) const {
            return repository.getUser(id);
        }
        
        Domain::User createUser(const UserDTO& userData) {
            if (repository.exists(userData.email)) {
                throw std::runtime_error("Email already registered");
            }
            
            Domain::User newUser{
                generateId(),
                userData.name,
                userData.email
            };
            
            if (!newUser.isValid()) {
                throw std::runtime_error("Invalid user data");
            }
            
            repository.saveUser(newUser);
            return newUser;
        }
    };
}

// ========================
// Interface Adapters Layer
// ========================
namespace InterfaceAdapters {
    class ConsolePresenter {
    public:
        void showUser(const Domain::User& user) const {
            std::cout << "User Details:\n"
                      << "ID: " << user.id << "\n"
                      << "Name: " << user.name << "\n"
                      << "Email: " << user.email << "\n\n";
        }
        
        void showError(const std::string& message) const {
            std::cerr << "Error: " << message << "\n";
        }
    };
    
    class CommandLineController {
        Application::UserService& service;
        const ConsolePresenter& presenter;
        
    public:
        CommandLineController(Application::UserService& srv, 
                            const ConsolePresenter& pres)
            : service(srv), presenter(pres) {}
            
        void createUser(const std::string& name, const std::string& email) {
            try {
                Application::UserDTO dto{name, email};
                auto user = service.createUser(dto);
                presenter.showUser(user);
            } catch (const std::exception& e) {
                presenter.showError(e.what());
            }
        }
        
        void getUser(int id) {
            try {
                auto user = service.getUser(id);
                presenter.showUser(user);
            } catch (const std::exception& e) {
                presenter.showError(e.what());
            }
        }
    };
}

// ========================
// Infrastructure Layer
// ========================
namespace Infrastructure {
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
}

// ========================
// Composition Root
// ========================
class Microservice {
    std::unique_ptr<Domain::IUserRepository> repository;
    Application::UserService service;
    InterfaceAdapters::ConsolePresenter presenter;
    InterfaceAdapters::CommandLineController controller;
    
public:
    Microservice() 
        : repository(std::make_unique<Infrastructure::InMemoryUserRepository>()),
          service(*repository),
          controller(service, presenter) {}
    
    void run() {
        std::cout << "User Microservice\n";
        std::cout << "================\n\n";
        
        // Simulate API calls
        controller.createUser("Alice", "alice@example.com");
        controller.createUser("Bob", "bob@example.com");
        
        // Get existing user
        controller.getUser(1);
        
        // Get non-existent user
        controller.getUser(99);
        
        // Try invalid email
        controller.createUser("Charlie", "invalid-email");
    }
};

int main() {
    Microservice service;
    service.run();
    return 0;
}
```

## Key Improvements in Clean Architecture Style:

1. **Strict Dependency Rule**:
   - Dependencies only point inward (Domain ← Application ← Interface ← Infrastructure)
   - Domain layer has no dependencies

2. **Clear Layer Separation**:
   - **Domain**: Pure business entities and interfaces
   - **Application**: Business logic and use cases
   - **Interface Adapters**: Convert between layers (presenters, controllers)
   - **Infrastructure**: Technical implementation details

3. **Enhanced Structure**:
   - Added proper DTOs for cross-layer communication
   - Added validation in domain entities
   - Better error handling
   - Proper presentation separation

4. **Testability**:
   - Every component can be tested in isolation
   - Easy to mock interfaces
   - Business rules don't depend on infrastructure

5. **Flexibility**:
   - Can change database without touching business logic
   - Can change UI without changing use cases
   - Can add new delivery mechanisms (HTTP API, etc.)

## Sample Output:
```
User Microservice
================

User Details:
ID: 1
Name: Alice
Email: alice@example.com

User Details:
ID: 2
Name: Bob
Email: bob@example.com

User Details:
ID: 1
Name: Alice
Email: alice@example.com

Error: User not found
Error: Invalid user data
```

This version better follows clean architecture principles with:
- Clear separation of concerns
- Independent evolvability of layers
- Business rules that are framework-independent
- Testable components
- Clear data flow between layers

---

The main differences between the **Clean Architecture** implementation and the previous **Onion Architecture** version are in their layer organization, dependency management, and design priorities. Here's a detailed comparison:

---

### **1. Layer Structure & Naming**
| **Onion Architecture**          | **Clean Architecture**           |
|----------------------------------|----------------------------------|
| 1. Domain (innermost)           | 1. Domain (innermost)            |
| 2. Application                  | 2. Application (Use Cases)       |
| 3. Infrastructure (outermost)   | 3. Interface Adapters            |
| 4. API/Entry Point              | 4. Infrastructure (outermost)    |
|                                 | 5. Composition Root              |

**Key Change**:  
- Clean Architecture explicitly separates **interface adapters** (controllers, presenters) from infrastructure.
- Onion treats the API layer as part of infrastructure, while Clean splits delivery mechanisms (e.g., CLI/HTTP) from technical implementations (e.g., databases).

---

### **2. Dependency Direction**
| **Onion**                       | **Clean**                        |
|----------------------------------|----------------------------------|
| Outer layers depend on inner layers (Domain ← Application ← Infrastructure) | Strict **dependency rule**: Only outer layers can depend on inner layers. Inner layers **never** know about outer layers. |
| API layer knows all layers       | **Composition Root** wires dependencies (avoids circular references) |

**Key Change**:  
- Clean Architecture enforces **strict unidirectional dependencies** (no inner-layer awareness of outer layers).
- Onion allows some flexibility (e.g., API layer may reference all layers directly).

---

### **3. Business Logic Isolation**
| **Onion**                       | **Clean**                        |
|----------------------------------|----------------------------------|
| Business logic in `UserService` (Application layer) | Business rules **purely in Domain**, use cases in Application layer. |
| Domain entities may have minimal logic | Domain entities enforce invariants (e.g., `User::isValid()`) |

**Key Change**:  
- Clean Architecture pushes **core business rules** deeper into the Domain layer (e.g., validation).
- Onion often keeps business logic in Application services.

---

### **4. Interface Adapters**
| **Onion**                       | **Clean**                        |
|----------------------------------|----------------------------------|
| API layer handles input/output   | Dedicated **presenters** and **controllers** convert data between layers. |
| Direct coupling to delivery (e.g., HTTP) | Decoupled delivery (CLI/HTTP can be swapped) |

**Example**:  
- Clean’s `ConsolePresenter` formats output **without** the Domain/Application layers knowing about presentation.
- Onion’s API layer often mixes delivery and business logic.

---

### **5. Testability**
| **Onion**                       | **Clean**                        |
|----------------------------------|----------------------------------|
| Mocking requires overriding infrastructure | Clear interfaces at **all layer boundaries** (easier to mock). |
| Domain tests may depend on infrastructure | Domain tests **never** touch infrastructure. |

**Key Change**:  
- Clean’s strict boundaries make unit tests **simpler** (e.g., test use cases with in-memory repos).

---

### **6. Flexibility**
| **Onion**                       | **Clean**                        |
|----------------------------------|----------------------------------|
| Database changes affect Application layer | Infrastructure changes **never** propagate inward. |
| UI/delivery changes may ripple to inner layers | UI changes are isolated to interface adapters. |

**Example**:  
- In Clean, switching from CLI to HTTP **only** requires new adapters (no changes to Domain/Application).

---

### **When to Use Which?**
- **Onion**: Simpler projects where layers may overlap (e.g., small microservices).
- **Clean**: Complex domains requiring **strict separation** (e.g., long-lived enterprise systems).

Both architectures share the same core goal (dependency inversion), but Clean enforces it more rigorously. The Onion version above could be seen as a "light" Clean Architecture.
