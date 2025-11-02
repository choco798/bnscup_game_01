# API リファレンス

本文書では、「季語シンクロ！」プロジェクトの主要クラスとその API について説明します。

## コアクラス

### ConfigManager（設定管理）
JSON形式の設定ファイルを管理するクラスです。

```cpp
class ConfigManager {
public:
    // 初期化
    bool initialize(const FilePath& path);
    
    // 設定取得
    const UIConfig& ui() const noexcept;
    const AudioConfig& audio() const noexcept;
    const RhythmConfig& rhythm() const noexcept;  // 新規追加
    
    // 設定更新
    void saveConfig() const;
    bool validate() const;
    
    // UI設定の更新
    void updateUIConfig(const UIConfig& newConfig);
    
    // 音響設定の更新
    void updateAudioConfig(const AudioConfig& newConfig);
    
    // リズム設定の更新（拡張機能）
    void updateRhythmConfig(const RhythmConfig& newConfig);
};
```

**使用例:**
```cpp
ConfigManager config;
if (config.initialize(U"App/config.json")) {
    auto& uiConfig = config.ui();
    int width = uiConfig.clientSizeX;
    int height = uiConfig.clientSizeY;
}
```

### ProblemManager（問題管理）
俳句問題の読み込み、管理、検索を行うクラスです。

```cpp
class ProblemManager {
public:
    // 問題読み込み
    bool loadFromJSON(const FilePath& path);
    
    // 問題取得
    Array<Problem> getProblems() const;
    Array<Problem> getProblemsByGrade(int32 grade) const;
    size_t size() const noexcept;
    size_t sizeByGrade(int32 grade) const noexcept;
    
    // 個別問題管理
    const Problem& getProblem(size_t index) const;
    const Problem& getProblemByGrade(int32 grade, size_t index) const;
    
    // 検証とフィルタリング
    bool validate() const;
    Array<Problem> filterProblemsByGrade(int32 grade) const;
    
    // 段位関連
    int32 getSelectedGrade() const noexcept;
    void setSelectedGrade(int32 grade);
    
    // 統計情報
    size_t getTotalProblemsCount() const noexcept;
    Array<int32> getAvailableGrades() const;
};
```

**使用例:**
```cpp
ProblemManager manager;
if (manager.loadFromJSON(U"App/problems.json")) {
    // 特定段位の問題を取得
    auto problems = manager.getProblemsByGrade(0); // 特待生の問題
    
    // 問題の詳細を取得
    if (!problems.empty()) {
        const auto& problem = problems[0];
        String text = problem.text;
        bool hasKigo = problem.hasKigo;
    }
}
```

### SaveDataManager（セーブデータ管理）
プレイヤーの進捗、スコア、クリア状況を管理するクラスです。

```cpp
class SaveDataManager {
public:
    // 初期化
    void initialize(const FilePath& path);
    bool load();
    bool save();
    
    // 段位管理
    size_t getCurrentRank() const noexcept;
    void setCurrentRank(size_t rank);
    bool canAccessGrade(size_t grade) const noexcept;
    
    // 段位別進捗
    size_t getGradeProgress(size_t grade) const;
    void incrementGradeProgress(size_t grade);
    void resetGradeProgress(size_t grade);
    bool isGradeCompleted(size_t grade) const;
    
    // スコア管理
    int32 getTotalScore() const noexcept;
    void addScore(int32 score);
    int32 getGradeScore(size_t grade) const;
    int32 getHighScore() const noexcept;
    
    // 問題クリア状況
    bool isProblemCleared(size_t index) const;
    void setProblemCleared(size_t index, bool cleared = true);
    Array<bool> getGradeClearStatus(size_t grade) const;
    
    // 統計情報
    size_t getTotalProblemsCleared() const;
    double getOverallProgress() const; // 0.0-1.0
    String getLastPlayDate() const;
    
    // データ管理
    void backup();
    bool restore();
    void reset(); // 全データリセット
};
```

