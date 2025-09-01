# Snowplow C++ Tracker API Implementation - CLAUDE.md

## Directory Overview

This directory contains the core implementation of the Snowplow C++ Tracker library. It includes all public headers and implementation files that comprise the tracker API. The code here follows strict patterns for cross-platform compatibility, memory safety, and API consistency.

### Key Components
- **Core Classes**: Tracker, Emitter, Subject, ClientSession
- **Event System**: Event hierarchy with specialized event types
- **Configuration**: Type-safe configuration objects
- **Storage**: Abstract storage interfaces with SQLite implementation
- **HTTP**: Platform-specific HTTP client implementations
- **Payload**: Event payload construction and serialization

## Development Commands

```bash
# Include this directory in your project
target_include_directories(your_target PRIVATE ${snowplow_SOURCE_DIR}/include)

# Link against snowplow library
target_link_libraries(your_target snowplow::snowplow)
```

## Architecture

### Header Organization
```
include/snowplow/
├── snowplow.hpp                 # Master include file
├── tracker.hpp                  # Core tracker class
├── subject.hpp                  # User/device information
├── client_session.hpp          # Session tracking
├── constants.hpp               # Global constants
├── cracked_url.hpp            # URL parsing
├── configuration/             # Configuration objects
├── events/                   # Event type hierarchy
├── emitter/                 # Event emission logic
├── http/                   # HTTP client abstraction
├── storage/               # Storage interfaces
├── payload/              # Payload construction
├── detail/              # Implementation details
└── thirdparty/         # Vendored dependencies
```

## Core Architectural Principles

### 1. **Header-Only vs Implementation Files**
Mixed approach with headers for interfaces, .cpp for implementations:
```cpp
// ✅ Header with declaration
class Tracker {
public:
    string track(const Event &event);
};

// ✅ Implementation in .cpp
string Tracker::track(const Event &event) { /* impl */ }
```

### 2. **Const Correctness**
Consistent use of const for immutable operations:
```cpp
// ✅ Const methods that don't modify state
string get_namespace() const { return m_namespace; }
vector<SelfDescribingJson> get_context() const;

// ❌ Missing const on getter
string get_namespace() { return m_namespace; }
```

### 3. **Virtual Function Pattern**
Pure virtual for interfaces, virtual for overridable behavior:
```cpp
// ✅ Pure virtual for interfaces
struct EventStore {
    virtual void add_event(const Payload &payload) = 0;
};

// ✅ Protected virtual for template method
class Event {
protected:
    virtual EventPayload get_custom_event_payload(bool use_base64) const = 0;
};
```

### 4. **Copy/Move Semantics**
Proper handling of resource management:
```cpp
// ✅ Pass by const reference for read-only
void set_context(const vector<SelfDescribingJson> &context);

// ✅ Move semantics for transfer of ownership
Emitter(shared_ptr<EventStore> event_store) : m_event_store(move(event_store))

// ❌ Pass by value unnecessarily
void set_context(vector<SelfDescribingJson> context);
```

## Layer Organization & Responsibilities

### Configuration Layer (`configuration/`)
```cpp
// Configuration object pattern
class TrackerConfiguration {
    TrackerConfiguration(const string &name_space, const string &app_id = "", Platform platform = srv);
    void set_use_base64(bool use_base64);
    string get_namespace() const;
};
```

### Event Layer (`events/`)
```cpp
// Event hierarchy pattern
class Event {  // Base class
    vector<SelfDescribingJson> get_context() const;
    void set_context(const vector<SelfDescribingJson> &context);
protected:
    virtual EventPayload get_custom_event_payload(bool use_base64) const = 0;
};

class StructuredEvent : public Event {  // Concrete event
    StructuredEvent(const string &category, const string &action);
protected:
    EventPayload get_custom_event_payload(bool use_base64) const override;
};
```

### HTTP Layer (`http/`)
```cpp
// Platform abstraction pattern
class HttpClient {  // Abstract base
public:
    virtual ~HttpClient() {}
protected:
    virtual HttpRequestResult http_request(...) = 0;
};

#if defined(WIN32)
class HttpClientWindows : public HttpClient { };
#elif defined(__APPLE__)
class HttpClientApple : public HttpClient { };
#else
class HttpClientCurl : public HttpClient { };
#endif
```

## Critical Import Patterns

### Include Guards Convention
```cpp
// ✅ Use FILENAME_H pattern
#ifndef TRACKER_H
#define TRACKER_H
// content
#endif

// ❌ Don't use pragma once (less portable)
#pragma once
```

### Circular Dependency Prevention
```cpp
// ✅ Forward declare when possible
class Emitter;  // Forward declaration
class Tracker {
    shared_ptr<Emitter> m_emitter;
};

// ❌ Include full header unnecessarily
#include "emitter.hpp"  // Full include in header
```

### Platform-Specific Includes
```cpp
// ✅ Conditional compilation for platform headers
#if defined(WIN32)
#include <windows.h>
#include <WinInet.h>
#elif defined(__APPLE__)
#import <Foundation/Foundation.h>
#endif
```

## Essential Library Patterns

