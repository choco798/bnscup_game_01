#include "GameScene.hpp"
#include "GameConstants.hpp"

// ローカル関数定義

ColorF GeneratePastelColor()
{
	using namespace GameConstants::UI;
	// HSV空間でランダムに生成（パステル調にする）
	double hue = Random(HUE_RANGE_MIN, HUE_RANGE_MAX);	// 色相：全範囲
	double saturation =
		Random(PASTEL_SAT_MIN, PASTEL_SAT_MAX);	 // 彩度：低め（0〜1）
	double value =
		Random(PASTEL_VAL_MIN, PASTEL_VAL_MAX);	 // 明度：高め（明るい色）

	return HSV{hue, saturation, value};
}

void DrawRadialFadeCircle(const Circle& c, const ColorF& base,
						  int steps = GameConstants::UI::RADIAL_FADE_STEPS)
{
	using namespace GameConstants;

	double prevR = 0.0;
	for (int i = 1; i <= steps; ++i)
	{
		double t = static_cast<double>(i) / steps;	// 0 → 1（中心→外周）
		double r = c.r * t;							// 現在の半径
		double thickness = r - prevR;				// リング幅（重ならない）
		// 線形フェード：中心1.0 → 外周0.0
		double a = 1.0 - t;

		// ちょっと柔らかく（スムースステップ）
		a = a * a * (UI::SMOOTH_STEP_FACTOR_A - UI::SMOOTH_STEP_FACTOR_B * a);

		Circle(c.center, r).drawFrame(thickness, 0.0, ColorF(base, base.a * a));
		prevR = r;
	}
}

// 0〜1に丸める補助
static inline double frac(double x)
{
	return x - std::floor(x);
}