**使用例:**
```cpp
SaveDataManager saveData;
saveData.initialize(U"App/savedata.json");

// プレイヤーの現在状況を確認
size_t currentRank = saveData.getCurrentRank();
int32 totalScore = saveData.getTotalScore();

// 問題クリア後の処理
saveData.addScore(120);
saveData.setProblemCleared(problemIndex, true);
saveData.incrementGradeProgress(currentGrade);
saveData.save();
```

### GameScene（ゲームシーン）
メインゲームプレイを担当するシーンクラスです。

```cpp
class GameScene : public KigoGameApp::Scene {
public:
    // コンストラクタ
    GameScene(const InitData& init);
    
    // ゲームフロー
    void startProblem();
    void update() override;
    void draw() const override;
    
    // UI描画
    void drawKigoRect() const;
    void drawWordRect(s3d::int32 i, const UIConfig& ui) const;
    void drawHaikuRect() const;
    void drawScoreDisplay() const;
    void drawGradeDisplay() const;
    
    // レイアウト計算
    Vec2 getKigoRectCenter() const;
    RectF getHaikuArea() const;
    RectF getButtonArea() const;
    
    // ユーティリティ
    static RectF Inflate(const RectF& r, double padPx, double padScale);
    
    // エフェクト描画
    void drawCorrectEffect() const;
    void drawWrongEffect() const;
    void drawFlowHint(const RectF& area) const;

private:
    // 入力処理
    void handleClick();
    void handleKeyInput();
    
    // ゲーム状態変更
    void ExecWrong();
    void ExecCorrect();
    void nextProblem();
    
    // 判定処理
    bool isHitKigo() const;
    bool isHitNoKigoButton() const;
    int32 getClickedCharIndex() const;
    
    // 内部状態
    Problem m_currentProblem;
    Array<LayoutChar> m_layoutChars;
    double m_effectTimer;
    bool m_showExplanation;
    int32 m_currentScore;
};
```

**使用例:**
```cpp
// GameSceneの初期化
GameScene scene(initData);

// 問題開始
scene.startProblem();

// メインループ内で
scene.update(); // 状態更新
scene.draw();   // 描画処理
```

### SoundManager（音響管理）
ゲーム内の音響効果を管理するクラスです。

```cpp
class SoundManager {
public:
    // アセット管理
    void loadAssets();
    void unloadAssets();
    bool isLoaded() const;
    
    // 再生制御
    void playBGM(const String& name, bool loop = true);
    void playSE(const String& name);
    void stopBGM();
    void stopAllSE();
    void pauseBGM();
    void resumeBGM();
    
    // 音量制御
    void setMasterVolume(double volume);    // 0.0-1.0
    void setBGMVolume(double volume);       // 0.0-1.0
    void setSEVolume(double volume);        // 0.0-1.0
    double getMasterVolume() const;
    double getBGMVolume() const;
    double getSEVolume() const;
    
    // 状態確認
    bool isBGMPlaying() const;
    bool isBGMPaused() const;
    
    // 特殊効果
    void fadeIn(const String& bgmName, double duration);
    void fadeOut(double duration);
    
    // リズム機能用（拡張）
    void playRhythmSound(const String& soundName, double timing);
    void setRhythmBPM(double bpm);
};
```

**使用例:**
```cpp
SoundManager sound;
sound.loadAssets();

// BGM再生
sound.playBGM(U"op01");

// 効果音再生
sound.playSE(U"correct1");

// 音量調整
sound.setBGMVolume(0.4);
sound.setSEVolume(0.8);
```

### Renderer（描画システム）
ゲーム画面の描画を統括するクラスです。

