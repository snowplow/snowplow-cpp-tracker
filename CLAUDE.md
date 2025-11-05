# Snowplow C++ Tracker - CLAUDE.md

## Project Overview

The Snowplow C++ Tracker is a cross-platform analytics library for collecting behavioral event data from C++ applications. It sends events to Snowplow collectors for processing in the Snowplow analytics pipeline. The tracker supports Windows, macOS, and Linux with platform-specific HTTP client implementations.

### Key Technologies
- **Language**: C++11
- **Build System**: CMake 3.15+
- **Testing**: Catch2 framework
- **Dependencies**: nlohmann/json, SQLite3, platform-specific (CURL/WinInet/Apple Foundation)
- **Platforms**: Windows, macOS, Linux

## Development Commands

```bash
# Configure build with tests
cmake -B build -DSNOWPLOW_BUILD_TESTS=ON -DSNOWPLOW_BUILD_EXAMPLE=ON

# Build project
cmake --build build

# Run tests
./build/snowplow-tests

# Install library
cmake --install build --prefix /usr/local
```

## Architecture

### System Design
The tracker follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────┐
│           Application Layer                  │
│         (Snowplow, Tracker)                 │
├─────────────────────────────────────────────┤
│           Event Layer                        │
│    (Event types, SelfDescribingJson)        │
├─────────────────────────────────────────────┤
│          Emitter Layer                       │
│    (Emitter, RetryDelay, EmitStatus)        │
├─────────────────────────────────────────────┤
│         Storage Layer                        │
│    (EventStore, SessionStore, SQLite)       │
├─────────────────────────────────────────────┤
│           HTTP Layer                         │
│  (Platform-specific HttpClient impls)       │
└─────────────────────────────────────────────┘
```

### Core Components
- **Tracker**: Main interface for tracking events with configuration
- **Emitter**: Asynchronous event sender with retry logic
- **Event Hierarchy**: Base Event class with specialized event types
- **Storage**: SQLite-based persistent event queue and session storage
- **HTTP Clients**: Platform-specific implementations (Windows/Apple/CURL)
- **Configuration**: Structured config objects for tracker, network, emitter, session

## Core Architectural Principles

### 1. **Configuration-Driven Initialization**
All components use configuration objects instead of long parameter lists:
```cpp
// ✅ Use configuration objects
TrackerConfiguration tracker_config("namespace", "app-id", mob);
NetworkConfiguration network_config(uri, POST);

// ❌ Avoid multiple constructor parameters
Tracker(emitter, subject, session, "srv", "app", "ns", true, true);
```

### 2. **RAII and Smart Pointer Usage**
Consistent use of smart pointers for memory management:
```cpp
// ✅ Use shared_ptr for shared ownership
shared_ptr<Emitter> emitter = make_shared<Emitter>(...);
shared_ptr<Subject> subject = make_shared<Subject>();

// ❌ Avoid raw pointers for ownership
Emitter* emitter = new Emitter(...);
```

### 3. **Platform Abstraction**
Platform-specific code isolated in separate implementations:
```cpp
// ✅ Abstract base class with platform implementations
class HttpClient { virtual HttpRequestResult http_request(...) = 0; };
class HttpClientWindows : public HttpClient { };
class HttpClientApple : public HttpClient { };
```

### 4. **Namespace Organization**
All code within `snowplow` namespace with minimal `using` declarations:
```cpp
namespace snowplow {
using std::string;  // Only common types
using std::shared_ptr;
// Implementation here
}
```

## Layer Organization & Responsibilities

### Application Layer (`snowplow.hpp`, `tracker.hpp`)
- Static factory methods for tracker creation
- Tracker lifecycle management
- Event tracking interface

### Event Layer (`events/`)
- Event base class with virtual methods
- Specialized event types (StructuredEvent, ScreenViewEvent, etc.)
- Self-describing JSON support

### Emitter Layer (`emitter/`)
- Asynchronous event dispatch
- Batch processing
- Retry logic with exponential backoff
- Request callbacks

### Storage Layer (`storage/`)
- Event queue persistence
- Session state management
- Abstract interfaces (EventStore, SessionStore)
- SQLite implementation

### HTTP Layer (`http/`)
- Platform-specific HTTP clients
- Request/response handling
- Cookie management (CURL)

## Critical Import Patterns

### Header Guards
```cpp
// ✅ Always use header guards
#ifndef TRACKER_H
#define TRACKER_H
// content
#endif

// ❌ Never omit header guards
// content without guards
```

### Include Order
```cpp
// ✅ Correct include order
#include "local_header.hpp"           // Local headers first
#include "../relative/header.hpp"     // Relative paths
#include <snowplow/public_header.hpp> // Library headers
#include <system_header>              // System headers
```

### Forward Declarations
```cpp
// ✅ Use forward declarations in headers
namespace snowplow {
class Emitter;  // Forward declaration
class Tracker {
    shared_ptr<Emitter> m_emitter;
};
}
```

## Essential Library Patterns

### Event Creation Pattern
```cpp
// ✅ Use builder pattern for events
StructuredEvent event("category", "action");
event.set_label("label");
event.set_property("property");
event.set_value(100.0);
tracker->track(event);
```

### Configuration Pattern
```cpp
// ✅ Configure before construction
EmitterConfiguration emitter_config("sp.db");
emitter_config.set_batch_size(500);
emitter_config.set_byte_limit_post(5200);
auto emitter = make_shared<Emitter>(network_config, emitter_config);
```

### Callback Pattern
```cpp
// ✅ Use lambda callbacks
emitter_config.set_request_callback(
    [](list<string> event_ids, EmitStatus status) {
        // Handle callback
    },
    EmitStatus::SUCCESS | EmitStatus::FAILED_WILL_RETRY
);
```

## Model Organization Pattern

### Event Hierarchy
```cpp
class Event {  // Abstract base
protected:
    virtual EventPayload get_custom_event_payload(bool use_base64) const = 0;
};