// 方向ヒント（風・粒子の流れ）：t01 ∈ [0,1] を進行度として受け取り描画する
// area: 描画エリア / dir: 流れの方向ベクトル / t01: 0〜1の時間 / count: 粒子数
void DrawFlowHintPastel(
	const RectF& area, Vec2 dir, double t01,
	size_t count = GameConstants::UI::FLOW_HINT_PARTICLE_COUNT)
{
	using namespace GameConstants;

	// デフォルト方向（未指定なら右向き）
	if (dir.isZero())
	{
		dir = Vec2{1, 0};
	}
	const Vec2 nDir = dir.normalized();

	// 流れの見た目チューニング
	const double baseSpeed =
		UI::FLOW_HINT_BASE_SPEED;  // 基本スピード（px / 周期）
	const double swirlScale = UI::FLOW_HINT_SWIRL_SCALE;  // 横揺れの大きさ
	const double swirlFreq = UI::FLOW_HINT_SWIRL_FREQ;	// 横揺れの周波数（2π）
	const double trailLength = UI::FLOW_HINT_TRAIL_LENGTH;	// 軌跡の長さ
	const double alphaBase =
		(t01 < 0.1)
			? t01 * 10 * UI::RADIAL_FADE_ALPHA_BASE
			: ((0.9 < t01)
				   ? (1 - t01) * 10 * UI::RADIAL_FADE_ALPHA_BASE
				   : UI::RADIAL_FADE_ALPHA_BASE);  // 粒子の基本アルファ（やわらかく）
	const double satMin = UI::PASTEL_SAT_MIN,
				 satMax = UI::PASTEL_SAT_MAX;  // パステル彩度
	const double valMin = UI::PASTEL_VAL_MIN,
				 valMax = UI::PASTEL_VAL_MAX;  // パステル明度

	// 直交方向（横揺れ用）
	const Vec2 ortho = Vec2{-nDir.y, nDir.x};

	for (size_t i = 0; i < count; ++i)
	{
		// 擬似乱数（インデックスのみから決まる → 純粋）
		const double r0 =
			frac(std::sin((i + UI::RANDOM_OFFSET_1) * UI::RANDOM_SEED_1) *
				 UI::RANDOM_MULT_1);
		const double r1 =
			frac(std::sin((i + UI::RANDOM_OFFSET_2) * UI::RANDOM_SEED_2) *
				 UI::RANDOM_MULT_2);
		const double r2 =
			frac(std::sin((i + UI::RANDOM_OFFSET_3) * UI::RANDOM_SEED_3) *
				 UI::RANDOM_MULT_3);
		const double r3 =
			frac(std::sin((i + UI::RANDOM_OFFSET_4) * UI::RANDOM_SEED_4) *
				 UI::RANDOM_MULT_4);

		// 進行度に対する開始オフセット（ループ感を出す）
		const double phase = frac(r0 + t01);

		// エリア内の初期位置（決定的に散らす）
		Vec2 basePos{area.x + area.w * frac(r1 + UI::POSITION_OFFSET_1 * i),
					 area.y + area.h * frac(r2 + UI::POSITION_OFFSET_2 * i)};

		// 進行に応じた移動量（エリア対角を基準に大きさ調整）
		const double loopDist = baseSpeed;	// 1 周期での移動距離（px）
		Vec2 advect = nDir * (loopDist * phase);

		// 横揺れ（やわらかい流れ感）
		double wobble =
			std::sin((r3 * UI::SWIRL_MULTIPLIER + phase) * swirlFreq);
		Vec2 offset = ortho * (wobble * swirlScale);

		// 位置合成 → エリア内にラップ（はみ出しをループ）
		Vec2 p = basePos + advect + offset;

		auto wrap = [](double v, double a, double b)
		{
			const double w = (b - a);
			v = std::fmod((v - a), w);
			if (v < 0) v += w;
			return v + a;
		};

		p.x = wrap(p.x, area.x, area.x + area.w);
		p.y = wrap(p.y, area.y, area.y + area.h);

		// パステル色（HSV）：色相は全域、彩度低め、明度高め
		const double hue = UI::HUE_RANGE_MAX * r1;
		const double sat = Math::Lerp(satMin, satMax, r2);
		const double val = Math::Lerp(valMin, valMax, r3);

		// 粒子サイズとアルファ（中心ほど濃く、端は淡く）
		const double size =
			Math::Lerp(UI::PARTICLE_SIZE_MIN, UI::PARTICLE_SIZE_MAX, r0);
		const double a =
			alphaBase * (UI::ALPHA_BREATHE_BASE +
						 UI::ALPHA_BREATHE_AMPLITUDE *
							 std::cos(phase * Math::Pi * 2));  // ゆるく呼吸する

		const ColorF col = HSV{hue, sat, val, a};

		// 軌跡（やわらかい流れ方向のヒント）
		const Vec2 tail = p - nDir * trailLength;
		(void)tail;
		// Line{tail, p}.draw(2.0, col);
		Circle{p, size}.draw(col);
	}
}

////////////////////////
// ここからGameScene
////////////////////////
GameScene::GameScene(const InitData& init) : IScene(init)
{
	getData().sound.stopBGM();

	// リズムモード初期化
	initializeRhythmMode();

	startProblem();

	m_noKigoBtn = ui::Button(U"季語なし", GameConstants::Fonts::KEY_GAME,
							 GameConstants::UI::NO_KIGO_BUTTON_POS);
	m_startStopBtn = ui::Button(U"開始/停止", GameConstants::Fonts::KEY_GAME,
								GameConstants::UI::START_STOP_BUTTON_POS);
}

