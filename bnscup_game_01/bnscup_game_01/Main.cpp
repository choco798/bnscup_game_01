#include <windows.h>

#include <Siv3D.hpp>

#include "Config.hpp"
#include "GameScene.hpp"
#include "GameState.hpp"
#include "ProblemManager.hpp"
#include "Renderer.hpp"
#include "ResultScene.hpp"
#include "SceneBase.hpp"
#include "SoundManager.hpp"
#include "TitleScene.hpp"
#include "UiButton.hpp"
#include "HowToPlay.hpp"

enum class SceneID
{
	Title,
	Game,
	Result
};

namespace
{

bool ClickedCloseOnHowTo(const RectF& panel)
{
	const RectF close{panel.x + panel.w - 200, panel.y + panel.h - 68, 160, 48};
	return (close.mouseOver() && MouseL.down());
}

void InitializeGameAsset()
{
	// フォントアセットを登録する
	FontAsset::Register(U"Title", FontMethod::MSDF, 64,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(U"TitleText", FontMethod::MSDF, 32,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(U"Game", FontMethod::MSDF, 48,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(U"Explanation", FontMethod::MSDF, 30,
						Typeface::CJK_Regular_JP);
	FontAsset::Register(U"Menu", FontMethod::MSDF, 48, Typeface::Bold);
	FontAsset::Register(U"HowToPlay", FontMethod::MSDF, 28, Typeface::Bold);
	FontAsset::Register(U"Result", FontMethod::MSDF, 56, Typeface::Bold);
	FontAsset::Register(U"Score", FontMethod::MSDF, 32);
	FontAsset::Register(U"Copyright", FontMethod::MSDF, 16);
	FontAsset::Wait(U"Title");
	FontAsset::Wait(U"TitleText");
	FontAsset::Wait(U"Game");
	FontAsset::Wait(U"Explanation");
	FontAsset::Wait(U"Menu");
	FontAsset::Wait(U"HowToPlay");
	FontAsset::Wait(U"Result");
	FontAsset::Wait(U"Score");
	FontAsset::Wait(U"Copyright");
}
}  // namespace

void Main()
{
	// ゲームアセットを準備する
	::InitializeGameAsset();

	// 設定ロード
	Config config;
	config.load(U"config.json");

	Window::Resize(config.ui().clientSizeX, config.ui().clientSizeY);
	Scene::SetBackground(ColorF{0.95});

	const RectF panel{140, 140, 1000, 440};
	ui::Button closeHowToBtn{U"　閉じる　", U"HowToPlay",
						Vec2{panel.x + panel.w - 120, panel.y + panel.h - 44}};

	// 問題ロード
	ProblemManager problemManager;
	if (!problemManager.loadFromJSON(U"problems.json"))
	{
		System::MessageBoxOK(
			U"問題ファイルの読み込みに失敗しました。\nAssets/problems.json "
			U"を確認してください。");
	}

	// シーン管理
	KigoGameApp manager;

	if (manager.get())
	{
		manager.get()->sound.loadAssets();
		manager.get()->gameState = GameState{problemManager.getProblems()};
		manager.get()->renderer.initRenderer(U"Game");
	}

	manager.add<TitleScene>(State::Title);
	manager.add<GameScene>(State::Game);
	manager.add<ResultScene>(State::Result);

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