```cpp
class Renderer {
public:
    // 初期化
    void initRenderer(const String& fontName);
    void setDefaultFont(const String& fontName);
    
    // テキスト描画
    void drawText(const String& text, const Vec2& pos) const;
    void drawText(const String& text, const Vec2& pos, const ColorF& color) const;
    void drawRuby(const String& text, const String& ruby, const Vec2& pos) const;
    void drawCenteredText(const String& text, const RectF& area) const;
    
    // 俳句専用描画
    void drawHaiku(const Problem& problem, const RectF& area) const;
    void drawHaikuWithHighlight(const Problem& problem, const RectF& area,
                               int32 highlightStart, int32 highlightEnd) const;
    
    // UI描画
    void drawBackground() const;
    void drawScoreDisplay(int32 score, const Vec2& pos) const;
    void drawGradeDisplay(const String& gradeName, const Vec2& pos) const;
    void drawExplanation(const String& text) const;
    void drawProgressBar(double progress, const RectF& area) const;
    
    // エフェクト描画
    void drawParticleEffect(const Vec2& center, const ColorF& color) const;
    void drawGlowEffect(const RectF& area, const ColorF& color) const;
    void drawFadeTransition(double alpha) const;
    
    // スタイル管理
    void setFontSize(int32 size);
    void setColor(const ColorF& color);
    void setRubySize(int32 size);
    void setLineHeight(double height);
    
    // レンダリング状態
    void pushRenderState();
    void popRenderState();
    void resetRenderState();
};
```

**使用例:**
```cpp
Renderer renderer;
renderer.initRenderer(U"YujiBoku");

// 俳句の描画
renderer.drawHaiku(problem, haikuArea);

// スコア表示
renderer.drawScoreDisplay(1200, Vec2(50, 50));

// エフェクト描画
renderer.drawParticleEffect(mousePos, Palette::Gold);
```

### TextLayouter（テキストレイアウト）
日本語テキストとルビのレイアウトを計算するクラスです。

```cpp
class TextLayouter {
public:
    // レイアウト生成
    Array<LayoutChar> layout(const String& text, const String& ruby = {});
    Array<LayoutChar> layoutHaiku(const Problem& problem);
    Array<LayoutChar> layoutWithWordWrap(const String& text, double maxWidth);
    
    // レイアウト設定
    void setBaseSize(double size);
    void setRubySize(double size);
    void setSpacing(double space);
    void setLineHeight(double height);
    void setMaxWidth(double width);
    
    // 位置計算
    RectF calculateBounds(const Array<LayoutChar>& chars) const;
    Vec2 calculateCenterPosition(const Array<LayoutChar>& chars, const RectF& area) const;
    double calculateTextWidth(const String& text) const;
    double calculateTextHeight(const String& text) const;
    
    // 特殊レイアウト
    Array<LayoutChar> layoutVertical(const String& text);
    Array<LayoutChar> layoutInCircle(const String& text, double radius);
    
    // ヒット判定サポート
    int32 getCharIndexAt(const Array<LayoutChar>& chars, const Vec2& pos) const;
    RectF getCharBounds(const Array<LayoutChar>& chars, int32 index) const;
    
    // ルビ処理
    struct RubyInfo {
        String baseText;
        String rubyText;
        int32 startIndex;
        int32 endIndex;
    };
    Array<RubyInfo> parseRuby(const String& rubyText) const;
};
```

**使用例:**
```cpp
TextLayouter layouter;
layouter.setBaseSize(24);
layouter.setRubySize(12);

// 俳句のレイアウト
auto chars = layouter.layoutHaiku(problem);

// ヒット判定
Vec2 mousePos = Cursor::Pos();
int32 charIndex = layouter.getCharIndexAt(chars, mousePos);
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
## 定数と列挙型

### ゲーム定数（GameConstants）
```cpp
namespace GameConstants {
    namespace Game {
        constexpr int32 PROBLEMS_PER_GRADE = 5;    // 段位あたりの問題数
        constexpr int32 MAX_GRADE = 3;             // 最大段位
        constexpr double BASE_SCORE = 100.0;       // 基本スコア
        constexpr double SPEED_BONUS_MAX = 50.0;   // 速答ボーナス最大値
    }
    
    namespace UI {
        constexpr double DEFAULT_FONT_SIZE = 24.0;
        constexpr double RUBY_FONT_SIZE = 12.0;
        constexpr double BUTTON_HEIGHT = 40.0;
        constexpr double MARGIN = 20.0;
    }
    