void GameScene::startProblem()
{
	m_showExplanation = false;
	m_result = false;
	m_hintGauge = 0.0;
	m_hintCheckPoint = 0.0;

	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		return;
	}

	const auto& ui = getData().configManager.ui();
	const auto& problem =
		getData().gameState.problems[getData().gameState.currentIndex];

	// 画面分割対応：左半分のサイズで計算
	const double halfWidth = static_cast<double>(ui.clientSizeX) * 0.5;
	TextLayouter layouter{GameConstants::Fonts::KEY_GAME,
						  Min(ui.maxLineWidth, halfWidth - 40),
						  ui.lineHeightScale, ui.lineWidthScale, halfWidth};

	// フリガナ表示が有効な場合はlayoutWithRubyを使用
	if (problem.displayRuby && !problem.ruby.isEmpty())
	{
		m_chars = layouter.layoutWithRuby(problem.text, problem.ruby);
	}
	else
	{
		m_chars = layouter.layout(problem.text);
	}

	// 俳句表示の開始位置（左上）にオフセットを与える
	Vec2 base = GameConstants::UI::GAME_BASE_POSITION;
	double minLeftX = 0;
	for (auto& c : m_chars)
	{
		minLeftX = Min(minLeftX, c.box.leftX());
	}
	base.x -= minLeftX;
	for (auto& c : m_chars)
	{
		c.pos += base;
		c.rubyPos += base;
		c.box.moveBy(base);
	}

	// リズムモード：モーラデータの解析
	if (m_rhythmModeActive && !problem.rhythm.isEmpty())
	{
		// モーラストリームをパース
		RestPreset restPreset;
		m_parsedStream = KanaStreamParser::Parse(problem.rhythm, restPreset);

		// BeatTransportをリセットして開始
		m_beatTransport.reset();
		m_beatTransport.setPreRoll(0.5);
		m_beatHitDetector.reset();
	}
}

void GameScene::update()
{
	// ミスクリック時に方向を示す
	if (m_flowTime > 0.0f)
	{
		m_flowTime -=
			Scene::DeltaTime() / GameConstants::UI::FLOW_TIME_DURATION;
	}

	// チュートリアルに関しては特別扱い
	const bool isTutorial = (getData().gameState.currentIndex == 0) &&
							(getData().gameState.currentRankName() ==
							 GameConstants::RankNames::getRankName(0));
	const bool isMicTutorial = (getData().gameState.currentIndex == 1) &&
							   (getData().gameState.currentRankName() ==
								GameConstants::RankNames::getRankName(0));
	if (!isTutorial && !isMicTutorial)
	{
		m_noKigoBtn.update();
	}
	if (m_rhythmModeActive)
	{
		m_startStopBtn.update();
	}

	const bool disableInput = getData().showHowToPlay;
	if (disableInput)
	{
		return;
	}

	// リズムモード更新
	if (m_rhythmModeActive)
	{
		updateRhythmMode();
	}

	handleClick();
	// GameScene 内部で問題を進め、末尾まで到達したら結果へ
	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		changeScene(State::Result);
		return;
	}
}

void GameScene::draw() const
{
	Reseed(getData().gameState.currentIndex);

	getData().renderer.drawBackground();

	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		FontAsset(GameConstants::Fonts::KEY_GAME)(U"終了！").drawAt(
			Scene::Center(), Palette::Black);
		return;
	}

	if (m_rhythmModeActive)
	{
		// 画面分割モード：左半分に俳句、右半分にリズム
		drawRhythmContent();
		renderTexture.draw(Scene::Width() * 0.5, 0);
		drawGameContent();
		drawHintGauge();
	}
	else
	{
		// 従来の全画面表示
		drawGameContent();
	}
}

void GameScene::drawKigoRect() const
{
	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (prob.hasKigo)
	{
		const auto& ui = getData().configManager.ui();
		for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
		{
			drawWordRect(i, ui);
		}
	}
}

void GameScene::drawHiakuRect() const
{
	const auto& ui = getData().configManager.ui();
	for (int32 i = 0; i < m_chars.size(); ++i)
	{
		drawWordRect(i, ui);
	}
}

