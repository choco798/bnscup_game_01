#include <windows.h>

#include <Siv3D.hpp>

// ゲーム調整用定数
constexpr int32 ShotSize = 10;
constexpr int32 MaxShots = 3;

struct Shot
{
	double posX{};
	double posY{};
	bool isShot{};
};

struct Sprite
{
	double posX{};
	double posY{};
	double speed{};
	bool isFacingRight{};
	bool isFacingDown{};
	bool isHit{};
	int hp{};
};

struct GameData
{
	// Assets
	const Texture emoji{U"Images/player_left.png"};
	const Texture emojiLeft{U"Images/player_left.png"};
	const Texture emojiRight{U"Images/player_right.png"};
	const Texture emojiUp{U"Images/player_up.png"};
	const Texture emojiDown{U"Images/player_down.png"};
	const Texture background03{U"Images/background_03.png"};
	const Texture background04{U"Images/background_04.png"};
	const Font font{FontMethod::MSDF, 48, Typeface::Bold};

	// State
	int timer{};
	const Texture emojiEnemy{U"🦖"_emoji};
	const Texture emojiEnemy01{U"🐟"_emoji};
	const Texture emojiEnemy02{U"🚗"_emoji};
	int score{};

	double mapPosX[5]{};
	double mapPosY[5]{};

	Shot shots[10]{};
	Sprite sprites[10]{};

	int hitTime{};
	int shotIndex{};
	bool clearFlag{};
};

void GameInit(GameData* data)
{
	Scene::SetBackground(ColorF{Palette::Black});

	data->timer = 0;
	data->score = 0;
	data->shotIndex = 0;
	data->clearFlag = false;

	// Player
	data->sprites[0].speed = 2.0;
	data->sprites[0].posX = 400;
	data->sprites[0].posY = 200;
	data->sprites[0].isFacingRight = true;
	data->sprites[0].isFacingDown = true;

	// Enemy
	data->sprites[1].speed = 1.0;
	data->sprites[1].posX = 800;
	data->sprites[1].posY = 100;
	data->sprites[1].isFacingRight = false;
	data->sprites[1].isHit = false;

	for (int i = 2; i <= 3; ++i)
	{
		data->sprites[i].speed = 1.0;
		data->sprites[i].posX = -100;
		data->sprites[i].isFacingRight = false;
		data->sprites[i].hp = 3;
		data->sprites[i].isHit = false;
	}

	// Boss
	data->sprites[6].speed = 0.5;
	data->sprites[6].posX = -100;
	data->sprites[6].posY = -500;
	data->sprites[6].isFacingRight = false;
	data->sprites[6].hp = 30;
	data->sprites[6].isHit = false;

	// Shots
	for (int i = 0; i <= 4; ++i)
	{
		data->shots[i].isShot = false;
		data->shots[i].posX = -300;
		data->shots[i].posY = -300;
	}

	// Map
	for (int i = 0; i < 5; ++i)
	{
		data->mapPosX[i] = 0;
		data->mapPosY[i] = 0;
	}
}

void MapRoutine(GameData* data)
{
	data->mapPosX[0] -= 1;
	if (data->mapPosX[0] < -800)
	{
		data->mapPosX[0] = 0;
	}
}

void EnemyRoutine(GameData* data)
{
	if ((data->timer % 2) == 0)
	{
		data->sprites[1].posX -= data->sprites[1].speed;
		data->sprites[2].posX -= data->sprites[2].speed;
		data->sprites[3].posX -= data->sprites[3].speed;

		if (data->sprites[6].posX >= 400)
		{
			data->sprites[6].posX -= data->sprites[6].speed;
		}
		else if (data->sprites[6].posX >= 0)
		{
			data->sprites[6].posY -= data->sprites[6].speed;
			if (data->sprites[6].posY <= 0)
			{
				data->sprites[6].posY = 600;
			}
		}
	}

	if (data->sprites[1].posX <= -100)
	{
		data->sprites[1].posX = 800;
	}

	if (data->timer == 3000)
	{
		data->sprites[2].posY = 100;
		data->sprites[2].posX = 800;
		data->sprites[3].posY = 500;
		data->sprites[3].posX = 800;
	}

	if (data->timer == 5999)
	{
		data->sprites[6].posY = 300;
		data->sprites[6].posX = 800;
	}

	if (data->sprites[1].isHit)
	{
		data->sprites[1].posX = 900;
		data->sprites[1].posY += 100;
		if (data->sprites[1].posY > 500)
		{
			data->sprites[1].posY = 100;
		}
		data->sprites[1].isHit = false;
	}

	if (data->sprites[2].isHit)
	{
		--data->sprites[2].hp;
		if (data->sprites[2].hp <= 0)
		{
			data->score += 50;
			data->sprites[2].posX = -100;
			data->sprites[2].posY = -100;
			data->sprites[2].hp = 3;
		}
		data->sprites[2].isHit = false;
	}

	if (data->sprites[3].isHit)
	{
		--data->sprites[3].hp;
		if (data->sprites[3].hp <= 0)
		{
			data->score += 50;
			data->sprites[3].posX = -100;
			data->sprites[3].posY = -100;
			data->sprites[3].hp = 3;
		}
		data->sprites[3].isHit = false;
	}

	if (data->sprites[6].isHit)
	{
		--data->sprites[6].hp;
		if (data->sprites[6].hp <= 0)
		{
			data->score += 1000;
			data->sprites[3].posX = -100;
			data->sprites[3].posY = -500;
			data->sprites[3].hp = 30;
			data->clearFlag = true;
		}
		data->sprites[6].isHit = false;
	}
}

