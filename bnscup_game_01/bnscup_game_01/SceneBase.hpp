#pragma once

#include <Siv3D.hpp>
#include "GameState.hpp"
#include "Config.hpp"
#include "SoundManager.hpp"
#include "Renderer.hpp"

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