### Factory Method Pattern
```cpp
// ✅ Static factory methods in Snowplow class
static shared_ptr<Tracker> create_tracker(
    const string &name_space,
    const string &collector_url,
    Method method,
    const string &db_name
);
```

### Builder Pattern for Events
```cpp
// ✅ Fluent interface for event construction
StructuredEvent event("category", "action");
event.set_label("label")
     .set_property("property")
     .set_value(100.0);
```

### Template Method Pattern
```cpp
// ✅ Base class defines algorithm, subclasses implement steps
class Event {
private:
    EventPayload get_payload(bool use_base64) const {
        EventPayload payload = get_custom_event_payload(use_base64);
        // Common processing
        return payload;
    }
protected:
    virtual EventPayload get_custom_event_payload(bool use_base64) const = 0;
};
```

## Model Organization Pattern

### Configuration Objects
```cpp
// ✅ Separate configuration from construction
NetworkConfiguration network_config(uri, POST);
network_config.set_custom_headers(headers);

EmitterConfiguration emitter_config("sp.db");
emitter_config.set_batch_size(500);
```

### Storage Abstraction
```cpp
// ✅ Interface segregation
struct EventStore {
    virtual void add_event(const Payload &payload) = 0;
};

struct SessionStore {
    virtual json get_session_data() = 0;
    virtual void update_session_data(const json &data) = 0;
};

// Concrete implementation combines both
class SqliteStorage : public EventStore, public SessionStore { };
```

## Common Pitfalls & Solutions

### String Handling
```cpp
// ❌ Wrong: C-style strings
char* namespace = "tracker";

// ✅ Correct: Use std::string
string m_namespace = "tracker";
```

### Exception Safety
```cpp
// ❌ Wrong: Can throw in destructor
~Emitter() {
    stop();  // May throw
}

// ✅ Correct: No-throw destructor
~Emitter() {
    try {
        stop();
    } catch (...) {
        // Log but don't propagate
    }
}
```

### Resource Management
```cpp
// ❌ Wrong: Manual memory management
HttpClient* client = new HttpClientWindows();
delete client;

// ✅ Correct: RAII with smart pointers
unique_ptr<HttpClient> client = make_unique<HttpClientWindows>();
```

### Thread Safety
```cpp
// ✅ Mutex protection for shared state
class Snowplow {
private:
    static map<string, shared_ptr<Tracker>> m_trackers;
    static mutex m_tracker_get;
public:
    static shared_ptr<Tracker> get_tracker(const string &name_space) {
        lock_guard<mutex> guard(m_tracker_get);
        return m_trackers[name_space];
    }
};
```

## File Structure Template

### Header File Structure
```cpp
// header.hpp
#ifndef HEADER_H
#define HEADER_H

#include <string>              // System includes
#include "local_header.hpp"    // Local includes

namespace snowplow {

using std::string;            // Limited using declarations

class ClassName {
public:
    // Public interface
    void public_method();
    
private:
    // Private members
    string m_member_var;      // m_ prefix for members
};

} // namespace snowplow

#endif
```

### Implementation File Structure
```cpp
// implementation.cpp
#include "snowplow/header.hpp"
#include <algorithm>           // Additional includes

namespace snowplow {

void ClassName::public_method() {
    // Implementation
}

} // namespace snowplow
```

## Quick Reference

### Creating New Event Types
1. Inherit from `Event` base class
2. Implement `get_custom_event_payload()` method
3. Add event-specific setters
4. Define event type constant in `constants.hpp`

### Adding Platform Support
1. Create new `HttpClient[Platform]` class
2. Implement `http_request()` method
3. Add platform detection in CMakeLists.txt
4. Update `emitter.cpp` to instantiate correct client

### Storage Implementation Checklist
- [ ] Implement `EventStore` interface
- [ ] Implement `SessionStore` interface (if needed)
- [ ] Handle thread safety
- [ ] Provide transaction support
- [ ] Test with concurrent access

## Contributing to CLAUDE.md

When adding or updating content in this document, please follow these guidelines:

### File Size Limit
- **CLAUDE.md must not exceed 20KB** (currently ~9KB)
- Check file size after updates: `wc -c CLAUDE.md`
- Remove outdated content if approaching the limit

### Code Examples
- Keep all code examples **4 lines or fewer**
- Focus on the essential pattern, not complete implementations
- Use `// ❌` and `// ✅` to clearly show wrong vs right approaches

### Content Organization
- Add new patterns to existing sections when possible
- Create new sections sparingly to maintain structure
- Update the architectural principles section for major changes
- Ensure examples follow current codebase conventions

### Quality Standards
- Test any new patterns in actual code before documenting
- Verify imports and syntax are correct for the codebase
- Keep language concise and actionable
- Focus on "what" and "how", minimize "why" explanations

### Instructions for LLMs
When editing files in this directory, **always check for CLAUDE.md guidance**:

1. **Look for CLAUDE.md in the same directory** as the file being edited
2. **If not found, check parent directories** recursively up to project root
3. **Follow the patterns and conventions** described in the applicable CLAUDE.md
4. **Prioritize directory-specific guidance** over root-level guidance when conflicts exist