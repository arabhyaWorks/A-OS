# A-OS Documentation Index

**Complete Guide to A-OS Architecture, Standards, and Development**

---

## Quick Navigation

| Document | Purpose | Audience |
|----------|---------|----------|
| **[PROJECT_SPEC.md](PROJECT_SPEC.md)** | Comprehensive technical specification | All developers |
| **[CODING_RULES.md](CODING_RULES.md)** | Code standards and conventions | Contributors |
| **[ARCHITECTURE.md](ARCHITECTURE.md)** | System design and data flow | Architects, senior devs |
| **[OBJECTIVES.md](OBJECTIVES.md)** | Project goals and success criteria | All stakeholders |
| **[../README.md](../README.md)** | Quick start and building | New developers |

---

## Document Purposes

### 1. [PROJECT_SPEC.md](PROJECT_SPEC.md) - **THE MASTER DOCUMENT**

**Read this first if you're joining the project.**

**Contains:**
- Executive summary
- Vision & philosophy
- Complete architecture
- Subsystem specifications
- Application development guide
- Build & deployment
- Development workflow
- Roadmap
- Design decisions & rationale

**When to use:**
- Onboarding new developers
- Understanding system design
- Making architectural decisions
- Reference for any technical question

**Length:** ~300 lines (comprehensive)

---

### 2. [CODING_RULES.md](CODING_RULES.md) - **STYLE GUIDE**

**Read this before writing code.**

**Contains:**
- Naming conventions
- File organization
- Code formatting rules
- Memory management rules
- Error handling patterns
- Forbidden practices
- Code review checklist

**When to use:**
- Before committing code
- During code review
- When in doubt about style
- Setting up editor/IDE

**Length:** ~150 lines (quick reference)

---

### 3. [ARCHITECTURE.md](ARCHITECTURE.md) - **DEEP DIVE**

**Read this to understand how the system works internally.**

**Contains:**
- Visual system diagrams
- Component interactions
- Data flow diagrams
- Lifecycle state machines
- Threading model
- Memory layout
- Platform abstraction

**When to use:**
- Understanding component relationships
- Debugging complex issues
- Planning new features
- Performance optimization

**Length:** ~200 lines (visual guide)

---

### 4. [OBJECTIVES.md](OBJECTIVES.md) - **THE "WHY"**

**Read this to understand project goals and vision.**

**Contains:**
- Primary mission
- Core objectives
- Success metrics
- Non-goals
- Long-term vision
- Success definition

**When to use:**
- Understanding project direction
- Making feature decisions
- Prioritizing work
- Explaining A-OS to others

**Length:** ~200 lines (strategic)

---

### 5. [../README.md](../README.md) - **GETTING STARTED**

**Read this to build and run A-OS.**

**Contains:**
- Quick overview
- Installation prerequisites
- Build instructions
- Running the OS
- Controls
- Current status

**When to use:**
- First-time setup
- Quick reference
- Sharing with others
- Deployment instructions

**Length:** ~100 lines (practical)

---

## Reading Order by Role

### For New Developers

**Day 1:**
1. Read [README.md](../README.md) - Get it running
2. Read [OBJECTIVES.md](OBJECTIVES.md) - Understand the vision

**Day 2:**
3. Read [PROJECT_SPEC.md](PROJECT_SPEC.md) sections:
   - Executive Summary
   - Vision & Philosophy
   - Core Architecture
   - Application Development Guide

**Day 3:**
4. Read [CODING_RULES.md](CODING_RULES.md) - Before writing code
5. Browse [ARCHITECTURE.md](ARCHITECTURE.md) - Understand system

**Day 4+:**
6. Start contributing!
7. Reference docs as needed

---

### For Architects/Technical Leads

**Priority order:**
1. [PROJECT_SPEC.md](PROJECT_SPEC.md) - Full read
2. [ARCHITECTURE.md](ARCHITECTURE.md) - Full read
3. [OBJECTIVES.md](OBJECTIVES.md) - Understand goals
4. [CODING_RULES.md](CODING_RULES.md) - Enforce standards

---

