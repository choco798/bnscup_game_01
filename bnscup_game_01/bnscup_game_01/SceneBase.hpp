#pragma once

#include <Siv3D.hpp>
#include "Config.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "SoundManager.hpp"

class SceneBase
{
   public:
	virtual ~SceneBase() = default;
	virtual void update(bool enable) = 0;
	virtual void draw() const = 0;
};

// シーンのステート
enum class State
{
	Title,
	Game,
	Result,
};

// 共有するデータ
struct GameData
{
	SoundManager sound{};
	const Config config{};
	bool showHowToPlay{};
	GameState gameState{};
	Renderer renderer{};
};

using KigoGameApp = SceneManager<State, GameData>;