void ShotRoutine(GameData* data)
{
	if (KeySpace.down())
	{
		if (!data->shots[data->shotIndex].isShot)
		{
			data->shots[data->shotIndex].posX = data->sprites[0].posX + 5;
			data->shots[data->shotIndex].posY = data->sprites[0].posY;
			data->shots[data->shotIndex].isShot = true;

			++(data->shotIndex);
			if (data->shotIndex >= MaxShots)
			{
				data->shotIndex = 0;
			}
		}
	}

	for (int i = 0; i < MaxShots; ++i)
	{
		if (data->shots[i].isShot)
		{
			data->shots[i].posX += 10;
		}
		else
		{
			data->shots[i].posX = -200;
		}

		if (data->shots[i].posX > 800)
		{
			data->shots[i].isShot = false;
		}
	}
}

void PlayerRoutine(GameData* data)
{
	if (KeyLeft.pressed())
	{
		data->sprites[0].posX -= data->sprites[0].speed;
		data->sprites[0].isFacingRight = false;
	}
	if (KeyRight.pressed())
	{
		data->sprites[0].posX += data->sprites[0].speed;
		data->sprites[0].isFacingRight = true;
	}
	if (KeyUp.pressed())
	{
		data->sprites[0].posY -= data->sprites[0].speed;
		data->sprites[0].isFacingDown = false;
	}
	if (KeyDown.pressed())
	{
		data->sprites[0].posY += data->sprites[0].speed;
		data->sprites[0].isFacingDown = true;
	}

	ShotRoutine(data);
}

void JudgementRoutine(GameData* data)
{
	for (int i = 0; i < MaxShots; ++i)
	{
		if (Circle{data->shots[i].posX, data->shots[i].posY, ShotSize}
				.intersects(
					Circle{data->sprites[1].posX, data->sprites[1].posY, 30}))
		{
			data->sprites[1].isHit = true;
			data->shots[i].isShot = false;
			data->hitTime = data->timer + 100;
			data->score += 10;
		}

		if (Circle{data->shots[i].posX, data->shots[i].posY, ShotSize}
				.intersects(
					Circle{data->sprites[2].posX, data->sprites[2].posY, 30}))
		{
			data->sprites[2].isHit = true;
			data->shots[i].isShot = false;
			data->hitTime = data->timer + 100;
		}

		if (Circle{data->shots[i].posX, data->shots[i].posY, ShotSize}
				.intersects(
					Circle{data->sprites[3].posX, data->sprites[3].posY, 30}))
		{
			data->sprites[3].isHit = true;
			data->shots[i].isShot = false;
			data->hitTime = data->timer + 100;
		}

		if (Circle{data->shots[i].posX, data->shots[i].posY, ShotSize}
				.intersects(
					Circle{data->sprites[6].posX, data->sprites[6].posY, 60}))
		{
			data->sprites[6].isHit = true;
			data->shots[i].isShot = false;
			data->hitTime = data->timer + 100;
		}
	}
}

void Print(GameData* data)
{
	data->background03.draw(data->mapPosX[0], data->mapPosY[0]);

	for (int i = 0; i < 16; ++i)
	{
		data->background04.draw(50 * i, 0);
	}

	data->emoji.scaled(0.75)
		.mirrored(data->sprites[0].isFacingRight)
		.drawAt(data->sprites[0].posX, data->sprites[0].posY);

	data->emojiEnemy.scaled(0.75)
		.mirrored(data->sprites[1].isFacingRight)
		.drawAt(data->sprites[1].posX, data->sprites[1].posY);

	data->emojiEnemy01.scaled(0.75)
		.mirrored(data->sprites[2].isFacingRight)
		.drawAt(data->sprites[2].posX, data->sprites[2].posY);

	data->emojiEnemy01.scaled(0.75)
		.mirrored(data->sprites[3].isFacingRight)
		.drawAt(data->sprites[3].posX, data->sprites[3].posY);

	data->emojiEnemy02.scaled(2)
		.mirrored(data->sprites[6].isFacingRight)
		.drawAt(data->sprites[6].posX, data->sprites[6].posY);

	for (int i = 0; i < MaxShots; ++i)
	{
		if (data->shots[i].isShot)
		{
			Circle{data->shots[i].posX, data->shots[i].posY, ShotSize}.draw(
				Palette::Seagreen);
		}
	}

	data->font(U"Your Score:", data->score)
		.draw(32, Vec2{20, 500}, {Palette::Mistyrose});

	if (data->timer <= data->hitTime)
	{
		if (!data->clearFlag)
		{
			data->font(U"Hit!").draw(64, Vec2{0, 0}, {Palette::Gold});
		}
		else
		{
			data->font(U"Congraduation!")
				.draw(128, Vec2{0, 0}, {Palette::Gold});
			System::Sleep(5000ms);
			System::Exit();
		}
	}
}

void TimerRoutine(GameData* data)
{
	++(data->timer);
	if (data->timer > 6000)
	{
		data->timer = 0;
	}
}

void Main()
{
	GameData data;

	GameInit(&data);

	while (System::Update())
	{
		MapRoutine(&data);
		EnemyRoutine(&data);
		PlayerRoutine(&data);
		JudgementRoutine(&data);
		::Print(&data);
		TimerRoutine(&data);
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