### For Project Managers/Stakeholders

**Essential reading:**
1. [OBJECTIVES.md](OBJECTIVES.md) - Goals & success metrics
2. [PROJECT_SPEC.md](PROJECT_SPEC.md) - Sections:
   - Executive Summary
   - Roadmap & Milestones
3. [README.md](../README.md) - See current status

---

### For Contributors (Pull Requests)

**Before submitting PR:**
1. [CODING_RULES.md](CODING_RULES.md) - Follow all rules
2. [PROJECT_SPEC.md](PROJECT_SPEC.md) - Reference architecture
3. Use code review checklist from CODING_RULES.md

---

## Document Relationships

```
                    ┌─────────────────┐
                    │   OBJECTIVES    │
                    │   (The "Why")   │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │  PROJECT_SPEC   │
                    │  (The "What")   │
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │                 │
                    ▼                 ▼
          ┌─────────────────┐  ┌─────────────────┐
          │  ARCHITECTURE   │  │  CODING_RULES   │
          │   (The "How")   │  │ (The "Standard")│
          └─────────────────┘  └─────────────────┘
                    │                 │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │     README      │
                    │ (Quick Start)   │
                    └─────────────────┘
```

---

## Quick Reference Tables

### Key Classes & Files

| Class/File | Location | Purpose | Document |
|------------|----------|---------|----------|
| `OSCore` | `src/os/os_core.h` | Main loop, init | PROJECT_SPEC §6.1 |
| `AppManager` | `src/os/app_manager.h` | App lifecycle | PROJECT_SPEC §6.3 |
| `EventBus` | `src/os/event_bus.h` | Pub/sub events | PROJECT_SPEC §6.2 |
| `App` | `src/os/app.h` | Base app class | PROJECT_SPEC §6.4 |
| `Renderer` | `src/ui/renderer.h` | Display abstraction | PROJECT_SPEC §6.6 |
| `InputManager` | `src/hal/input_manager.h` | Input normalization | PROJECT_SPEC §6.5 |

### Common Tasks

| Task | Where to Look |
|------|--------------|
| **Build the project** | README.md |
| **Create a new app** | PROJECT_SPEC.md §7.1 |
| **Add input handling** | PROJECT_SPEC.md §7.3 |
| **Deploy to Pi** | PROJECT_SPEC.md §8.4 |
| **Check coding style** | CODING_RULES.md |
| **Understand events** | ARCHITECTURE.md §3 |
| **App lifecycle** | ARCHITECTURE.md §4 |

### Project Status

| Phase | Status | Document |
|-------|--------|----------|
| MVP Foundation | ✅ Complete | OBJECTIVES.md |
| Text Rendering | 🚧 In Progress | README.md |
| Pi Deployment | ⏳ Planned | PROJECT_SPEC.md §8.3 |
| Voice System | ⏳ Planned | OBJECTIVES.md |
| App Packaging | ⏳ Future | OBJECTIVES.md |

---

## Core Principles (Quick Reference)

### From OBJECTIVES.md

1. **Device First** - Not a PC
2. **Voice Equal** - Input, not feature
3. **Native Only** - No web stack
4. **Linux Hidden** - User never sees it
5. **Desktop Dev** - Iterate on PC first
6. **60 FPS** - Non-negotiable
7. **Offline First** - Cloud optional

### From PROJECT_SPEC.md

1. **Deterministic Behavior** - Predictable, console-like
2. **Event-Driven Everything** - All input becomes events
3. **RAII** - Automatic resource management
4. **No Exceptions** - Return bool, log errors
5. **Forward Declarations** - Minimize header includes

### From CODING_RULES.md

1. **PascalCase** - Classes
2. **camelCase** - Functions, variables
3. **4 spaces** - Indentation
4. **K&R braces** - Same line
5. **Smart pointers** - Ownership
6. **#pragma once** - Header guards

---

## Glossary (Cross-Reference)

