# Memory Allocator OOP Refactoring

## Class Responsibilities

### MemoryBuffer
- **Single responsibility**: Manage the underlying 8MB memory block
- Allocates raw memory from OS using mmap
- Handles buffer position tracking
- Provides memory deallocation on destruction

### StatisticsManager
- **Single responsibility**: Track and update all statistics
- Records allocations, frees, and failures
- Maintains heap boundary information
- Provides read-only access to statistics

### BlockTracker
- **Single responsibility**: Manage active/freed blocks and coalescing logic
- Tracks active allocations (address → requested size)
- Manages freed blocks (address → aligned size)
- Implements best-fit allocation from freed blocks
- Handles block coalescing during free operations

### AlignmentCalculator
- **Single responsibility**: Handle all alignment calculations
- Calculates proper memory alignment
- Aligns sizes and positions
- Uses standard max alignment

### MemoryAllocator
- **Single responsibility**: Orchestrate all components and provide public interface
- Coordinates all other classes
- Implements malloc/free/calloc logic
- Provides C interface functions

## Design Principles Applied

### Single Responsibility Principle (SRP)
Each class is **focused on one specific job**, making the system modular and maintainable.

### Testability
- **Testable independently**: Each component can be unit tested in isolation
- Clear interfaces between components
- Minimal dependencies between classes

### Maintainability
- **Easier to understand**: Each class has a clear, focused purpose
- **Easier to modify**: Changes to one component don't affect others
- **Clear separation of concerns**: No class knows about implementation details of others

### Loose Coupling
- **Clear interfaces**: Well-defined public methods for each class
- **Minimal dependencies**: Classes interact through interfaces, not implementations
- **High cohesion**: Related functionality grouped together within each class

## Benefits of This Design

1. **Debugging**: Issues can be isolated to specific components
2. **Testing**: Each component can be tested independently with mocks
3. **Extensibility**: New features can be added by extending existing classes or adding new ones
4. **Code reuse**: Components can potentially be reused in other projects
5. **Team collaboration**: Different developers can work on different components simultaneously