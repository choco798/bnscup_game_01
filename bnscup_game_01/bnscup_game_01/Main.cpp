#include <windows.h>

#include <Siv3D.hpp>

#include "Config.hpp"
#include "GameConstants.hpp"
#include "GameScene.hpp"
#include "GameState.hpp"
#include "HowToPlay.hpp"
#include "ProblemManager.hpp"
#include "Renderer.hpp"
#include "ResultScene.hpp"
#include "RhythmOptionScene.hpp"
#include "SceneBase.hpp"
#include "SoundManager.hpp"
#include "TitleScene.hpp"
#include "UiButton.hpp"

enum class SceneID
{
	Title,
	Game,
	Result
};

namespace
{

void InitializeGameAsset()
{
	using namespace GameConstants::Fonts;
	using namespace GameConstants::Rhythm;
	// GameConstantsの定数を使ってフォント登録

	// フォントアセットを登録する（定数使用）
	FontAsset::Register(KEY_TITLE, FontMethod::MSDF, SIZE_TITLE,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(KEY_TITLE_TEXT, FontMethod::MSDF, SIZE_TITLE_TEXT,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(KEY_GAME, FontMethod::MSDF, SIZE_GAME,
						U"Yuji_Boku/YujiBoku-Regular.ttf");
	FontAsset::Register(KEY_RUBY, FontMethod::MSDF, SIZE_RUBY,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(KEY_EXPLANATION, FontMethod::MSDF, SIZE_EXPLANATION,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(KEY_MENU, FontMethod::MSDF, SIZE_MENU, Typeface::Bold);
	FontAsset::Register(KEY_HOW_TO_PLAY, FontMethod::MSDF, SIZE_HOW_TO_PLAY,
						Typeface::Bold);
	FontAsset::Register(KEY_RESULT, FontMethod::MSDF, SIZE_RESULT,
						Typeface::Bold);
	FontAsset::Register(KEY_SCORE, FontMethod::MSDF, SIZE_SCORE);
	FontAsset::Register(KEY_COPYRIGHT, FontMethod::MSDF, SIZE_COPYRIGHT);

	FontAsset::Register(KEY_KANA_BIG, FontMethod::MSDF, SIZE_KANA_BIG,
						Typeface::Regular);
	FontAsset::Register(KEY_KANA_SMALL, FontMethod::MSDF, SIZE_KANA_SMALL,
						Typeface::Regular);
	FontAsset::Register(KEY_UI_SMALL, FontMethod::MSDF, SIZE_UI_SMALL,
						Typeface::Regular);

	TextureAsset::Register(GameConstants::Textures::KEY_TITLE,
						   GameConstants::FilePaths::TITLE_IMAGE);
}

void WaitGameAssetLoad()
{
	using namespace GameConstants::Fonts;
	using namespace GameConstants::Rhythm;
	// 非同期読み込み待機
	const Array<StringView> fontKeys = {
		KEY_TITLE,	  KEY_TITLE_TEXT,  KEY_GAME,	KEY_RUBY,  KEY_EXPLANATION,
		KEY_MENU,	  KEY_HOW_TO_PLAY, KEY_RESULT,	KEY_SCORE, KEY_COPYRIGHT,
		KEY_KANA_BIG, KEY_KANA_SMALL,  KEY_UI_SMALL};

	for (const auto& key : fontKeys)
	{
		FontAsset::Wait(key);
	}

	const Array<StringView> textureKeys = {KEY_TITLE};

	for (const auto& key : fontKeys)
	{
		TextureAsset::Wait(key);
	}

}
}  // namespace

void Main()
{
	// ゲームアセットを準備する
	::InitializeGameAsset();

	// UI座標を定数化
	using namespace GameConstants::UI;
	const RectF panel{HELP_PANEL_POS, HELP_PANEL_SIZE};
	ui::Button closeHowToBtn{U"　閉じる　",
							 GameConstants::Fonts::KEY_HOW_TO_PLAY,
							 Vec2{panel.x + panel.w - CLOSE_BUTTON_OFFSET.x,
								  panel.y + panel.h - CLOSE_BUTTON_OFFSET.y}};

	// シーン管理
	KigoGameApp manager;

	// 設定ロード（定数ファイルパス使用）
	if (manager.get())
	{
		manager.get()->sound.loadAssets();
		manager.get()->saveDataManager.initialize(
			GameConstants::FilePaths::SAVE_DATA);
		manager.get()->configManager.initialize(
			GameConstants::FilePaths::CONFIG);

		// 問題ロード（エラーメッセージ定数使用）
		if (!manager.get()->problemManager.loadFromJSON(
				GameConstants::FilePaths::PROBLEMS))
		{
			System::MessageBoxOK(
				GameConstants::ErrorMessages::PROBLEM_FILE_LOAD_FAILED);
		}

		manager.get()->gameState.initialize();
		manager.get()->renderer.initRenderer(GameConstants::Fonts::KEY_GAME);

		Window::Resize(manager.get()->configManager.ui().clientSizeX,
					   manager.get()->configManager.ui().clientSizeY);
		Scene::SetBackground(BACKGROUND_COLOR);
	}

	::WaitGameAssetLoad();

	manager.add<TitleScene>(State::Title);
	manager.add<GameScene>(State::Game);
	manager.add<ResultScene>(State::Result);
	manager.add<RhythmOptionScene>(State::RhythmOption);

	manager.init(State::Title);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}

		// ヘルプ機能
		if (manager.get())
		{
			// F3 でヘルプ開閉
			if (KeyF3.down())
			{
				manager.get()->showHowToPlay = !manager.get()->showHowToPlay;
			}
			if (manager.get()->showHowToPlay)
			{
				closeHowToBtn.update();

				DrawHowToOverlay();
				closeHowToBtn.draw();
				if (closeHowToBtn.isClicked())
				{
					manager.get()->showHowToPlay = false;
				}
				continue;
			}
		}
	}
}

/*
開発メモ：

コーディングスタイル　Siv3D
https://siv3d.github.io/ja-jp/develop/coding-style/?utm_source=chatgpt.com

# 命名規則

### 変数
変数は camelCase
constexpr 定数は PascalCase

### 配列名
複数形にする
複数形が無い語は ~List とする

### 関数
関数名は PascalCase

### クラス
クラス名は PascalCase
public メンバのみを持つ場合は struct, それ以外は class を使う
class の場合、必要でない限り public: → protected: → private: の順に記述する
struct 内の変数は camelCase
非静的 private メンバ変数は m_ から始めて camelCase で続ける
非静的メンバ関数は camelCase
静的メンバ関数は PascalCase
静的メンバ定数は PascalCase

### 列挙型
列挙型名は PascalCase
列挙子は PascalCase
enum より enum class を使う

# フォーマット
Ctrl + K の後に Ctrl + Dを押すと自動でフォーマットされる。以下は自動で対応される

### インデント
タブ空白を用いる

### 字下げスタイル
オールマンスタイルを用いる

*/