void GameScene::drawWordRect(s3d::int32 i, const UIConfig& ui) const
{
	const bool isSpace = (m_chars[i].ch == U' ' || m_chars[i].ch == U'　');
	if (isSpace)
	{
		return;
	}

	const RectF inflated =
		Inflate(m_chars[i].box, ui.hitboxPaddingPx, ui.hitboxPaddingScale);
	// inflated.draw(Palette::White);

	// 放射状グラデーション（中心が濃く、外側が薄い）
	Circle rectCenter{
		inflated.center(),
		(inflated.w + inflated.h) * GameConstants::UI::CIRCLE_RADIUS_RATIO};
	DrawRadialFadeCircle(rectCenter, ColorF(GeneratePastelColor(), 0.5),
						 GameConstants::UI::RADIAL_FADE_STEPS);
}

Vec2 GameScene::getKigoRectCenter() const
{
	Vec2 result{};
	int rectCount{};

	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (prob.hasKigo)
	{
		const auto& ui = getData().configManager.ui();
		for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
		{
			const bool isSpace =
				(m_chars[i].ch == U' ' || m_chars[i].ch == U'　');
			if (isSpace)
			{
				continue;
			}

			const RectF inflated = Inflate(m_chars[i].box, ui.hitboxPaddingPx,
										   ui.hitboxPaddingScale);
			result += inflated.center();
			++rectCount;
		}
	}

	return result / rectCount;
}

RectF GameScene::Inflate(const RectF& r, double padPx, double padScale)
{
	const double pad = padPx + (r.w * padScale);
	return RectF{r.x - pad, r.y - pad,
				 r.w + pad * GameConstants::UI::PADDING_MULTIPLIER,
				 r.h + pad * GameConstants::UI::PADDING_MULTIPLIER};
}

bool GameScene::isHitKigo() const
{
	const auto& prob =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (!prob.hasKigo)
	{
		return false;
	}

	const auto& ui = getData().configManager.ui();

	for (int32 i = prob.kigoStart; i < prob.kigoEnd; ++i)
	{
		const RectF inflated =
			Inflate(m_chars[i].box, ui.hitboxPaddingPx, ui.hitboxPaddingScale);
		if (inflated.mouseOver() && MouseL.down())
		{
			return true;
		}
	}
	return false;
}

void GameScene::handleClick()
{
	// チュートリアルに関しては特別扱い
	const bool isTutorial = (getData().gameState.currentIndex == 0) &&
							(getData().gameState.currentRankName() ==
							 GameConstants::RankNames::getRankName(0));
	const bool isMicTutorial = (getData().gameState.currentIndex == 1) &&
							   (getData().gameState.currentRankName() ==
								GameConstants::RankNames::getRankName(0));

	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		return;
	}

	auto& prob = getData().gameState.problems[getData().gameState.currentIndex];

	if (!MouseL.down())
	{
		return;
	}

	// 開始/停止ボタンのチェック
	if (m_rhythmModeActive && m_startStopBtn.roundRect().mouseOver())
	{
		if (m_beatTransport.isRunning())
		{
			m_beatTransport.pause();
		}
		else
		{
			m_beatTransport.start();
		}
		return;
	}

	// 季語ヒット
	if (isHitKigo())
	{
		getData().gameState.answered = true;
		m_result = prob.hasKigo;  // hasKigo=true のときだけここに来る設計
		if (m_result)
		{
			ExecCorrect();
		}
		else
		{
			ExecWrong();
		}
		return;
	}

	// 季語なしボタンの簡易チェック
	const bool skipNoKigoBtn = isTutorial || isMicTutorial;
	if (!skipNoKigoBtn && m_noKigoBtn.roundRect().mouseOver())
	{
		getData().gameState.answered = true;
		m_result = (!prob.hasKigo);
		if (m_result)
		{
			ExecCorrect();
		}
		else
		{
			ExecWrong();
		}
		return;
	}

	// 解説表示中にクリックで次の問題へ
	if (getData().gameState.answered && m_showExplanation)
	{
		getData().gameState.stateNext();
		startProblem();
	}
	else
	{
		// 何もないところをクリックすると間違い扱いにする
		ExecWrong();
	}
}

