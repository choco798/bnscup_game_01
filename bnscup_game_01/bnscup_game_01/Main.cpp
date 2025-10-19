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

enum class SceneID
{
	Title,
	Game,
	Result
};

namespace
{
// タイトル画面のボタン矩形（TitleScene の描画と揃える）
RectF GetTitleStartRect()
{
	return RectF{Arg::center = Vec2{Scene::Center().x, 360}, 360, 72};
}

RectF GetTitleHowToRect()
{
	return RectF{Arg::center = Vec2{Scene::Center().x, 460}, 360, 72};
}

RectF GetTitleExitRect()
{
	return RectF{Arg::center = Vec2{Scene::Center().x, 560}, 360, 72};
}

// 簡易ヘルプ表示
void DrawHowToOverlay(const Font& font)
{
	const RectF panel{140, 140, 1000, 440};
	panel.rounded(16).draw(ColorF{1.0, 1.0, 1.0, 0.96});
	panel.drawFrame(3, 0, Palette::Black);
	font(U"あそびかた").draw(160, 160, Palette::Black);

	const Font text{28};
	text(U"・俳句の中の季語の文字をクリックします。")
		.draw(160, 220, Palette::Black);
	text(U"・季語が無い句は「季語なし」をクリックします。")
		.draw(160, 260, Palette::Black);
	text(U"・正解で +10 点。解説が表示されます。")
		.draw(160, 300, Palette::Black);
	text(U"・本バージョンでは問題順はファイル順です。")
		.draw(160, 340, Palette::Black);
	text(U"・スコアは実行中のみ保持されます。").draw(160, 380, Palette::Black);

	s3d::RoundRect{
		RectF{panel.x + panel.w - 200, panel.y + panel.h - 68, 160, 48}, 12}
		.draw(Palette::White)
		.drawFrame(3, 0, Palette::Black);
	text(U"閉じる").drawAt(panel.x + panel.w - 120, panel.y + panel.h - 44,
						   Palette::Black);
}

bool ClickedCloseOnHowTo(const RectF& panel)
{
	const RectF close{panel.x + panel.w - 200, panel.y + panel.h - 68, 160, 48};
	return (close.mouseOver() && MouseL.down());
}
}  // namespace

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{0.95});

	// 設定ロード
	Config config;
	config.load(U"config.json");

	Font font{48, Typeface::Regular};

	ProblemManager problemManager;
	problemManager.loadFromJSON(U"problems.json");

	// ゲーム状態
	GameState state{problemManager.getProblems()};

	// レンダラ／サウンド
	Renderer renderer{config, font};
	SoundManager sound;
	sound.loadAssets();

	SceneID scene = SceneID::Title;
	std::unique_ptr<SceneBase> current = std::make_unique<TitleScene>(sound);

	// 問題ロード
	if (!problemManager.loadFromJSON(U"problems.json"))
	{
		System::MessageBoxOK(
			U"問題ファイルの読み込みに失敗しました。\nAssets/problems.json "
			U"を確認してください。");
	}
	state.problems = problemManager.getProblems();


	// ヘルプ（遊び方）オーバーレイの状態
	bool showHowTo = false;

	while (System::Update())
	{
		// シーンごとの更新・描画
		current->update();
		current->draw();

		//========================
		//  タイトル → 遷移制御
		//========================
		if (scene == SceneID::Title)
		{
			// 遊び方オーバーレイ表示
			if (showHowTo)
			{
				DrawHowToOverlay(font);

				const RectF panel{140, 140, 1000, 440};
				if (ClickedCloseOnHowTo(panel))
				{
					showHowTo = false;
				}
				continue;
			}

			// ボタンヒットテスト（簡易）
			if (GetTitleStartRect().mouseOver() && MouseL.down())
			{
				// ゲーム開始
				state.state_reset();
				if (state.problems.isEmpty())
				{
					System::MessageBoxOK(U"出題できる問題がありません。");
					continue;
				}
				current = std::make_unique<GameScene>(state, renderer, sound,
													  config, font);
				scene = SceneID::Game;
				continue;
			}
			if (GetTitleHowToRect().mouseOver() && MouseL.down())
			{
				showHowTo = true;
				continue;
			}
			if (GetTitleExitRect().mouseOver() && MouseL.down())
			{
				break;	// アプリ終了
			}
		}

		//========================
		//  ゲーム → 遷移制御
		//========================
		if (scene == SceneID::Game)
		{
			// GameScene 内部で問題を進め、末尾まで到達したら結果へ
			if (state.currentIndex >= state.problems.size())
			{
				current = std::make_unique<ResultScene>(state, renderer, sound);
				scene = SceneID::Result;
				continue;
			}
		}

		//========================
		//  リザルト → 遷移制御
		//========================
		if (scene == SceneID::Result)
		{
			// 「タイトルへ」相当のボタン領域（ResultScene
			// の描画座標と合わせる）
			const RectF backBtn{780, 470, 260, 56};
			if (backBtn.mouseOver() && MouseL.down())
			{
				current = std::make_unique<TitleScene>(sound);
				scene = SceneID::Title;
				continue;
			}
		}

		// デバッグ：F1 でヘルプ開閉
		if (KeyF1.down())
		{
			if (scene == SceneID::Title)
			{
				showHowTo = !showHowTo;
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
