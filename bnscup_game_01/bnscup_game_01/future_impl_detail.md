# 実装詳細計画書

## 1. データ構造の拡張

### Problem構造体の拡張
```cpp
struct Problem {
    // 既存のフィールド
    String id;
    String author;
    String source;
    String text;
    bool hasKigo;
    int32 kigoStart;
    int32 kigoEnd;
    String kigo;
    String explanation;
    Array<String> tags;

    // 追加するフィールド
    int32 grade;           // 難易度（0=特待生, 1=名人, 2=達人）
    String ruby;           // フリガナ情報
    String rhythm;         // リズム情報
    bool displayRuby;      // フリガナ表示フラグ
};
```

### セーブデータ形式（savedata.ini）
```ini
[Progress]
current_rank=0          # 現在の段位（0=特待生）
total_score=0          # 累計スコア
unlocked_ranks=0,1     # 解放済み段位（カンマ区切り）

[Audio]
bgm_volume=0.8
se_volume=0.7
```

## 2. 段位システムの実装

### GameState::ranksの調整
```cpp
class GameState {
    // 段位名を簡素化
    Array<String> ranks{
        U"特待生",  // grade 0
        U"名人",    // grade 1
        U"達人",    // grade 2
        U"俳人"     // All Complete
    };
    
    // 段位関連の新規メソッド
    bool isRankUnlocked(size_t rank) const;
    void unlockRank(size_t rank);
    bool canPromoteToNextRank() const;
    void promoteRank();
};
```

### ProblemManager拡張
```cpp
class ProblemManager {
private:
    // 難易度別に問題を管理
    Array<Problem> m_problems[3];  // grade 0,1,2

public:
    // 追加メソッド
    const Array<Problem>& getProblemsForGrade(int32 grade) const;
    void shuffleProblemsForGrade(int32 grade);
    size_t getCompletedCountForGrade(int32 grade) const;
};
```

## 3. UI実装詳細

### TitleSceneの拡張
```cpp
class TitleScene {
private:
    // 段位選択用ボタン
    struct RankButton {
        ui::Button button;
        bool unlocked;
        int32 grade;
    };
    Array<RankButton> m_rankButtons;
    
    // スコア表示用パネル（シンプル化）
    struct ScorePanel {
        RectF rect;
        int32 totalScore;    // 累計スコア
    };
    ScorePanel m_scorePanel;
    
    void drawRankSelection() const;
    void drawScoreInfo() const {
        // 現在の段位名と累計スコアのみ表示
        FontAsset(U"TitleText")(U"現在の段位：{}"_fmt(getData().gameState.currentRankName()))
            .draw(m_scorePanel.rect.pos, Palette::Black);
        FontAsset(U"TitleText")(U"累計スコア：{}"_fmt(m_scorePanel.totalScore))
            .draw(m_scorePanel.rect.pos.movedBy(0, 40), Palette::Black);
    }
};
```

### フリガナ表示システム
```cpp
class Renderer {
private:
    struct RubyText {
        String base;     // 基本テキスト
        String ruby;     // フリガナ
        Vec2 position;   // 表示位置
    };
    
    void drawRubyText(const RubyText& text) const;
    Array<RubyText> parseRubyString(const String& input) const;
};
```

## 4. 実装手順

1. データ構造の拡張
   - Problem構造体の拡張
   - セーブデータ形式の実装
   - ProblemManagerの拡張

2. 段位システム実装
   - GameState::ranksの調整
   - 段位解放システムの実装
   - 問題フィルタリング機能の実装

3. UI実装
   - TitleScene改修（シンプルなスコア表示）
   - 段位選択UI実装
   - フリガナ表示システム実装

4. 音楽機能実装
   - リズム譜表示システム
   - 音声認識システム

## 5. テスト計画

1. 段位システム
   - 問題の難易度別フィルタリング
   - 段位解放条件の確認
   - セーブデータの保存・読み込み

2. UI機能
   - フリガナ表示の確認
   - 段位選択UIの動作確認
   - スコア表示の確認（シンプル化）

3. 音楽機能
   - リズム譜の表示確認
   - 音声認識の精度確認

## 6. 技術的な注意点

1. パフォーマンス
   - フリガナ描画時のメモリ使用量
   - 音声処理の負荷

2. エラー処理
   - セーブデータの破損対策
   - 問題データの検証

3. 拡張性
   - 将来の問題追加への対応
   - 新規機能追加の容易さ

## 7. スケジュール

Week 1:
- データ構造の拡張
- セーブデータシステムの実装

Week 2:
- 段位システムの実装
- ProblemManagerの拡張

Week 3:
- TitleScene改修（シンプル化）
- UI実装

Week 4:
- フリガナ表示システム
- テスト・デバッグ