// リズム機能メソッド実装
void GameScene::initializeRhythmMode()
{
	const auto& rhythmConfig = getData().configManager.rhythm();

	// リズム機能が有効かチェック
	m_rhythmModeActive = isRhythmModeEnabled();

	if (!m_rhythmModeActive)
	{
		return;
	}

	// MoraRendererの初期化
	m_moraRenderer = std::make_unique<MoraRenderer>();
	if (!m_moraRenderer->initialize())
	{
		Print << U"Failed to initialize MoraRenderer";
		m_rhythmModeActive = false;
		return;
	}

	// VoiceReactiveFxの初期化
	m_voiceReactiveFx = std::make_unique<VoiceReactiveFx>();

	// VoiceDetectorの初期化
	m_voiceDetector = std::make_unique<VoiceActivityDetector>();
	if (!m_voiceDetector->initialize())
	{
		Print << U"Failed to initialize VoiceDetector - using dummy";
		m_voiceDetector = std::make_unique<DummyVoiceActivityDetector>();
		m_voiceDetector->initialize();
	}

	// パラメータ設定
	VoiceActivityDetectorParams params;
	params.alpha = rhythmConfig.micSensitivity;
	m_voiceDetector->setParams(params);
}

void GameScene::updateRhythmMode()
{
	if (!m_rhythmModeActive || !m_voiceDetector || !m_voiceReactiveFx)
	{
		return;
	}

	// 音声検出更新
	const bool voiceChanged = m_voiceDetector->update();
	(void)voiceChanged;	 // 未使用警告回避
	VoiceState voiceState = m_voiceDetector->state();

	// 音声リアクティブエフェクト更新
	m_voiceReactiveFx->update(voiceState, Scene::DeltaTime());

	// BeatTransport更新とBeatHitDetector処理
	if (m_beatTransport.isRunning())
	{
		double level = m_voiceReactiveFx->level();
		m_beatHitDetector.process(
			m_parsedStream, getCurrentBeat(),
			[level, this](size_t moraIndex)
			{
				(void)moraIndex;
				// ビートヒット時の処理（SE再生など）
				getData().sound.playRhythmBeat();

				// ヒントゲージが溜まっていくのを実装する
				m_hintGauge +=
					level * GameConstants::Game::HINT_GAUGE_VOICE_BONUS;
			});
	}

	// ヒントゲージが50%でイベント発生
	if ((m_hintGauge > GameConstants::Game::HINT_GAUGE_HINT_POINT) &&
		(m_hintCheckPoint == 0.0))
	{
		StartFlow(GameConstants::Game::HINT_GAUGE_HINT_POINT);
		m_hintCheckPoint = GameConstants::Game::HINT_GAUGE_HINT_POINT;
	}
	// ヒントゲージ100%以降は毎回発生
	if ((m_hintGauge > GameConstants::Game::HINT_GAUGE_MAN) &&
		(m_hintGauge > m_hintCheckPoint))
	{
		m_hintGauge = GameConstants::Game::HINT_GAUGE_MAN - 1e-6;
		StartFlow(GameConstants::Game::HINT_GAUGE_HINT_POINT);
		m_hintCheckPoint = m_hintGauge;
	}

	// 譜面が最終タイミングまで来たら元に戻して、時間を止めるようにする
	if (getCurrentBeat() > m_parsedStream.totalBeats)
	{
		m_beatTransport.reset();
		m_beatHitDetector.reset();
	}
}

void GameScene::drawRhythmMode() const
{
	// このメソッドは既存のdrawRhythmContent()と統合
	drawRhythmContent();
}

bool GameScene::isRhythmModeEnabled() const
{
	// 現在の問題にリズムデータがあるかチェック
	if (getData().gameState.currentIndex >= getData().gameState.problems.size())
	{
		return false;
	}

	const auto& problem =
		getData().gameState.problems[getData().gameState.currentIndex];
	return !problem.rhythm.isEmpty();
}

double GameScene::getCurrentBeat() const
{
	return m_beatTransport.nowBeat();
}