    namespace RankNames {
        inline String getRankName(size_t index) {
            static const Array<String> RANK_NAMES = {
                U"特待生", U"名人", U"達人", U"俳人"
            };
            return (index < RANK_NAMES.size()) ? RANK_NAMES[index] : U"不明";
        }
    }
    
    namespace Rhythm {
        constexpr double DEFAULT_BPM = 120.0;
        constexpr double MIN_BPM = 60.0;
        constexpr double MAX_BPM = 200.0;
        constexpr double BEAT_TOLERANCE = 0.1;
    }
}
```

### 列挙型
```cpp
// シーン状態
enum class SceneID {
    Title,
    Game,
    Result,
    RhythmOption
};

// ボタン状態
enum class ButtonState {
    Normal,
    Hover,
    Pressed,
    Disabled
};

// 段位レベル
enum class GradeLevel {
    Trainee = 0,    // 特待生
    Master = 1,     // 名人
    Expert = 2,     // 達人
    Haijin = 3      // 俳人
};

// ゲーム状態
enum class GamePhase {
    Waiting,        // 待機中
    Playing,        // プレイ中
    Answering,      // 回答中
    ShowingResult,  // 結果表示中
    Completed       // 完了
};
```

## データ構造

### 設定構造体
```cpp
struct UIConfig {
    int32 clientSizeX = 1280;
    int32 clientSizeY = 720;
    double wordWidth = 80.0;
    double lineHeightScale = 1.25;
    double lineWidthScale = 1.25;
    double maxLineWidth = 920.0;
    double hitboxPaddingPx = 6.0;
    double hitboxPaddingScale = 0.1;
};

struct AudioConfig {
    double bgmVolume = 0.4;
    double seVolume = 0.8;
};

struct RhythmConfig {
    double bpm = 120.0;
    double micSensitivity = 2.0;
    bool rhythmModeEnabled = true;
    VoiceActivityDetectorParams vadParams;
};
```

### ゲームデータ構造
```cpp
struct Problem {
    String id;              // 一意識別子
    String author;          // 作者
    String source;          // 出典
    String text;           // 俳句本文
    String ruby;           // ルビ付きテキスト
    String rhythm;         // リズム記譜
    bool hasKigo;          // 季語の有無
    String kigo;           // 季語文字列
    int32 kigoStart;       // 季語開始位置
    int32 kigoEnd;         // 季語終了位置
    String explanation;    // 解説
    String note;           // 注記
    Array<String> tags;    // タグ
    int32 grade;          // 難易度
    bool displayRuby;     // ルビ表示フラグ
    
    // 検証メソッド
    bool isValid() const;
    bool hasValidKigo() const;
    StringView getKigoText() const;
};

struct SaveData {
    int32 currentRank = 0;
    Array<bool> clearedProblems;
    int32 totalScore = 0;
    Array<int32> gradeProgress;
    String lastPlayDate;
    
    // 統計データ
    int32 totalPlayTime = 0;        // 総プレイ時間（秒）
    int32 correctAnswers = 0;       // 正解数
    int32 totalAnswers = 0;         // 総回答数
    Array<int32> gradeScores;       // 段位別スコア
};
```

## エラーハンドリング

### 例外クラス
```cpp
class GameException : public std::exception {
public:
    GameException(const String& message) : m_message(message) {}
    const char* what() const noexcept override;
    
private:
    String m_message;
};

class FileLoadException : public GameException {
public:
    FileLoadException(const FilePath& path) 
        : GameException(U"ファイル読み込みエラー: " + path) {}
};

class ConfigurationException : public GameException {
public:
    ConfigurationException(const String& configName)
        : GameException(U"設定エラー: " + configName) {}
};
```

### エラー処理関数
```cpp
// ファイル操作エラーハンドリング
template<typename T>
Optional<T> safeLoadFile(const FilePath& path) {
    try {
        return T::loadFromFile(path);
    } catch (const FileLoadException& e) {
        logError(U"FileLoad", e.what());
        return none;
    }
}

