# Technical Specifications

## Development Environment

### Framework and Libraries
- **Primary Framework**: Siv3D
- **Build System**: CMake
- **Language Standard**: C++17 or later
- **Platform**: Windows (Direct3D 11)

### Required Tools
- Visual Studio with C++ development tools
- CMake 3.14 or later
- Git for version control

## System Requirements

### Minimum Requirements
- OS: Windows 10 64-bit
- CPU: Intel Core i3 or equivalent
- RAM: 4GB
- Graphics: DirectX 11 compatible GPU
- Storage: 500MB available space

### Recommended Requirements
- OS: Windows 10/11 64-bit
- CPU: Intel Core i5 or equivalent
- RAM: 8GB
- Graphics: DirectX 11 compatible GPU with 2GB VRAM
- Storage: 1GB available space

## Project Structure

### Code Organization
```
bnscup_game_01/
├── CMakeLists.txt           # Build configuration
├── Config.hpp/cpp           # Configuration structures
├── GameScene.hpp/cpp        # Main gameplay implementation
├── GameState.hpp/cpp        # Game state management
├── Problem.hpp/cpp          # Problem data structures
├── ProblemManager.hpp/cpp   # Problem loading/management
├── Renderer.hpp/cpp         # Graphics rendering
├── SaveDataManager.hpp/cpp  # Save data handling
├── SoundManager.hpp/cpp     # Audio management
└── TextLayout.hpp/cpp       # Text rendering utilities
```

### Asset Organization
```
App/
├── config.json       # Game configuration
├── problems.json    # Problem definitions
├── savedata.json    # Save data
├── bgm_se/         # Audio files
│   ├── correct1.mp3
│   ├── correct2.mp3
│   ├── wrong1.mp3
│   └── wrong2.mp3
└── Images/         # Image assets
    ├── background_*.png
    └── player_*.png
```

## Implementation Standards

### Coding Style
Based on Siv3D coding guidelines:

#### Naming Conventions
- Variables: camelCase
- Constants: PascalCase
- Functions: PascalCase
- Classes: PascalCase
- Namespaces: lowercase
- Files: PascalCase.hpp/cpp

#### Class Structure
```cpp
class ExampleClass
{
public:
    // Public interface
    void publicMethod();

protected:
    // Protected members
    int m_protectedMember;

private:
    // Private implementation
    void privateMethod();
    int m_privateMember;
};
```

### Error Handling
- Use exception handling for file operations
- Return bool for operations that can fail gracefully
- Log errors for debugging purposes
- Show user-friendly error messages in release builds

## Core Systems Specifications

### Configuration System
- File Format: JSON
- Location: App/config.json
- Components:
  - UI settings (window size, fonts)
  - Audio settings (volume levels)
  - Game settings (difficulty, etc.)

### Problem Management
- File Format: JSON
- Location: App/problems.json
- Features:
  - Problem validation
  - Grade-based filtering
  - Progress tracking
  - Dynamic loading

### Save Data System
- File Format: JSON
- Location: App/savedata.json
- Data Types:
  - Player progress
  - Completion status
  - High scores
  - Grade levels

#### Auto-Save Feature
- Trigger: ResultScene終了時（タイトルに戻る前）
- Saved Data:
  - 累積スコア
  - 問題のクリア状況
  - 段位と進捗状況
- Error Handling:
  - セーブ失敗時は`System::MessageBoxOK`でエラー通知
  - メッセージスタイル：MessageBoxStyle::Error
  - シンプルな1回のセーブ試行

### Audio System
- Format Support: MP3
- Categories:
  - BGM (background music)
  - SE (sound effects)
- Features:
  - Volume control
  - Multiple channel support
  - Async loading

### Rendering System
- Graphics API: Direct3D 11
- Text Rendering:
  - MSDF font support
  - Unicode text handling
  - Ruby text support
- UI Components:
  - Buttons
  - Text layouts
  - Progress indicators

## Performance Requirements

### Frame Rate
- Target: 60 FPS
- Minimum: 30 FPS

### Memory Usage
- Runtime: < 500MB
- Asset Loading: < 200MB

### Loading Times
- Initial Load: < 5 seconds
- Level Load: < 2 seconds
- Save/Load: < 1 second

## Testing Requirements

### Unit Tests
- Framework: Built-in test support
- Coverage: Core game logic
- Validation: Data structures

### Integration Tests
- Scene transitions
- Data persistence
- Asset loading
- Input handling

### Performance Tests
- Frame rate monitoring
- Memory usage tracking
- Load time measurement

## Security Considerations

### Save Data
- Format validation
- Corruption detection
- Backup creation

### Asset Protection
- Resource validation
- Checksum verification
- Error recovery

## Future Compatibility

### Versioning
- Semantic versioning for releases
- Save data versioning
- Configuration versioning

### Extensibility
- Modular architecture
- Plugin support preparation
- Asset pipeline scalability

## Dependencies

### External Libraries
- Siv3D Framework
- zstd compression
- JSON parser

### Asset Requirements
- Font files (MSDF format)
- Audio files (MP3)
- Image files (PNG)

## Build Configuration

### CMake Settings
```cmake
# Minimum required version
cmake_minimum_required(VERSION 3.14)

# Project name and language
project(bnscup_game_01 CXX)

# C++ standard requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Build Targets
- Debug: Development build with debugging symbols
- Release: Optimized build for distribution
- RelWithDebInfo: Release with debugging information

### Platform-Specific Settings
- Windows:
  - Visual Studio 2019 or later
  - Windows SDK 10.0
  - Direct3D 11 support