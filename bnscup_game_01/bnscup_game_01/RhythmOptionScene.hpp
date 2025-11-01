#pragma once

#include "SceneBase.hpp"
#include "UiButton.hpp"
#include "VoiceDetector.hpp"

// ---- リズムオプションシーン ----
class RhythmOptionScene : public KigoGameApp::Scene
{
   public:
	RhythmOptionScene(const InitData& init);
	~RhythmOptionScene();

	void update() override;
	void draw() const override;

   private:
	void initializeVoiceDetector();
	void updateMicSensitivity();
	void updateVADParameters();
	void drawMicrophoneSettings() const;
	void drawVADSettings() const;
	void drawMicrophoneVisualization() const;
	void drawBackButton() const;

   private:
	// UI関連
	ui::Button m_backButton;

	// マイク関連
	std::unique_ptr<IVoiceActivityDetector> m_voiceDetector;
	bool m_micInitialized = false;

	// UI制御用
	bool m_micSensitivityDragging = false;
	bool m_vadAlphaDragging = false;
	bool m_vadKOnDragging = false;
	bool m_vadKOffDragging = false;

	// スライダー領域
	RectF m_micSensitivitySlider;
	RectF m_vadAlphaSlider;
	RectF m_vadKOnSlider;
	RectF m_vadKOffSlider;

	// 表示用
	mutable String m_statusMessage;
};