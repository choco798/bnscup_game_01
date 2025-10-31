# Data Structures Documentation

## Configuration Structures

### UIConfig
```cpp
struct UIConfig {
    int32 clientSizeX;        // Window width
    int32 clientSizeY;        // Window height
    // Other UI-related settings
};
```

### AudioConfig
```cpp
struct AudioConfig {
    double masterVolume;      // Master volume level
    double bgmVolume;         // Background music volume
    double seVolume;          // Sound effects volume
};
```

### SaveData
```cpp
struct SaveData {
    int32 currentRank;        // Current player rank/grade
    Array<bool> clearedProblems;  // Problem completion status
    int32 totalScore;        // Accumulated score
    Array<int32> gradeProgress;  // Progress in each grade
};
```

## Game Data Structures

### Problem
```cpp
struct Problem {
    String id;               // Unique problem identifier
    String author;           // Haiku author
    String source;           // Source/collection name
    String text;            // Haiku text
    bool hasKigo;           // Whether contains seasonal reference
    int32 kigoStart;        // Start index of kigo
    int32 kigoEnd;          // End index of kigo
    String kigo;            // The seasonal reference
    String explanation;     // Problem explanation
    Array<String> tags;     // Metadata tags
    int32 grade;           // Difficulty grade (0-2)
    bool completed;         // Completion status
    String ruby;           // Furigana information
    String rhythm;         // Rhythm notation
    bool displayRuby;      // Ruby display flag
    
    bool isValid() const;   // Validates problem data
};
```

### Grade Constants
```cpp
struct Grade {
    static constexpr int32 Trainee = 0;  // Entry level
    static constexpr int32 Master = 1;   // Intermediate
    static constexpr int32 Expert = 2;   // Advanced
    static constexpr int32 Count = 3;    // Total grades
};
```

## JSON Data Formats

### config.json
```json
{
    "ui": {
        "clientSizeX": 1280,
        "clientSizeY": 720
    },
    "audio": {
        "masterVolume": 1.0,
        "bgmVolume": 0.8,
        "seVolume": 0.7
    }
}
```

### problems.json
```json
{
    "gameTitle": "ブレバトの季語探しゲーム",
    "version": "1.0",
    "problems": [
        {
            "id": "furuike_001",
            "author": "松尾芭蕉",
            "source": "俳諧",
            "text": "古池や 蛙飛びこむ 水の音",
            "ruby": "古(ふる)池(いけ)や 蛙(かわず)飛(と)びこむ 水(みず)の音(おと)",
            "rhythm": "44444!64444444!644444",
            "hasKigo": true,
            "kigo": "蛙",
            "kigoStart": 4,
            "kigoEnd": 5,
            "explanation": "芭蕉が自然の静けさと生命の音を感じ取った代表作です。",
            "tags": ["春", "有名句"],
            "grade": 0,
            "displayRuby": true
        }
    ]
}
```

### savedata.json
```json
{
    "currentRank": 0,
    "clearedProblems": [true, false, false],
    "totalScore": 1000,
    "gradeProgress": [2, 0, 0]
}
```

## UI Component Structures

### LayoutChar
```cpp
struct LayoutChar {
    String text;           // Character text
    String ruby;          // Ruby (furigana) text
    Vec2 position;        // Display position
    RectF bounds;         // Bounding rectangle
    bool isKigo;          // Seasonal word flag
};
```

### ButtonState
```cpp
enum class ButtonState {
    Normal,
    Hover,
    Pressed,
    Disabled
};
```

### ButtonStyle
```cpp
struct ButtonStyle {
    Font font;            // Button text font
    ColorF normalColor;   // Normal state color
    ColorF hoverColor;    // Hover state color
    ColorF pressColor;    // Pressed state color
    ColorF textColor;     // Text color
    double roundness;     // Corner roundness
};
```

## Scene Management Structures

### SceneState
```cpp
enum class State {
    Title,      // Title screen
    Game,       // Main gameplay
    Result      // Result display
};
```

### GameData
```cpp
struct GameData {
    ConfigManager configManager;     // Configuration
    SoundManager sound;             // Audio system
    SaveDataManager saveDataManager; // Save data
    GameState gameState;            // Game state
    Renderer renderer;              // Graphics
    bool showHowToPlay;            // Tutorial flag
};
```

## Memory Layout and Optimization

### Data Structure Size Considerations
```cpp
// Example memory layout optimization
struct Problem {
    // Strings are reference counted in Siv3D
    String id;           // 24 bytes
    String text;         // 24 bytes
    
    // POD types
    int32 kigoStart;    // 4 bytes
    int32 kigoEnd;      // 4 bytes
    int32 grade;        // 4 bytes
    bool hasKigo;       // 1 byte
    bool completed;      // 1 byte
    bool displayRuby;   // 1 byte
    // 3 bytes padding
};
```

### Container Usage Guidelines

1. **Array Usage**
```cpp
// Prefer Array for dynamic collections
Array<Problem> problems;        // Dynamic size
Array<bool> clearedProblems;   // Completion status

// Use std::array for fixed-size collections
std::array<int32, Grade::Count> gradeProgress;
```

2. **String Handling**
```cpp
// Siv3D String type for text
String text;        // UTF-32 encoding
String ruby;        // Furigana text

// Use StringView for non-owning references
StringView GetKigo() const { return text.substr(kigoStart, kigoEnd - kigoStart); }
```

## Data Validation Rules

### Problem Validation
```cpp
bool Problem::isValid() const {
    return isBasicDataValid()     // Check basic fields
        && isKigoValid()          // Validate kigo data
        && isGradeValid()         // Check grade value
        && isRubyValid()          // Validate ruby text
        && isRhythmValid();       // Check rhythm data
}
```

### Configuration Validation
```cpp
bool ConfigManager::validate() const {
    // Window size limits
    static constexpr int32 MinWidth = 800;
    static constexpr int32 MaxWidth = 1920;
    
    return (ui.clientSizeX >= MinWidth && ui.clientSizeX <= MaxWidth)
        && (audio.masterVolume >= 0.0 && audio.masterVolume <= 1.0);
}