// JSON検証
bool validateJSON(const JSON& json, const Array<String>& requiredFields) {
    for (const auto& field : requiredFields) {
        if (!json.hasElement(field)) {
            logError(U"JSON", U"必須フィールドが見つかりません: " + field);
            return false;
        }
    }
    return true;
}

// エラーログ出力
void logError(const String& component, const String& message) {
    Console.writeln(U"[ERROR] {}: {}"_fmt(component, message));
    // 必要に応じてファイルログ出力
}

// ユーザー向けエラー表示
void showErrorMessage(const String& message) {
    System::MessageBoxOK(message, MessageBoxStyle::Error);
}
```

## パフォーマンス最適化

### メモリ管理
```cpp
// オブジェクトプール
template<typename T>
class ObjectPool {
public:
    T* acquire() {
        if (m_pool.empty()) {
            return new T();
        }
        T* obj = m_pool.back();
        m_pool.pop_back();
        return obj;
    }
    
    void release(T* obj) {
        if (obj) {
            obj->reset(); // オブジェクトの状態をリセット
            m_pool.push_back(obj);
        }
    }
    
private:
    Array<T*> m_pool;
};

// スマートキャッシュ
template<typename Key, typename Value>
class Cache {
public:
    Optional<Value> get(const Key& key) {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            it->second.lastAccess = Time::GetSec();
            return it->second.value;
        }
        return none;
    }
    
    void put(const Key& key, const Value& value) {
        m_cache[key] = CacheEntry{value, Time::GetSec()};
        cleanup();
    }
    
private:
    struct CacheEntry {
        Value value;
        double lastAccess;
    };
    
    std::unordered_map<Key, CacheEntry> m_cache;
    void cleanup(); // 古いエントリの削除
};
```

### フレームレート最適化
```cpp
// フレームレート管理
class FrameRateManager {
public:
    void setTargetFPS(double fps) { m_targetFPS = fps; }
    void beginFrame() { m_frameStart = Time::GetSec(); }
    void endFrame() {
        double frameTime = Time::GetSec() - m_frameStart;
        double targetTime = 1.0 / m_targetFPS;
        if (frameTime < targetTime) {
            System::Sleep(static_cast<int32>((targetTime - frameTime) * 1000));
        }
    }
    
    double getCurrentFPS() const {
        return Graphics::GetFPS();
    }
    
private:
    double m_targetFPS = 60.0;
    double m_frameStart = 0.0;
};

// 描画最適化
class RenderOptimizer {
public:
    // 画面外判定
    bool isVisible(const RectF& rect) const {
        return rect.intersects(Scene::Rect());
    }
    
    // LOD (Level of Detail) 管理
    int32 calculateLOD(const Vec2& position, const Vec2& viewCenter) const {
        double distance = position.distanceFrom(viewCenter);
        if (distance < 100.0) return 0; // 高品質
        if (distance < 300.0) return 1; // 中品質
        return 2; // 低品質
    }
};
```

## デバッグ支援

### デバッグ情報表示
```cpp
class DebugInfo {
public:
    static void drawFPS(const Vec2& pos = Vec2(10, 10)) {
        FontAsset(U"Debug")(U"FPS: {:.1f}"_fmt(Graphics::GetFPS()))
            .draw(pos, Palette::White);
    }
    
    static void drawMemoryUsage(const Vec2& pos = Vec2(10, 30)) {
        size_t memUsage = System::GetMemoryUsage();
        FontAsset(U"Debug")(U"Memory: {:.1f} MB"_fmt(memUsage / 1024.0 / 1024.0))
            .draw(pos, Palette::White);
    }
    
    static void drawGameState(const GameData& gameData, const Vec2& pos = Vec2(10, 50)) {
        auto& saveData = gameData.saveDataManager;
        FontAsset(U"Debug")(U"Grade: {}, Score: {}"_fmt(
            saveData.getCurrentRank(), saveData.getTotalScore()))
            .draw(pos, Palette::White);
    }
};

