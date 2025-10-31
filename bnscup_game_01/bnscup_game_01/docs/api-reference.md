# API Reference

## Core Classes

### ConfigManager
```cpp
class ConfigManager {
public:
    // Initialization
    bool initialize(const FilePath& path);
    
    // Configuration Access
    const UIConfig& ui() const noexcept;
    const AudioConfig& audio() const noexcept;
    
    // Configuration Update
    void saveConfig() const;
    bool validate() const;
};
```

### ProblemManager
```cpp
class ProblemManager {
public:
    // Problem Loading
    bool loadFromJSON(const FilePath& path);
    
    // Problem Access
    Array<Problem> getProblems() const;
    size_t size() const noexcept;
    
    // Problem Management
    const Problem& getProblem(size_t index) const;
    bool validate() const;
};
```

### SaveDataManager
```cpp
class SaveDataManager {
public:
    // Initialization
    void initialize(const FilePath& path);
    
    // Progress Management
    size_t getCurrentRank() const noexcept;
    void setCurrentRank(size_t rank);
    
    // Grade Progress
    size_t getGradeProgress(size_t grade) const;
    void incrementGradeProgress(size_t grade);
    
    // Score Management
    int32 getTotalScore() const noexcept;
    void addScore(int32 score);
    
    // Problem Status
    bool isProblemCleared(size_t index) const;
    void setProblemCleared(size_t index, bool cleared = true);
};
```

### GameScene
```cpp
class GameScene : public KigoGameApp::Scene {
public:
    // Construction
    GameScene(const InitData& init);
    
    // Game Flow
    void startProblem();
    void update() override;
    void draw() const override;
    
    // UI Elements
    void drawKigoRect() const;
    void drawWordRect(s3d::int32 i, const UIConfig& ui) const;
    void drawHiakuRect() const;
    Vec2 getKigoRectCenter() const;
    
    // Utility
    static RectF Inflate(const RectF& r, double padPx, double padScale);

private:
    void handleClick();
    void ExecWrong();
    void ExecCorrect();
    bool isHitKigo() const;
};
```

### SoundManager
```cpp
class SoundManager {
public:
    // Asset Management
    void loadAssets();
    
    // Playback Control
    void playBGM(const String& name);
    void playSE(const String& name);
    void stopBGM();
    
    // Volume Control
    void setMasterVolume(double volume);
    void setBGMVolume(double volume);
    void setSEVolume(double volume);
};
```

### Renderer
```cpp
class Renderer {
public:
    // Initialization
    void initRenderer(const String& fontName);
    
    // Drawing Functions
    void drawText(const String& text, const Vec2& pos) const;
    void drawRuby(const String& text, const String& ruby, const Vec2& pos) const;
    void drawExplanation(const String& text) const;
    
    // Style Management
    void setFontSize(int32 size);
    void setColor(const ColorF& color);
};
```

### TextLayouter
```cpp
class TextLayouter {
public:
    // Layout Generation
    Array<LayoutChar> layout(const String& text, const String& ruby = {});
    
    // Layout Configuration
    void setBaseSize(double size);
    void setRubySize(double size);
    void setSpacing(double space);
    
    // Position Calculation
    RectF calculateBounds(const Array<LayoutChar>& chars) const;
};
```

## UI Components

### ui::Button
```cpp
namespace ui {
    class Button {
    public:
        // Construction
        Button(const String& text, const String& fontName, const Vec2& pos);
        
        // State Management
        void update();
        void draw() const;
        
        // Event Handling
        bool isClicked() const;
        bool isHovered() const;
        
        // Style Configuration
        void setStyle(const ButtonStyle& style);
        void setEnabled(bool enabled);
    };
}
```

## Scene Management

### SceneBase
```cpp
class SceneBase {
public:
    // Scene Lifecycle
    virtual void update() = 0;
    virtual void draw() const = 0;
    
    // State Management
    State getState() const;
    void setState(State newState);
    
    // Data Access
    GameData& getData();
    const GameData& getData() const;
};
```

## Utility Functions

### Text Drawing
```cpp
// Draw radial fade circle effect
void DrawRadialFadeCircle(const Circle& c, const ColorF& base, int steps = 16);

// Generate pastel color for UI
ColorF GeneratePastelColor();

// Draw flow hint with pastel colors
void DrawFlowHintPastel(const RectF& area, Vec2 dir, double t01,
                       const ColorF& base = ColorF(1.0));
```

### How To Play
```cpp
// Draw the how-to-play overlay
void DrawHowToOverlay();
```

### Game Asset Management
```cpp
// Initialize game assets (fonts, etc.)
void InitializeGameAsset();
```

## Event System

### Input Events
```cpp
// Mouse Input
bool isMouseOver(const RectF& rect);
bool isClicked(const RectF& rect);

// Keyboard Input
bool isKeyPressed(KeyCode key);
bool isKeyReleased(KeyCode key);
```

### Scene Events
```cpp
// Scene Transitions
void transitionTo(State newState);
void popScene();

// Scene Updates
void updateScene();
void drawScene() const;
```

## Constants and Enums

### Scene States
```cpp
enum class SceneID {
    Title,
    Game,
    Result
};
```

### Button States
```cpp
enum class ButtonState {
    Normal,
    Hover,
    Pressed,
    Disabled
};
```

### Grade Levels
```cpp
struct Grade {
    static constexpr int32 Trainee = 0;
    static constexpr int32 Master = 1;
    static constexpr int32 Expert = 2;
    static constexpr int32 Count = 3;
};
```

## Error Handling

### File Operations
```cpp
// Load file with error handling
Optional<String> loadTextFile(const FilePath& path);

// Save file with backup
bool saveTextFile(const FilePath& path, const String& content);

// Validate JSON data
bool validateJSON(const JSON& json, const Array<String>& requiredFields);
```

### Runtime Errors
```cpp
// Display error message to user
void showErrorMessage(const String& message);

// Log error for debugging
void logError(const String& component, const String& message);