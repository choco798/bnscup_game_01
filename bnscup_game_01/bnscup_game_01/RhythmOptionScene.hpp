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
	void updateSliders();
	void updateVADParameters();
	void drawSliders() const;
	void drawMicrophoneVisualization() const;

   private:
	// UI関連
	ui::Button m_backButton;

	// マイク関連
	std::unique_ptr<IVoiceActivityDetector> m_voiceDetector;
	bool m_micInitialized = false;

	// スライダー
	ui::Slider m_micSensitivitySlider;
	ui::Slider m_vadAlphaSlider;
	ui::Slider m_vadKOnSlider;
	ui::Slider m_vadKOffSlider;

	// 表示用
	mutable String m_statusMessage;
};
