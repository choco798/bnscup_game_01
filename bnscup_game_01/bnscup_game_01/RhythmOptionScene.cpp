#include "RhythmOptionScene.hpp"
#include "GameConstants.hpp"

RhythmOptionScene::RhythmOptionScene(const InitData& init)
	: IScene(init),
	  m_backButton(U"戻る", GameConstants::Fonts::KEY_MENU, Vec2{800, 600})
{
	// UI配置設定
	m_micSensitivitySlider = RectF{200, 240, 400, 20};
	m_vadAlphaSlider = RectF{200, 320, 400, 20};
	m_vadKOnSlider = RectF{200, 400, 400, 20};
	m_vadKOffSlider = RectF{200, 480, 400, 20};

	// マイク初期化試行
	initializeVoiceDetector();
}

RhythmOptionScene::~RhythmOptionScene()
{
	if (m_voiceDetector)
	{
		m_voiceDetector->finalize();
	}
}

void RhythmOptionScene::initializeVoiceDetector()
{
	// VoiceActivityDetectorを作成・初期化
	auto detector = std::make_unique<VoiceActivityDetector>();
	if (detector->initialize())
	{
		m_voiceDetector = std::move(detector);
		m_micInitialized = true;
		m_statusMessage = U"マイクが正常に初期化されました";
	}
	else
	{
		// フォールバック：ダミー実装
		m_voiceDetector = std::make_unique<DummyVoiceActivityDetector>();
		m_voiceDetector->initialize();
		m_micInitialized = false;
		m_statusMessage = U"マイクの初期化に失敗しました（ダミーモード）";
	}

	// 初期パラメータ設定
	updateVADParameters();
}

void RhythmOptionScene::update()
{
	m_backButton.update();

	// 戻るボタン
	if (m_backButton.isClicked())
	{
		changeScene(State::Title);
		return;
	}

	// マイク感度調整
	updateMicSensitivity();

	// VADパラメータ更新
	updateVADParameters();

	// 音声検出更新
	if (m_voiceDetector)
	{
		m_voiceDetector->update();
	}
}

void RhythmOptionScene::updateMicSensitivity()
{
	const auto& cursor = Cursor::Pos();

	// マイク感度スライダー
	if (m_micSensitivitySlider.leftClicked())
	{
		m_micSensitivityDragging = true;
	}

	if (m_micSensitivityDragging)
	{
		if (MouseL.up())
		{
			m_micSensitivityDragging = false;
		}
		else
		{
			const double ratio =
				Saturate((cursor.x - m_micSensitivitySlider.x) /
						 m_micSensitivitySlider.w);
			getData().configManager.rhythm().micSensitivity = ratio * 2.0;

			const double param_ratio = 1.0 - ratio;
			// 感度はまとめて調整できる便利パラメータ
			getData().configManager.rhythm().vadAlpha =
				0.01 + param_ratio * 0.1;  // 0.01-0.11 範囲
			getData().configManager.rhythm().vadKOn =
				1.0 + param_ratio * 1.0;  // 1.0-2.0 範囲
			getData().configManager.rhythm().vadKOff =
				0.3 + param_ratio * 1.0;  // 0.3-1.3 範囲
		}
	}
}

void RhythmOptionScene::updateVADParameters()
{
	const auto& cursor = Cursor::Pos();

	// VAD Alpha スライダー
	if (m_vadAlphaSlider.leftClicked())
	{
		m_vadAlphaDragging = true;
	}
	if (m_vadAlphaDragging)
	{
		if (MouseL.up())
		{
			m_vadAlphaDragging = false;
		}
		else
		{
			const double ratio =
				Saturate((cursor.x - m_vadAlphaSlider.x) / m_vadAlphaSlider.w);
			getData().configManager.rhythm().vadAlpha =
				0.01 + ratio * 0.1;	 // 0.01-0.11 範囲
		}
	}

	// VAD K_ON スライダー
	if (m_vadKOnSlider.leftClicked())
	{
		m_vadKOnDragging = true;
	}
	if (m_vadKOnDragging)
	{
		if (MouseL.up())
		{
			m_vadKOnDragging = false;
		}
		else
		{
			const double ratio =
				Saturate((cursor.x - m_vadKOnSlider.x) / m_vadKOnSlider.w);
			getData().configManager.rhythm().vadKOn =
				1.0 + ratio * 2.0;	// 1.0-3.0 範囲
		}
	}

	// VAD K_OFF スライダー
	if (m_vadKOffSlider.leftClicked())
	{
		m_vadKOffDragging = true;
	}
	if (m_vadKOffDragging)
	{
		if (MouseL.up())
		{
			m_vadKOffDragging = false;
		}
		else
		{
			const double ratio =
				Saturate((cursor.x - m_vadKOffSlider.x) / m_vadKOffSlider.w);
			getData().configManager.rhythm().vadKOff =
				0.3 + ratio * 1.0;	// 0.3-1.3 範囲
		}
	}

	// パラメータをVADに反映
	if (m_voiceDetector)
	{
		const auto& rhythm = getData().configManager.rhythm();
		VoiceActivityDetectorParams params;
		params.alpha = rhythm.vadAlpha;
		params.kOn = rhythm.vadKOn;
		params.kOff = rhythm.vadKOff;
		params.absOn = rhythm.vadAbsOn;
		params.absOff = rhythm.vadAbsOff;
		params.bandLowHz = rhythm.vadBandLowHz;
		params.bandHighHz = rhythm.vadBandHighHz;
		params.minOnMs = rhythm.vadMinOnMs;
		params.minOffMs = rhythm.vadMinOffMs;

		m_voiceDetector->setParams(params);
	}
}