class StructuredEvent : public Event {  // Concrete implementation
protected:
    EventPayload get_custom_event_payload(bool use_base64) const override;
};
```

### Storage Interfaces
```cpp
struct EventStore {  // Pure interface
    virtual void add_event(const Payload &payload) = 0;
    virtual void get_event_rows_batch(list<EventRow> *event_list, int number) = 0;
    virtual void delete_event_rows_with_ids(const list<int> &id_list) = 0;
};
```

## Common Pitfalls & Solutions

### Memory Management
```cpp
// ❌ Wrong: Memory leak with raw pointer
HttpClient* client = new HttpClientWindows();

// ✅ Correct: Use unique_ptr for ownership
unique_ptr<HttpClient> client(new HttpClientWindows());
```

### Thread Safety
```cpp
// ❌ Wrong: Unprotected shared state
m_trackers[namespace] = tracker;

// ✅ Correct: Use mutex for thread safety
std::lock_guard<std::mutex> guard(m_tracker_get);
m_trackers[namespace] = tracker;
```

### Platform Detection
```cpp
// ✅ Correct platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
// Windows-specific code
#elif defined(__APPLE__)
// Apple-specific code
#else
// Linux/Unix code
#endif
```

### Constant Definitions
```cpp
// ✅ Use const string in namespace
namespace snowplow {
const string SNOWPLOW_TRACKER_VERSION_LABEL = "cpp-2.0.0";
}

// ❌ Avoid #define for constants
#define VERSION "2.0.0"
```

## File Structure Template

```
snowplow-cpp-tracker/
├── CMakeLists.txt                 # Main build configuration
├── cmake/                         # CMake modules and configs
│   ├── FindLibUUID.cmake
│   └── snowplow-config.cmake.in
├── include/snowplow/              # Public headers
│   ├── snowplow.hpp              # Main include file
│   ├── tracker.hpp               # Tracker interface
│   ├── subject.hpp               # User/device info
│   ├── client_session.hpp       # Session tracking
│   ├── configuration/            # Configuration objects
│   │   ├── tracker_configuration.hpp
│   │   ├── network_configuration.hpp
│   │   └── emitter_configuration.hpp
│   ├── events/                   # Event types
│   │   ├── event.hpp
│   │   └── structured_event.hpp
│   ├── emitter/                  # Event emission
│   │   ├── emitter.hpp
│   │   └── retry_delay.hpp
│   ├── http/                     # HTTP clients
│   │   ├── http_client.hpp
│   │   └── http_client_[platform].hpp
│   ├── storage/                  # Persistence
│   │   ├── event_store.hpp
│   │   └── sqlite_storage.hpp
│   └── payload/                  # Event payloads
│       ├── payload.hpp
│       └── self_describing_json.hpp
├── test/                         # Test files
│   ├── catch.hpp                # Test framework
│   └── *_test.cpp              # Test implementations
└── examples/                     # Usage examples
    └── main.cpp
```

## Quick Reference

### Event Tracking Checklist
- [ ] Create tracker with appropriate configuration
- [ ] Set up Subject with user/device information
- [ ] Configure Emitter with storage and network settings
- [ ] Create event objects with required properties
- [ ] Call `tracker->track(event)` to send events
- [ ] Use `tracker->flush()` before application exit

### Common Event Types
```cpp
// Structured Event
StructuredEvent se("category", "action");

// Screen View Event  
ScreenViewEvent sv("screen_name", "screen_id");

// Self-Describing Event
SelfDescribingJson data("iglu:com.acme/event/jsonschema/1-0-0", json_data);
SelfDescribingEvent sde(data);

// Timing Event
TimingEvent te("category", "variable", 1500);
```

### Platform-Specific Considerations
- **Windows**: Uses WinInet API, no external dependencies
- **macOS/iOS**: Uses NSURLSession via Objective-C++
- **Linux**: Requires CURL and LibUUID libraries

## Contributing to CLAUDE.md

When adding or updating content in this document, please follow these guidelines:

### File Size Limit
- **CLAUDE.md must not exceed 40KB** (currently ~10KB)
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

### Multiple CLAUDE.md Files
- **Directory-specific CLAUDE.md files** can be created for specialized modules
- Follow the same structure and guidelines as this root CLAUDE.md
- Keep them focused on directory-specific patterns and conventions
- Maximum 20KB per directory-specific CLAUDE.md file

### Instructions for LLMs
When editing files in this repository, **always check for CLAUDE.md guidance**:

1. **Look for CLAUDE.md in the same directory** as the file being edited
2. **If not found, check parent directories** recursively up to project root
3. **Follow the patterns and conventions** described in the applicable CLAUDE.md
4. **Prioritize directory-specific guidance** over root-level guidance when conflicts exist