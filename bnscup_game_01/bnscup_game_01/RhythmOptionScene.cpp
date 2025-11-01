#include "RhythmOptionScene.hpp"
#include "GameConstants.hpp"

using namespace GameConstants::Rhythm::OptionUI;

RhythmOptionScene::RhythmOptionScene(const InitData& init)
	: IScene(init),
	  m_backButton(U"戻る", GameConstants::Fonts::KEY_MENU, BACK_BUTTON_POS)
{
	// スライダーの初期化
	m_micSensitivitySlider.setTrackRect(RectF{MIC_SENSITIVITY_POS, SLIDER_SIZE})
		.setRange(0.0, 2.0)
		.setValue(getData().configManager.rhythm().micSensitivity);

	m_vadAlphaSlider.setTrackRect(RectF{VAD_ALPHA_POS, SLIDER_SIZE})
		.setRange(VAD_ALPHA_MIN, VAD_ALPHA_MAX)
		.setValue(getData().configManager.rhythm().vadAlpha);

	m_vadKOnSlider.setTrackRect(RectF{VAD_K_ON_POS, SLIDER_SIZE})
		.setRange(VAD_K_ON_MIN, VAD_K_ON_MAX)
		.setValue(getData().configManager.rhythm().vadKOn);

	m_vadKOffSlider.setTrackRect(RectF{VAD_K_OFF_POS, SLIDER_SIZE})
		.setRange(VAD_K_OFF_MIN, VAD_K_OFF_MAX)
		.setValue(getData().configManager.rhythm().vadKOff);

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
	if (m_backButton.isClicked() || KeyEscape.down())
	{
		changeScene(State::Title);
		return;
	}

	// スライダー更新
	updateSliders();

	// VADパラメータ更新
	updateVADParameters();

	// 音声検出更新
	if (m_voiceDetector)
	{
		m_voiceDetector->update();
	}
}

void RhythmOptionScene::updateSliders()
{
	auto& rhythm = getData().configManager.rhythm();

	if (m_micSensitivitySlider.update())
	{
		rhythm.micSensitivity = m_micSensitivitySlider.getValue();

		const double param_ratio = 1.0 - rhythm.micSensitivity / 2.0;
		// 感度をまとめて調整できる便利パラメータ
		rhythm.vadAlpha = 0.01 + param_ratio * 0.1;	 // 0.01-0.11 範囲
		rhythm.vadKOn = 0.5 + param_ratio * 2.0;	 // 1.0-2.0 範囲
		rhythm.vadKOff = 0.3 + param_ratio * 1.0;	 // 0.3-1.3 範囲

		// スライダーも更新
		m_vadAlphaSlider.setValue(rhythm.vadAlpha);
		m_vadKOnSlider.setValue(rhythm.vadKOn);
		m_vadKOffSlider.setValue(rhythm.vadKOff);
	}

	if (m_vadAlphaSlider.update())
	{
		rhythm.vadAlpha = m_vadAlphaSlider.getValue();
	}

	if (m_vadKOnSlider.update())
	{
		rhythm.vadKOn = m_vadKOnSlider.getValue();
	}

	if (m_vadKOffSlider.update())
	{
		rhythm.vadKOff = m_vadKOffSlider.getValue();
	}
}

void RhythmOptionScene::updateVADParameters()
{
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

	// スライダー描画
	drawSliders();

	// マイク可視化
	if (m_micInitialized)
	{
		drawMicrophoneVisualization();
	}

	// 戻るボタン
	m_backButton.draw();
}

void RhythmOptionScene::drawSliders() const
{
	using namespace GameConstants::Fonts;
	const auto& rhythm = getData().configManager.rhythm();

	// マイク感度
	FontAsset(KEY_EXPLANATION)(U"マイク感度: {:.2f}"_fmt(rhythm.micSensitivity))
		.draw(50, MIC_SENSITIVITY_POS.y - 60, Palette::Black);
	m_micSensitivitySlider.draw();

	// VAD Alpha
	FontAsset(KEY_EXPLANATION)(U"ノイズ追従度: {:.3f}"_fmt(rhythm.vadAlpha))
		.draw(50, VAD_ALPHA_POS.y - 60, Palette::Black);
	m_vadAlphaSlider.draw();

	// VAD K_ON
	FontAsset(KEY_EXPLANATION)(U"ON閾値倍率: {:.2f}"_fmt(rhythm.vadKOn))
		.draw(50, VAD_K_ON_POS.y - 60, Palette::Black);
	m_vadKOnSlider.draw();

	// VAD K_OFF
	FontAsset(KEY_EXPLANATION)(U"OFF閾値倍率: {:.2f}"_fmt(rhythm.vadKOff))
		.draw(50, VAD_K_OFF_POS.y - 60, Palette::Black);
	m_vadKOffSlider.draw();
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