void RhythmOptionScene::draw() const
{
	// 背景
	Scene::SetBackground(GameConstants::UI::BACKGROUND_COLOR);

	// タイトル
	FontAsset(GameConstants::Fonts::KEY_TITLE)(U"マイク設定")
		.drawAt(Scene::Center().x, 80, Palette::Black);

	// ステータスメッセージ
	FontAsset(GameConstants::Fonts::KEY_EXPLANATION)(m_statusMessage)
		.draw(50, 120, m_micInitialized ? Palette::Green : Palette::Red);

	// マイク設定
	drawMicrophoneSettings();

	// VAD設定
	drawVADSettings();

	// マイク可視化
	if (m_micInitialized)
	{
		drawMicrophoneVisualization();
	}

	// 戻るボタン
	drawBackButton();
}

void RhythmOptionScene::drawMicrophoneSettings() const
{
	using namespace GameConstants::Fonts;

	// マイク感度
	const auto& rhythm = getData().configManager.rhythm();
	FontAsset(KEY_EXPLANATION)(U"マイク感度: {:.2f}"_fmt(rhythm.micSensitivity))
		.draw(50, 180, Palette::Black);

	// スライダー背景
	m_micSensitivitySlider.draw(m_micSensitivityDragging
									? Palette::Gray : Palette::Lightgray);

	// スライダーハンドル
	const double handleX = m_micSensitivitySlider.x +
						   rhythm.micSensitivity / 2.0 * m_micSensitivitySlider.w;
	Circle{handleX, m_micSensitivitySlider.center().y, 12}.draw(
		m_micSensitivityDragging ? Palette::Orange : Palette::Gray);
}

void RhythmOptionScene::drawVADSettings() const
{
	using namespace GameConstants::Fonts;
	const auto& rhythm = getData().configManager.rhythm();

	// VAD Alpha
	FontAsset(KEY_EXPLANATION)(U"ノイズ追従度: {:.3f}"_fmt(rhythm.vadAlpha))
		.draw(50, 260, Palette::Black);

	m_vadAlphaSlider.draw(m_vadAlphaDragging ? Palette::Gray
											 : Palette::Lightgray);
	const double alphaRatio = (rhythm.vadAlpha - 0.01) / 0.1;
	const double alphaHandleX =
		m_vadAlphaSlider.x + alphaRatio * m_vadAlphaSlider.w;
	Circle{alphaHandleX, m_vadAlphaSlider.center().y, 12}.draw(
		m_vadAlphaDragging ? Palette::Orange : Palette::Gray);

	// VAD K_ON
	FontAsset(KEY_EXPLANATION)(U"ON閾値倍率: {:.2f}"_fmt(rhythm.vadKOn))
		.draw(50, 340, Palette::Black);

	m_vadKOnSlider.draw(m_vadKOnDragging ? Palette::Gray : Palette::Lightgray);
	const double onRatio = (rhythm.vadKOn - 1.0) / 1.0;
	const double onHandleX = m_vadKOnSlider.x + onRatio * m_vadKOnSlider.w;
	Circle{onHandleX, m_vadKOnSlider.center().y, 12}.draw(
		m_vadKOnDragging ? Palette::Orange : Palette::Gray);

	// VAD K_OFF
	FontAsset(KEY_EXPLANATION)(U"OFF閾値倍率: {:.2f}"_fmt(rhythm.vadKOff))
		.draw(50, 420, Palette::Black);

	m_vadKOffSlider.draw(m_vadKOffDragging ? Palette::Gray
										   : Palette::Lightgray);
	const double offRatio = (rhythm.vadKOff - 0.3) / 1.0;
	const double offHandleX = m_vadKOffSlider.x + offRatio * m_vadKOffSlider.w;
	Circle{offHandleX, m_vadKOffSlider.center().y, 12}.draw(
		m_vadKOffDragging ? Palette::Orange : Palette::Gray);
}

void RhythmOptionScene::drawMicrophoneVisualization() const
{
	if (!m_voiceDetector)
	{
		return;
	}

	// VoiceActivityDetectorのUIを描画
	if (auto* detector =
			dynamic_cast<VoiceActivityDetector*>(m_voiceDetector.get()))
	{
		detector->drawUI();
	}
}

void RhythmOptionScene::drawBackButton() const
{
	m_backButton.draw();
}
