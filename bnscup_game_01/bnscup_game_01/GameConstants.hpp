#pragma once
#include <Siv3D.hpp>

namespace GameConstants
{
// === 段位システム ===
constexpr int32 PROMOTION_REQUIRED_CORRECT = 5;
constexpr size_t MAX_RANK_INDEX = 3;  // 0:特待生, 1:名人, 2:達人, 3:俳人
constexpr int32 DEFAULT_SELECTED_GRADE = -1;

// === スコアリング ===
constexpr int32 SCORE_MULTIPLIER_BASE = 10;
constexpr int32 INITIAL_SCORE = 0;
constexpr int32 INITIAL_CORRECT_COUNT = 0;

// === ゲームパラメータ（サウンド） ===
namespace Game
{
constexpr double MIN_VOLUME = 0.0;
constexpr double MAX_VOLUME = 1.0;
}  // namespace Game

// === UI描画パラメータ ===
namespace UI
{
// フロー効果
constexpr double FLOW_HINT_BASE_SPEED = 180.0;
constexpr double FLOW_HINT_SWIRL_SCALE = 10.0;
constexpr double FLOW_HINT_SWIRL_FREQ = 6.28318;  // 2π
constexpr double FLOW_HINT_TRAIL_LENGTH = 26.0;
constexpr size_t FLOW_HINT_PARTICLE_COUNT = 300;

// 放射状フェード
constexpr int RADIAL_FADE_STEPS = 16;
constexpr double RADIAL_FADE_ALPHA_BASE = 0.26;

// パステル色設定
constexpr double PASTEL_SAT_MIN = 0.28;
constexpr double PASTEL_SAT_MAX = 0.45;
constexpr double PASTEL_VAL_MIN = 0.95;
constexpr double PASTEL_VAL_MAX = 1.00;

// UI座標とサイズ
constexpr Vec2 HELP_PANEL_POS = Vec2{140, 140};
constexpr Vec2 HELP_PANEL_SIZE = Vec2{1000, 440};
constexpr Vec2 CLOSE_BUTTON_OFFSET = Vec2{120, 44};

// ゲーム画面のレイアウト
constexpr Vec2 GAME_BASE_POSITION = Vec2{120, 60};	// 俳句表示の基準位置
constexpr Vec2 NO_KIGO_BUTTON_POS = Vec2{960, 140};

// 色とグラデーション
constexpr ColorF BACKGROUND_COLOR = ColorF{0.95};
constexpr double FLOW_TIME_DURATION = 3.0;	// フロー効果の持続時間（秒）

// レンダリング関連
constexpr Vec2 TEACHER_POSITION = Vec2{1000, 420};
constexpr ColorF FALLBACK_BACKGROUND_COLOR = ColorF{0.97, 0.97, 0.94};

// 解説パネル
constexpr RectF EXPLANATION_SEASON_PANEL = RectF{60, 470, 120, 50};
constexpr RectF EXPLANATION_MAIN_PANEL = RectF{60, 540, 1100, 140};
constexpr Vec2 EXPLANATION_KIGO_BASE_POS = Vec2{200, 470};
constexpr Vec2 EXPLANATION_KIGO_BASE_SIZE = Vec2{60, 50};
constexpr Vec2 EXPLANATION_TEXT_OFFSET = Vec2{30, 16};
constexpr Vec2 EXPLANATION_SMALL_OFFSET = Vec2{30, 0};
constexpr double EXPLANATION_PANEL_RADIUS = 16.0;
constexpr int EXPLANATION_FRAME_THICKNESS = 2;
constexpr ColorF EXPLANATION_PANEL_COLOR = ColorF{1.0, 1.0, 1.0, 0.9};

// ヘルプオーバーレイ
constexpr RectF HELP_OVERLAY_PANEL = RectF{140, 140, 1000, 440};
constexpr ColorF HELP_OVERLAY_COLOR = ColorF{1.0, 1.0, 1.0, 0.96};
constexpr int HELP_OVERLAY_FRAME_THICKNESS = 3;
constexpr Vec2 HELP_TITLE_POS = Vec2{160, 160};
constexpr Vec2 HELP_TEXT_BASE_POS = Vec2{160, 220};
constexpr int HELP_TEXT_LINE_HEIGHT = 40;

// タイトルシーン
constexpr ColorF TITLE_BACKGROUND_COLOR = ColorF{0.96};
constexpr double TITLE_Y_POS = 160.0;
constexpr double START_BUTTON_Y_POS = 430.0;
constexpr double HOWTO_BUTTON_Y_POS = 520.0;
constexpr double RHYTHM_OPTION_Y_POS = 590.0;
constexpr double EXIT_BUTTON_Y_POS = 660.0;

// 現在段位表示パネル
constexpr RectF CURRENT_RANK_PANEL = RectF{50, 250, 300, 100};
constexpr Vec2 CURRENT_RANK_TEXT_OFFSET = Vec2{16, 10};
constexpr Vec2 CURRENT_RANK_VALUE_OFFSET = Vec2{16, 46};

// 音量スライダー
constexpr RectF BGM_VOLUME_BAR = RectF{820, 620, 360, 10};
constexpr RectF SE_VOLUME_BAR = RectF{820, 660, 360, 10};
constexpr double VOLUME_KNOB_RADIUS = 10.0;
constexpr double VOLUME_BAR_CENTER_RATIO = 0.5;
constexpr int VOLUME_KNOB_FRAME_THICKNESS = 2;
constexpr ColorF VOLUME_BAR_NORMAL_COLOR = ColorF{0.85};
constexpr ColorF VOLUME_BAR_DRAG_COLOR = ColorF{0.4};

// 段位選択
constexpr RectF GRADE_SELECTION_PANEL = RectF{900, 300, 300, 280};
constexpr Vec2 GRADE_SELECTION_TITLE_OFFSET =
	Vec2{150, 20};	// panel center + offset
constexpr Vec2 GRADE_BUTTON_BASE_POS = Vec2{1050, 380};
constexpr double GRADE_BUTTON_Y_STEP = 70.0;
constexpr ColorF GRADE_SELECTION_HIGHLIGHT = ColorF{0.3, 0.6, 1.0, 0.6};
constexpr int GRADE_SELECTION_HIGHLIGHT_STRETCH = 4;

// 著作権表示
constexpr Vec2 COPYRIGHT_OFFSET = Vec2{50, 200};

// リザルトシーン
constexpr Size RESULT_BUTTON_SIZE = Size{260, 56};
constexpr Vec2 RESULT_BUTTON_BASE_POS = Vec2{780, 470};
constexpr double RESULT_TITLE_Y_POS = 140.0;
constexpr RectF RESULT_PANEL = RectF{200, 220, 880, 320};
constexpr double RESULT_PANEL_RADIUS = 20.0;
constexpr Vec2 RESULT_CORRECT_COUNT_POS = Vec2{240, 260};
constexpr Vec2 RESULT_SCORE_POS = Vec2{240, 320};

// ゲームシーン定数
constexpr double HUE_RANGE_MIN = 0.0;
constexpr double HUE_RANGE_MAX = 360.0;
constexpr double FLOW_TIME_RESET = 1.0f;
constexpr double CIRCLE_RADIUS_RATIO = 0.5;	 // (w + h) / 2 の計算用
constexpr double PARTICLE_SIZE_MIN = 4.0;
constexpr double PARTICLE_SIZE_MAX = 12.0;
constexpr double ALPHA_BREATHE_BASE = 0.75;
constexpr double ALPHA_BREATHE_AMPLITUDE = 0.25;
constexpr double SMOOTH_STEP_FACTOR_A = 3.0;
constexpr double SMOOTH_STEP_FACTOR_B = 2.0;
constexpr double PADDING_MULTIPLIER = 2.0;

// 乱数シード関連の定数
constexpr double RANDOM_SEED_1 = 12.9898;
constexpr double RANDOM_SEED_2 = 78.2330;
constexpr double RANDOM_SEED_3 = 34.1234;
constexpr double RANDOM_SEED_4 = 19.9753;
constexpr double RANDOM_MULT_1 = 43758.5453;
constexpr double RANDOM_MULT_2 = 12345.6789;
constexpr double RANDOM_MULT_3 = 98765.4321;
constexpr double RANDOM_MULT_4 = 54321.1234;
constexpr double RANDOM_OFFSET_1 = 0.5;
constexpr double RANDOM_OFFSET_2 = 2.5;
constexpr double RANDOM_OFFSET_3 = 4.5;
constexpr double RANDOM_OFFSET_4 = 6.5;
constexpr double POSITION_OFFSET_1 = 0.37;
constexpr double POSITION_OFFSET_2 = 0.61;
constexpr double SWIRL_MULTIPLIER = 7.0;

// テキストレイアウト定数
constexpr int32 BACK_POSITION_OFFSET = 2;
constexpr int32 LINE_COUNT_START = 1;
constexpr int32 HEAD_POSITION_START = 1;
constexpr bool DISPLAY_SPACE_BOX = false;
}  // namespace UI

// === フォント設定 ===
namespace Fonts
{
constexpr int32 SIZE_TITLE = 64;
constexpr int32 SIZE_TITLE_TEXT = 32;
constexpr int32 SIZE_GAME = 48;
constexpr int32 SIZE_RUBY = 24;
constexpr int32 SIZE_EXPLANATION = 30;
constexpr int32 SIZE_MENU = 48;
constexpr int32 SIZE_HOW_TO_PLAY = 28;
constexpr int32 SIZE_RESULT = 56;
constexpr int32 SIZE_SCORE = 32;
constexpr int32 SIZE_COPYRIGHT = 16;

// フォントキー名
constexpr StringView KEY_TITLE = U"Title";
constexpr StringView KEY_TITLE_TEXT = U"TitleText";
constexpr StringView KEY_GAME = U"Game";
constexpr StringView KEY_RUBY = U"Ruby";
constexpr StringView KEY_EXPLANATION = U"Explanation";
constexpr StringView KEY_MENU = U"Menu";
constexpr StringView KEY_HOW_TO_PLAY = U"HowToPlay";
constexpr StringView KEY_RESULT = U"Result";
constexpr StringView KEY_SCORE = U"Score";
constexpr StringView KEY_COPYRIGHT = U"Copyright";
}  // namespace Fonts

// === ファイルパス ===
namespace FilePaths
{
constexpr StringView SAVE_DATA = U"savedata.json";
constexpr StringView CONFIG = U"config.json";
constexpr StringView PROBLEMS = U"problems.json";
}  // namespace FilePaths

// === エラーメッセージ ===
namespace ErrorMessages
{
constexpr StringView PROBLEM_FILE_LOAD_FAILED =
	U"問題ファイルの読み込みに失敗しました。\\App\\problems.json "
	U"を確認してください。";
constexpr StringView JSON_PARSE_FAILED = U"JSONファイルの解析に失敗しました";
constexpr StringView FILE_NOT_FOUND = U"ファイルが見つかりません";
constexpr StringView MISSING_PROBLEMS_KEY = U"JSONに'problems'キーがありません";
constexpr StringView MICROPHONE_INIT_FAILED = U"マイクの初期化に失敗しました";
constexpr StringView VAD_LOG_FILE_FAILED =
	U"VADログファイルの作成に失敗しました";
}  // namespace ErrorMessages

// === ゲームプレイ設定 ===
namespace Gameplay
{
constexpr bool DEFAULT_SHUFFLE_ENABLED = false;
constexpr double CLICK_TOLERANCE = 0.1;	 // クリック判定の許容範囲
}  // namespace Gameplay

// === 段位名 ===
namespace RankNames
{
// 段位名の取得関数
inline String getRankName(size_t index)
{
	static const Array<String> RANK_NAMES = {
		U"特待生",	// ProblemGrade::Trainee
		U"名人",	// ProblemGrade::Master
		U"達人",	// ProblemGrade::Expert
		U"俳人"		// 最高段位
	};
	return (index < RANK_NAMES.size()) ? RANK_NAMES[index] : U"不明";
}

constexpr size_t RANK_COUNT = 4;
}  // namespace RankNames

// === リズムモード設定 ===
namespace Rhythm
{
// RestPreset相当の設定
constexpr int32 LIGHT_CUT_BEATS = 1;   // |
constexpr int32 STRONG_CUT_BEATS = 2;  // ||
constexpr int32 FERMATA_BEATS = 1;	   // ~（余韻）
constexpr int32 TAIL_END_BEATS = 2;	   // 末尾の余韻

// Layout設定
constexpr double LANE_Y = 300.0;	   // ベース線Y座標
constexpr double PX_PER_BEAT = 160.0;  // 1拍の横幅
constexpr double HIT_X = 240.0;		   // 基準線X座標
constexpr double MORA_RADIUS = 12.0;   // モーラ玉の半径
constexpr double BEAM_Y = 10.0;		   // ビーム縦位置オフセット

// フォントキー
constexpr StringView KEY_KANA_BIG = U"RhythmKanaBig";
constexpr StringView KEY_KANA_SMALL = U"RhythmKanaSmall";
constexpr StringView KEY_UI_SMALL = U"RhythmUISmall";

// フォントサイズ
constexpr int32 SIZE_KANA_BIG = 20;
constexpr int32 SIZE_KANA_SMALL = 16;
constexpr int32 SIZE_UI_SMALL = 18;

// VAD（Voice Activity Detection）設定
constexpr double VAD_ALPHA = 0.02;			 // ノイズEMAの追従度
constexpr double VAD_K_ON = 1.3;			 // ON閾値倍率
constexpr double VAD_K_OFF = 0.8;			 // OFF閾値倍率
constexpr double VAD_ABS_ON = 0.005;		 // ON閾値絶対値
constexpr double VAD_ABS_OFF = 0.01;		 // OFF閾値絶対値
constexpr double VAD_BAND_LOW_HZ = 80.0;	 // 人声帯域下限
constexpr double VAD_BAND_HIGH_HZ = 6000.0;	 // 人声帯域上限
constexpr int32 VAD_MIN_ON_MS = 80;			 // 声あり最小継続時間
constexpr int32 VAD_MIN_OFF_MS = 200;		 // 無音最小継続時間

// デフォルト俳句データ
constexpr StringView DEFAULT_KANA_STREAM =
	U"きゃ=れ=え=だ=に | か=ら=す の と=ま=り~ け=り~ || あ=き の く=れ~";

// デフォルトBPM
constexpr double DEFAULT_BPM = 120.0;

// 描画関連定数
constexpr double FLOW_TIME_DURATION = 3.0;	// フロー効果持続時間
constexpr ColorF GLOW_COLOR_BASE =
	ColorF{1.0, 0.9, 0.6, 0.35};  // グローエフェクト基本色

// RhythmOptionScene UI定数
namespace OptionUI
{
constexpr Vec2 SLIDER_SIZE{400, 20};
constexpr Vec2 MIC_SENSITIVITY_POS{200, 240};
constexpr Vec2 VAD_ALPHA_POS{200, 320};
constexpr Vec2 VAD_K_ON_POS{200, 400};
constexpr Vec2 VAD_K_OFF_POS{200, 480};
constexpr Vec2 BACK_BUTTON_POS{800, 600};

// スライダー範囲
constexpr double VAD_ALPHA_MIN = 0.01;
constexpr double VAD_ALPHA_MAX = 0.11;
constexpr double VAD_K_ON_MIN = 1.0;
constexpr double VAD_K_ON_MAX = 2.0;
constexpr double VAD_K_OFF_MIN = 0.3;
constexpr double VAD_K_OFF_MAX = 1.3;
}  // namespace OptionUI
}  // namespace Rhythm
}  // namespace GameConstants