// プロファイラー
class Profiler {
public:
    static void beginSection(const String& name) {
        s_sections[name].startTime = Time::GetSecSinceEpoch();
    }
    
    static void endSection(const String& name) {
        auto& section = s_sections[name];
        section.totalTime += Time::GetSecSinceEpoch() - section.startTime;
        section.callCount++;
    }
    
    static void drawReport(const Vec2& pos = Vec2(10, 100)) {
        int32 y = static_cast<int32>(pos.y);
        for (const auto& [name, data] : s_sections) {
            double avgTime = data.totalTime / data.callCount;
            FontAsset(U"Debug")(U"{}: {:.3f}ms ({}calls)"_fmt(
                name, avgTime * 1000.0, data.callCount))
                .draw(Vec2(pos.x, y), Palette::Yellow);
            y += 20;
        }
    }
    
private:
    struct SectionData {
        double startTime = 0.0;
        double totalTime = 0.0;
        int32 callCount = 0;
    };
    static std::unordered_map<String, SectionData> s_sections;
};

// スコープ付きプロファイラー
class ScopedProfiler {
public:
    ScopedProfiler(const String& name) : m_name(name) {
        Profiler::beginSection(m_name);
    }
    
    ~ScopedProfiler() {
        Profiler::endSection(m_name);
    }
    
private:
    String m_name;
};

#define PROFILE_SCOPE(name) ScopedProfiler _prof(name)
```

## テスト支援

### 単体テスト
```cpp
// 簡易テストフレームワーク
class TestRunner {
public:
    static void runAllTests() {
        testProblemManager();
        testSaveDataManager();
        testConfigManager();
        printResults();
    }
    
private:
    static void testProblemManager() {
        TEST_CASE(U"ProblemManager基本機能") {
            ProblemManager manager;
            ASSERT_TRUE(manager.loadFromJSON(U"App/problems.json"));
            ASSERT_GT(manager.size(), 0);
            
            const auto& problem = manager.getProblem(0);
            ASSERT_FALSE(problem.text.empty());
        }
    }
    
    static void printResults() {
        Console.writeln(U"テスト結果: {}/{} 通過"_fmt(s_passedTests, s_totalTests));
    }
    
    static int32 s_passedTests;
    static int32 s_totalTests;
};

#define TEST_CASE(name) \
    Console.writeln(U"テスト: " + String(name)); \
    s_totalTests++; \
    try {

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("アサーション失敗: " #condition); \
    }

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))
#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define ASSERT_GT(a, b) ASSERT_TRUE((a) > (b))
```

## 拡張性のためのインターフェース

### プラグインシステム（将来実装）
```cpp
// プラグインインターフェース
class IGamePlugin {
public:
    virtual ~IGamePlugin() = default;
    virtual bool initialize() = 0;
    virtual void finalize() = 0;
    virtual String getName() const = 0;
    virtual String getVersion() const = 0;
    
    // フック関数
    virtual void onProblemStart(const Problem& problem) {}
    virtual void onProblemComplete(bool correct, int32 score) {}
    virtual void onSceneChange(SceneID from, SceneID to) {}
};

// プラグインマネージャー
class PluginManager {
public:
    bool loadPlugin(const FilePath& pluginPath);
    void unloadPlugin(const String& pluginName);
    void unloadAllPlugins();
    
    Array<String> getLoadedPlugins() const;
    IGamePlugin* getPlugin(const String& name) const;
    
    // イベント配信
    void broadcastProblemStart(const Problem& problem);
    void broadcastProblemComplete(bool correct, int32 score);
    void broadcastSceneChange(SceneID from, SceneID to);
    
private:
    std::unordered_map<String, std::unique_ptr<IGamePlugin>> m_plugins;
};
```

---

**APIリファレンス バージョン**: 2.0  
**最終更新**: 2025年11月2日  
**対応ゲームバージョン**: 1.0以降

このAPIリファレンスは、「季語シンクロ！」の開発において重要な関数とクラスの使用方法を示しています。各機能の詳細な実装例と注意点も含まれているため、開発時の参考としてご活用ください。