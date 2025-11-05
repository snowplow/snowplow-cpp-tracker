# Snowplow C++ Tracker Testing - CLAUDE.md

## Directory Overview

This directory contains the comprehensive test suite for the Snowplow C++ Tracker. Tests are written using the Catch2 framework and follow a consistent pattern for unit testing, integration testing, and mocking. The test suite ensures correctness across all platforms and configurations.

### Test Categories
- **Unit Tests**: Individual component testing with mocks
- **Integration Tests**: End-to-end testing with Snowplow Micro
- **Configuration Tests**: Validation of configuration objects
- **Storage Tests**: Database and persistence testing
- **HTTP Tests**: Network layer testing with mock clients

## Development Commands

```bash
# Build tests
cmake -B build -DSNOWPLOW_BUILD_TESTS=ON
cmake --build build

# Run all tests
./build/snowplow-tests

# Run specific test suite
./build/snowplow-tests "[tracker]"

# Run with verbose output
./build/snowplow-tests -s
```

## Architecture

### Test Organization
```
test/
├── main.cpp                     # Catch2 entry point
├── catch.hpp                   # Catch2 header
├── *_test.cpp                 # Unit test files
├── configuration/             # Configuration tests
│   └── *_test.cpp
├── emitter/                  # Emitter tests
│   └── *_test.cpp
├── http/                     # HTTP layer tests
│   ├── test_http_client.hpp # Mock HTTP client
│   └── *_test.cpp
├── integration/             # Integration tests
│   ├── micro.hpp           # Snowplow Micro helper
│   └── integration_test.cpp
├── payload/                # Payload tests
│   └── *_test.cpp
└── storage/               # Storage tests
    └── *_test.cpp
```

## Core Testing Principles

### 1. **Test Isolation**
Each test is independent with its own setup/teardown:
```cpp
// ✅ Isolated test with local state
TEST_CASE("tracker initialization") {
    auto storage = make_shared<SqliteStorage>("test.db");
    auto emitter = make_shared<MockEmitter>(storage);
    // Test logic
    remove("test.db");  // Cleanup
}

// ❌ Shared state between tests
static auto storage = make_shared<SqliteStorage>("shared.db");
```

### 2. **Mock Pattern**
Consistent mock implementation for dependencies:
```cpp
// ✅ Mock with clear interface
class MockEmitter : public Emitter {
    vector<Payload> m_payloads;
public:
    void add(Payload payload) override { m_payloads.push_back(payload); }
    vector<Payload> get_added_payloads() { return m_payloads; }
};
```

### 3. **Assertion Patterns**
Use Catch2 assertions consistently:
```cpp
// ✅ Descriptive assertions
REQUIRE(result.is_success());
CHECK(payloads.size() == 2);
REQUIRE_THROWS_AS(invalid_call(), invalid_argument);

// ❌ Basic assertions without context
assert(result == true);
```

### 4. **Test Naming**
Clear, descriptive test names:
```cpp
// ✅ Descriptive test cases and sections
TEST_CASE("emitter processes batches correctly") {
    SECTION("single event batch") { }
    SECTION("multiple event batch") { }
    SECTION("oversized event handling") { }
}
```

## Layer Testing Patterns

### Unit Test Pattern
```cpp
TEST_CASE("component unit test") {
    // Arrange
    auto mock_dependency = make_shared<MockDependency>();
    ComponentUnderTest component(mock_dependency);
    
    // Act
    auto result = component.method_to_test();
    
    // Assert
    REQUIRE(result == expected_value);
    REQUIRE(mock_dependency->was_called());
}
```

### Integration Test Pattern
```cpp
TEST_CASE("integration test with micro") {
    // Setup Micro collector
    Micro micro;
    int port = micro.start();
    
    // Create real tracker
    auto tracker = Snowplow::create_tracker("ns", 
        "http://localhost:" + to_string(port), POST, "test.db");
    
    // Track event
    StructuredEvent event("category", "action");
    tracker->track(event);
    tracker->flush();
    
    // Verify in Micro
    auto good = micro.get_good_events();
    REQUIRE(good.size() == 1);
}
```

### Mock HTTP Client Pattern
```cpp
class TestHttpClient : public HttpClient {
    HttpRequestResult m_next_result;
public:
    void set_http_response(int code, bool success) {
        m_next_result = HttpRequestResult(list<int>{}, code, success);
    }
protected:
    HttpRequestResult http_request(...) override {
        return m_next_result;
    }
};
```

## Critical Testing Patterns

