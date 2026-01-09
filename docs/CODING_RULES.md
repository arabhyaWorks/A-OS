# A-OS Coding Rules & Standards

**Quick Reference for Contributors**

This document provides a concise, enforceable set of coding rules for A-OS development.

---

## Table of Contents

1. [Language & Compiler](#language--compiler)
2. [File Conventions](#file-conventions)
3. [Naming Rules](#naming-rules)
4. [Code Formatting](#code-formatting)
5. [Memory Management](#memory-management)
6. [Error Handling](#error-handling)
7. [Header Files](#header-files)
8. [Performance Rules](#performance-rules)
9. [Forbidden Practices](#forbidden-practices)
10. [Code Review Checklist](#code-review-checklist)

---

## Language & Compiler

### MUST

- ✅ Use **C++17** (no newer, no older)
- ✅ Compile with zero warnings
- ✅ Support GCC 7+, Clang 5+, MSVC 2019+
- ✅ Compile on Linux, Windows, macOS

### MUST NOT

- ❌ Use C++20/23 features
- ❌ Use compiler extensions (stay standard)
- ❌ Use platform-specific APIs outside HAL

---

## File Conventions

### File Names

**Rule:** Use `snake_case` for files

```
✅ app_manager.cpp
✅ event_bus.h
✅ home_app.cpp

❌ AppManager.cpp
❌ EventBus.hpp
❌ HomeApp.cc
```

### File Extensions

**Rule:** Use `.h` for headers, `.cpp` for sources

```
✅ renderer.h, renderer.cpp

❌ renderer.hpp
❌ renderer.hxx
❌ renderer.cxx
```

### File Headers

**Rule:** Use `#pragma once`, not include guards

```cpp
// ✅ CORRECT
#pragma once

namespace AOS {
    // ...
}

// ❌ WRONG
#ifndef APP_MANAGER_H
#define APP_MANAGER_H
// ...
#endif
```

### Include Order

**Rule:** Own header → System → Project

```cpp
// ✅ CORRECT
#include "home_app.h"        // Own header first

#include <iostream>          // System headers
#include <memory>
#include <SDL2/SDL.h>

#include "os/app_manager.h"  // Project headers
#include "ui/renderer.h"

// ❌ WRONG - random order
#include "ui/renderer.h"
#include "home_app.h"
#include <iostream>
```

---

## Naming Rules

### Classes & Structs

**Rule:** `PascalCase`

```cpp
✅ class AppManager {};
✅ struct EventData {};

❌ class app_manager {};
❌ class appManager {};
```

### Functions & Methods

**Rule:** `camelCase`

```cpp
✅ void launchApp(int index);
✅ bool hasActiveApp() const;

❌ void LaunchApp(int index);
❌ void launch_app(int index);
```

### Variables

**Rule:** `camelCase`

```cpp
✅ int focusedIndex;
✅ App* activeApp;

❌ int FocusedIndex;
❌ int focused_index;
```

### Constants

**Rule:** `UPPER_SNAKE_CASE`

```cpp
✅ const int MAX_APPS = 64;
✅ constexpr int DEFAULT_WIDTH = 1280;

❌ const int maxApps = 64;
❌ const int kMaxApps = 64;
```

### Private Members

**Rule:** `camelCase` (no prefix)

```cpp
class MyClass {
private:
    ✅ int counter;
    ✅ std::string name;

    ❌ int m_counter;      // No m_ prefix
    ❌ int _counter;       // No _ prefix
    ❌ int counter_;       // No _ suffix
};
```

### Enums

**Rule:** Enum = `PascalCase`, Values = `PascalCase` or `UPPER_SNAKE_CASE`

```cpp
✅ enum class EventType {
    KEY_UP,
    KEY_DOWN,
    VOICE_WAKE
};

✅ enum class State {
    Idle,
    Running,
    Paused
};

❌ enum class event_type { ... };
❌ enum class EventType { keyUp, keyDown };
```

### Namespaces

**Rule:** `PascalCase`, always use `AOS::`

```cpp
✅ namespace AOS {
    class App {};
}

❌ namespace aos { ... }
❌ namespace a_o_s { ... }
```

---

## Code Formatting

### Indentation

**Rule:** 4 spaces, no tabs

```cpp
✅ void function() {
    if (condition) {
        doSomething();
    }
}

❌ void function() {
  if (condition) {  // 2 spaces
        doSomething();
    }
}
```

### Braces

**Rule:** K&R style (opening brace on same line)

```cpp
✅ void function() {
    if (x > 0) {
        doSomething();
    } else {
        doOtherThing();
    }
}

❌ void function()
{
    if (x > 0)
    {
        doSomething();
    }
}
```

**Exception:** Structs/classes can have brace on same or next line

```cpp
✅ class MyClass {
    // ...
};

✅ class MyClass
{
    // ...
};
```

### Line Length

**Rule:** Prefer <100 characters, hard limit 120

```cpp
✅ void shortFunction(int a, int b);

✅ void longerFunctionName(
    int parameterOne,
    int parameterTwo,
    const std::string& parameterThree
);

❌ void reallyLongFunctionWithManyParameters(int a, int b, int c, const std::string& d, const std::string& e, float f, double g);
```

### Whitespace

**Rule:** Space after keywords, around operators

```cpp
✅ if (condition) { ... }
✅ for (int i = 0; i < 10; i++) { ... }
✅ int sum = a + b;

❌ if(condition){ ... }
❌ for(int i=0;i<10;i++){ ... }
❌ int sum=a+b;
```

---

## Memory Management

### Ownership

**Rule:** Use smart pointers for ownership

```cpp
✅ std::unique_ptr<App> app;         // Unique ownership
✅ std::shared_ptr<Texture> texture; // Shared ownership (rare)
✅ App* activeApp;                    // Non-owning reference

❌ App* app = new App();              // Manual new/delete
❌ App* apps[10];                     // Manual array management
```

### RAII

**Rule:** Always use RAII (Resource Acquisition Is Initialization)

```cpp
✅ class Texture {
    SDL_Texture* sdlTexture;
public:
    Texture(SDL_Renderer* r, const char* path) {
        sdlTexture = IMG_LoadTexture(r, path);
    }
    ~Texture() {
        SDL_DestroyTexture(sdlTexture);
    }
};

❌ SDL_Texture* loadTexture(const char* path) {
    return IMG_LoadTexture(renderer, path);
    // Caller must remember to free!
}
```

### Allocation

**Rule:** Prefer stack, avoid heap unless necessary

```cpp
✅ Rect rect(10, 10, 100, 100);          // Stack
✅ std::vector<int> numbers;             // Stack (manages heap)

❌ Rect* rect = new Rect(10, 10, 100, 100);  // Unnecessary heap
```

### No Manual new/delete

**Rule:** Never use raw `new`/`delete` except in constructors/destructors

```cpp
✅ auto app = std::make_unique<App>();

❌ App* app = new App();
   // ... use app ...
   delete app;
```

---

## Error Handling

### No Exceptions

**Rule:** Do NOT use exceptions (embedded-friendly code)

```cpp
❌ throw std::runtime_error("Failed");
❌ try { ... } catch { ... }
```

### Return Bool for Success

**Rule:** Return `bool` for success/failure, log errors

```cpp
✅ bool initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

❌ void initialize() {
    SDL_Init(SDL_INIT_VIDEO);  // No error check!
}
```

### Assert for Impossible Conditions

**Rule:** Use `assert()` for developer errors

```cpp
✅ #include <cassert>

void launchApp(size_t index) {
    assert(index < apps.size());  // Debug check
    // ...
}

❌ void launchApp(size_t index) {
    // No check, crashes if out of bounds
}
```

### Logging

**Rule:** Use `std::cout` for info, `std::cerr` for errors

```cpp
✅ std::cout << "HomeApp: Started" << std::endl;
✅ std::cerr << "Failed to load texture: " << path << std::endl;

❌ printf("App started\n");
```

---

## Header Files

### Minimal Includes

**Rule:** Only include what you use in the header

```cpp
// app.h
#pragma once

namespace AOS {
    class Renderer;  // ✅ Forward declaration
    struct Event;    // ✅ Forward declaration
}

// app.cpp
#include "app.h"
#include "ui/renderer.h"   // ✅ Include in source
#include "os/event_bus.h"  // ✅ Include in source
```

### Forward Declarations

**Rule:** Prefer forward declarations in headers

```cpp
✅ // header
class Renderer;
void draw(Renderer& r);

❌ // header
#include "renderer.h"  // Unnecessary if only using pointer/reference
void draw(Renderer& r);
```

### Include Guards

**Rule:** Use `#pragma once`

```cpp
✅ #pragma once

❌ #ifndef MY_HEADER_H
   #define MY_HEADER_H
   ...
   #endif
```

---

## Performance Rules

### No Allocation in Loops

**Rule:** Avoid allocations in tight loops or render code

```cpp
✅ void render() {
    Rect rect(x, y, w, h);  // Stack, fast
    renderer.drawRect(rect, color);
}

❌ void render() {
    auto rect = std::make_unique<Rect>(x, y, w, h);  // Heap, slow
    renderer.drawRect(*rect, color);
}
```

### Cache Calculations

**Rule:** Don't recompute in render loop

```cpp
✅ void update(float dt) {
    cachedLayout = computeLayout();  // Once per frame
}

void render() {
    drawLayout(cachedLayout);
}

❌ void render() {
    Layout layout = computeLayout();  // 60x per second!
    drawLayout(layout);
}
```

### Pass by const Reference

**Rule:** Pass large objects by `const&`

```cpp
✅ void draw(const std::string& text);
✅ void process(const std::vector<int>& data);

❌ void draw(std::string text);        // Copies string
❌ void process(std::vector<int> data); // Copies vector
```

---

## Forbidden Practices

### ❌ Global Variables (except controlled cases)

```cpp
❌ int globalCounter;  // Avoid

✅ // Only acceptable pattern:
extern AppManager* g_appManager;  // Set once in main()
```

### ❌ using namespace in Headers

```cpp
❌ // header.h
using namespace std;  // Pollutes global namespace

✅ // source.cpp
using namespace std;  // OK in .cpp files
```

### ❌ Magic Numbers

```cpp
❌ if (width == 1280) { ... }

✅ constexpr int DEFAULT_WIDTH = 1280;
   if (width == DEFAULT_WIDTH) { ... }
```

### ❌ C-style Casts

```cpp
❌ int x = (int)floatValue;

✅ int x = static_cast<int>(floatValue);
```

### ❌ malloc/free

```cpp
❌ int* data = (int*)malloc(sizeof(int) * 100);
   free(data);

✅ std::vector<int> data(100);
```

### ❌ NULL

```cpp
❌ Renderer* r = NULL;

✅ Renderer* r = nullptr;
```

---

## Code Review Checklist

Before committing code, verify:

### Compilation

- [ ] Compiles with zero warnings
- [ ] Builds on at least one platform (desktop or Pi)
- [ ] No unused variables or functions

### Style

- [ ] Follows naming conventions
- [ ] 4-space indentation
- [ ] Line length <120 characters
- [ ] `#pragma once` in headers

### Memory

- [ ] No raw `new`/`delete`
- [ ] Smart pointers for ownership
- [ ] No leaks (test with valgrind/sanitizers)

### Performance

- [ ] No allocations in render loop
- [ ] Pass by `const&` where appropriate
- [ ] Cached computed values

### Correctness

- [ ] Error checking (bool returns)
- [ ] Asserts for impossible conditions
- [ ] Null pointer checks

### Documentation

- [ ] Class/function comments where needed
- [ ] Explains "why", not "what"
- [ ] Updated README if adding features

---

## When in Doubt

1. **Check existing code** - follow established patterns
2. **Consult PROJECT_SPEC.md** - comprehensive guide
3. **Ask in PR** - get team feedback
4. **Prefer simple** - avoid clever code

---

## Tools for Enforcement

**Future:**
- clang-format config
- clang-tidy checks
- Pre-commit hooks

---

**Remember:** These rules exist to keep the codebase maintainable, portable, and performant. Follow them consistently.
