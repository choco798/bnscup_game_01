#pragma once

#include <Siv3D.hpp>
#include "ConfigManager.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "SaveDataManager.hpp"
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
	ConfigManager configManager{};
	SaveDataManager saveDataManager{};
	bool showHowToPlay{};
	GameState gameState{saveDataManager, configManager};
	Renderer renderer{};
};

using KigoGameApp = SceneManager<State, GameData>;