### Database Test Cleanup
```cpp
// ✅ Always clean up test databases
TEST_CASE("storage test") {
    string db_name = "test_" + to_string(time(nullptr)) + ".db";
    {
        SqliteStorage storage(db_name);
        // Test operations
    }
    remove(db_name.c_str());
}
```

### Async Test Handling
```cpp
// ✅ Wait for async operations
TEST_CASE("async emitter test") {
    emitter->add(payload);
    emitter->flush();  // Blocks until complete
    
    // Or use sleep for time-based
    this_thread::sleep_for(chrono::milliseconds(100));
    REQUIRE(emitter->get_event_count() == 0);
}
```

### Exception Testing
```cpp
// ✅ Test both success and failure paths
TEST_CASE("error handling") {
    SECTION("valid input succeeds") {
        REQUIRE_NOTHROW(function_under_test("valid"));
    }
    
    SECTION("invalid input throws") {
        REQUIRE_THROWS_AS(function_under_test(""), invalid_argument);
    }
}
```

## Essential Test Utilities

### Test Data Builders
```cpp
// ✅ Reusable test data creation
Payload create_test_payload(const string &event_type = "se") {
    Payload p;
    p.add("e", event_type);
    p.add("eid", Utils::get_uuid4());
    return p;
}
```

### Mock Validators
```cpp
// ✅ JSON validation helpers
void validate_event_json(const json &event) {
    REQUIRE(event.contains("e"));
    REQUIRE(event.contains("eid"));
    REQUIRE(event.contains("dtm"));
    REQUIRE(event.contains("tv"));
}
```

### Test Database Helper
```cpp
// ✅ Temporary database management
class TestDatabase {
    string m_path;
public:
    TestDatabase() : m_path("test_" + get_uuid4() + ".db") {}
    ~TestDatabase() { remove(m_path.c_str()); }
    string get_path() const { return m_path; }
};
```

## Common Testing Pitfalls & Solutions

### Resource Leaks
```cpp
// ❌ Wrong: File handle leak
FILE* f = fopen("test.txt", "w");
// Missing fclose

// ✅ Correct: RAII cleanup
{
    ofstream file("test.txt");
    // Auto-cleanup on scope exit
}
```

### Race Conditions
```cpp
// ❌ Wrong: No synchronization
emitter->add(payload);
REQUIRE(emitter->sent_count() == 1);  // May fail

// ✅ Correct: Proper synchronization
emitter->add(payload);
emitter->flush();  // Wait for completion
REQUIRE(emitter->sent_count() == 1);
```

### Platform Dependencies
```cpp
// ✅ Conditional test compilation
#ifdef _WIN32
TEST_CASE("windows specific test") {
    // Windows-only test
}
#endif
```

### Mock State Management
```cpp
// ✅ Reset mock state between tests
class MockEmitter : public Emitter {
public:
    void reset() {
        m_payloads.clear();
        m_call_count = 0;
    }
};
```

## Test File Template

```cpp
/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.
[License header]
*/

#include "catch.hpp"
#include "../include/snowplow/component_under_test.hpp"
#include "test_helpers.hpp"  // If needed

using namespace snowplow;
using std::make_shared;

TEST_CASE("component name") {
    // Setup shared test data
    auto test_db = make_unique<TestDatabase>();
    
    SECTION("feature one") {
        // Arrange
        ComponentUnderTest component;
        
        // Act
        auto result = component.method();
        
        // Assert
        REQUIRE(result == expected);
    }
    
    SECTION("error case") {
        REQUIRE_THROWS_AS(invalid_operation(), exception_type);
    }
}
```

## Quick Reference

### Test Execution Patterns
```bash
# Run all tests
./snowplow-tests

# Run tests matching pattern
./snowplow-tests "[tracker]"

# List all tests
./snowplow-tests --list-tests

# Run with detailed output
./snowplow-tests -s --reporter console
```

### Common Test Assertions
```cpp
REQUIRE(condition);              // Test fails if false
CHECK(condition);                // Test continues if false
REQUIRE_THROWS(expression);      // Must throw
REQUIRE_THROWS_AS(expr, type);   // Must throw specific type
REQUIRE_NOTHROW(expression);     // Must not throw
```

### Mock Creation Checklist
- [ ] Inherit from interface/base class
- [ ] Override virtual methods
- [ ] Add inspection methods (get_call_count, etc.)
- [ ] Add configuration methods (set_response, etc.)
- [ ] Provide reset/clear functionality

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