void GameScene::drawGameContent() const
{
	// 画面分割時の左半分に俳句を描画
	const RectF gameArea =
		m_rhythmModeActive ? RectF{0, 0, Scene::Width() * 0.5, Scene::Height()}
						   : RectF{0, 0, Scene::Width(), Scene::Height()};
	(void)gameArea;	 // 未使用警告回避

	// チュートリアルに関しては特別扱い
	const bool isTutorial = (getData().gameState.currentIndex == 0) &&
							(getData().gameState.currentRankName() ==
							 GameConstants::RankNames::getRankName(0));
	const bool isMicTutorial = (getData().gameState.currentIndex == 1) &&
							(getData().gameState.currentRankName() ==
							 GameConstants::RankNames::getRankName(0));

	if (m_showExplanation || isTutorial)
	{
		// 季語をハイライトする
		drawKigoRect();
	}
	else
	{
		// 俳句の背景に色を付ける
		drawHiakuRect();
	}

	// ミスクリック時に方向を示す
	if (m_flowTime > 0.0f)
	{
		DrawFlowHintPastel(
			RectF(getData().configManager.ui().clientSizeX,
				  getData().configManager.ui().clientSizeY),
			(m_flowStartPos - getKigoRectCenter()), m_flowTime,
			static_cast<size_t>(GameConstants::UI::FLOW_HINT_PARTICLE_COUNT * m_flowPower));
	}

	// 俳句本文（フリガナ対応）
	const auto& problem =
		getData().gameState.problems[getData().gameState.currentIndex];
	if (problem.displayRuby && !problem.ruby.isEmpty())
	{
		getData().renderer.drawHaikuWithRuby(m_chars);
	}
	else
	{
		getData().renderer.drawHaiku(m_chars);
	}

	if (!isTutorial && !isMicTutorial)
	{
		// 季語なしボタン（簡易）
		m_noKigoBtn.draw();
	}
	if (m_rhythmModeActive)
	{
		m_startStopBtn.draw();
	}

	// 先生リアクション
	if (!getData().gameState.answered)
	{
		getData().renderer.drawTeacherNormal();
	}
	else
	{
		if (m_result)
		{
			getData().renderer.drawTeacherHappy();
		}
		else
		{
			getData().renderer.drawTeacherAngry();
		}
	}

	// 解説
	if (m_showExplanation)
	{
		getData().renderer.drawExplanation(
			getData().gameState.problems[getData().gameState.currentIndex].kigo,
			getData()
				.gameState.problems[getData().gameState.currentIndex]
				.getSeason(),
			getData()
				.gameState.problems[getData().gameState.currentIndex]
				.explanation);
	}
	else
	{
		// チュートリアルテキスト
		if (isTutorial)
		{
			getData().renderer.drawTutorial(
				U"俳句の中の季語を見つけていく（クリックする）ゲームです\n"
				U"言葉の芯を楽しんでください！！！");
		}
		else if (isMicTutorial)
		{
			getData().renderer.drawTutorial(
				U"リズムにシンクロして、俳句を読み上げると、ヒントポイントが手に入る！\n"
					  "ヒントポイントが一定以上溜まると、、、");
		}
	}

	if (!isTutorial && !isMicTutorial)
	{
		// 現在スコアの表示
		FontAsset(GameConstants::Fonts::KEY_EXPLANATION)(
			U"スコア : {}"_fmt(getData().gameState.score))
			.draw(Vec2(50.0, 20.0),
				  Palette::Black)
			.scaled(1.2)
			.stretched(8)
			.drawFrame(3.0, Palette::Black);
	}

}