| Term | Definition | Where Defined |
|------|------------|---------------|
| **A-OS** | Application Operating System | PROJECT_SPEC.md §1 |
| **HAL** | Hardware Abstraction Layer | PROJECT_SPEC.md §3 |
| **EventBus** | Pub/sub event system | ARCHITECTURE.md §2 |
| **App** | User-facing program | PROJECT_SPEC.md §6.4 |
| **OSCore** | Main coordinator | ARCHITECTURE.md §1 |
| **Lifecycle** | App state management | ARCHITECTURE.md §4 |
| **RAII** | Resource Acquisition Is Initialization | CODING_RULES.md §5 |
| **ASR** | Automatic Speech Recognition | OBJECTIVES.md |

---

## FAQ

### "Where do I start?"

**Answer:** [README.md](../README.md) to build, then [PROJECT_SPEC.md](PROJECT_SPEC.md) §1-2 for overview.

### "How do I create a new app?"

**Answer:** [PROJECT_SPEC.md](PROJECT_SPEC.md) §7.1 has step-by-step guide.

### "What are the coding standards?"

**Answer:** [CODING_RULES.md](CODING_RULES.md) - complete style guide.

### "How does input work?"

**Answer:** [ARCHITECTURE.md](ARCHITECTURE.md) §3 - data flow diagrams.

### "What's the project vision?"

**Answer:** [OBJECTIVES.md](OBJECTIVES.md) - goals and success criteria.

### "How do I deploy to Raspberry Pi?"

**Answer:** [PROJECT_SPEC.md](PROJECT_SPEC.md) §8.3-8.4.

### "What's the app lifecycle?"

**Answer:** [ARCHITECTURE.md](ARCHITECTURE.md) §4 with state machine.

### "Why C++ and not Rust?"

**Answer:** [PROJECT_SPEC.md](PROJECT_SPEC.md) §11 - design decisions.

---

## Contributing

### Before Your First Contribution

**Read (in order):**
1. [README.md](../README.md) - Build it
2. [OBJECTIVES.md](OBJECTIVES.md) - Understand vision
3. [CODING_RULES.md](CODING_RULES.md) - Follow standards
4. [PROJECT_SPEC.md](PROJECT_SPEC.md) §7 - App development guide

### Before Submitting PR

**Checklist:**
- [ ] Code follows [CODING_RULES.md](CODING_RULES.md)
- [ ] Compiles with zero warnings
- [ ] Tested on desktop
- [ ] Documented if adding features
- [ ] Reviewed [PROJECT_SPEC.md](PROJECT_SPEC.md) relevant sections

---

## Updating Documentation

### When to Update Docs

| Change | Update |
|--------|--------|
| **New feature** | PROJECT_SPEC.md, README.md |
| **Architecture change** | ARCHITECTURE.md, PROJECT_SPEC.md |
| **Style rule** | CODING_RULES.md |
| **New objective** | OBJECTIVES.md |
| **Build change** | README.md |

### Document Owners

| Document | Primary Owner |
|----------|---------------|
| PROJECT_SPEC.md | Tech Lead |
| CODING_RULES.md | Tech Lead |
| ARCHITECTURE.md | Architect |
| OBJECTIVES.md | Product Owner |
| README.md | All |

---

## External Resources

### Official Links

- **Project Repository:** (to be added)
- **Issue Tracker:** (to be added)
- **Discussion Forum:** (to be added)

### Dependencies Documentation

- **SDL2:** https://wiki.libsdl.org/
- **CMake:** https://cmake.org/documentation/
- **Raspberry Pi:** https://www.raspberrypi.com/documentation/
- **Sherpa-ONNX (Voice):** https://github.com/k2-fsa/sherpa-onnx

---

## Document History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-09 | Initial documentation suite created |

---

## Summary

**This documentation suite provides:**

✅ Complete technical specification
✅ Code standards and rules
✅ Architecture deep dive
✅ Project objectives and vision
✅ Quick start guide

**Everything needed to:**
- Understand A-OS
- Build A-OS
- Contribute to A-OS
- Deploy A-OS
- Explain A-OS

**Start here, reference often, contribute confidently.**

---

**For questions or clarifications, refer to the specific documents above or reach out to the project maintainers.**