void GameScene::drawRhythmContent() const
{
	if (!m_rhythmModeActive || !m_moraRenderer || !m_voiceReactiveFx)
	{
		return;
	}

	const ScopedRenderTarget2D target{renderTexture};
	const ScopedRenderStates2D blend{};
	// 背景クリア
	renderTexture.clear(Palette::White);

	// 画面右半分にリズム表示
	const RectF rhythmArea{0, 0, Scene::Width() * 0.5, Scene::Height()};

	// 背景
	rhythmArea.draw(ColorF{0.95, 0.95, 1.0, 0.3});

	// リズム描画レイアウト設定
	RhythmLayoutSettings layout;
	layout.hitX = rhythmArea.x + rhythmArea.w * 0.3;  // 右側エリアの30%位置

	// 現在の拍取得
	const double currentBeat = getCurrentBeat();

	// 音声レベル取得
	const double voiceLevel = m_voiceReactiveFx->level();

	// リズム描画
	m_moraRenderer->drawGuide(layout, currentBeat);
	m_moraRenderer->drawMoras(m_parsedStream, layout, currentBeat, voiceLevel);
	m_moraRenderer->drawCuts(m_parsedStream, layout, currentBeat);

	// デバッグ情報
	if (GameConstants::Debug::IS_ENABLE && m_voiceDetector)
	{
		const VoiceState voiceState = m_voiceDetector->state();
		(void)voiceState;  // 未使用警告回避
		const Vec2 debugPos{rhythmArea.x + 20, rhythmArea.y + 20};
		FontAsset(GameConstants::Fonts::KEY_GAME)(
			U"Voice: {:.2f}"_fmt(voiceLevel))
			.draw(debugPos, Palette::Black);
		FontAsset(GameConstants::Fonts::KEY_GAME)(
			U"Beat: {:.1f}"_fmt(currentBeat))
			.draw(debugPos + Vec2{0, 30}, Palette::Black);
	}
}

void GameScene::drawHintGauge() const
{
	FontAsset(GameConstants::Fonts::KEY_EXPLANATION)(U"ヒント")
		.draw(GameConstants::UI::HINT_GAUGE_RECT.pos - Vec2(100.0, 20.0),
			  Palette::Black);
	GameConstants::UI::HINT_GAUGE_RECT.draw(Palette::White).draw(Palette::Lightgray)
		.drawFrame(2.0, Palette::Gray);
	// 現在容量
	RectF HintBar(
		GameConstants::UI::HINT_GAUGE_RECT.pos +
			Vec2(GameConstants::UI::HINT_GAUGE_RECT.w * (1.0 - m_hintGauge), 0),
		Vec2(GameConstants::UI::HINT_GAUGE_RECT.w * m_hintGauge,
			 GameConstants::UI::HINT_GAUGE_RECT.h));
	HintBar.draw(Palette::Yellow);
	// 半分のところに線を入れる
	RectF HintLine(GameConstants::UI::HINT_GAUGE_RECT.pos +
					   Vec2(GameConstants::UI::HINT_GAUGE_RECT.w / 2.0, 0),
				   Vec2(2.0, GameConstants::UI::HINT_GAUGE_RECT.h));
	HintLine.draw(Palette::Yellowgreen);
}

void GameScene::ExecWrong()
{
	getData().sound.playWrong();

	// 間違ったらヒントゲージを加算してスコアを削るようにする
	m_hintGauge += GameConstants::Game::HINT_GAUGE_FAILED_BONUS;

	if (!m_showExplanation)
	{
		getData().gameState.score = Max(
			0, getData().gameState.score - GameConstants::SCORE_FIALED_BASE);
	}
}

void GameScene::StartFlow(double flowPower)
{
	m_flowTime = GameConstants::UI::FLOW_TIME_RESET * flowPower;
	m_flowStartPos = Cursor::Pos();
	m_flowPower = flowPower;
}

void GameScene::ExecCorrect()
{
	getData().sound.playCorrect();
	// 連打したときに音は鳴らすけど、正解数カウントは上げないようにする
	if (!m_showExplanation)
	{
		getData().gameState.score +=
			GameConstants::SCORE_MULTIPLIER_BASE *
			(getData()
				 .gameState.problems[getData().gameState.currentIndex]
				 .grade +
			 1);
		getData().gameState.correctCount += 1;
		m_showExplanation = true;
	}
}
