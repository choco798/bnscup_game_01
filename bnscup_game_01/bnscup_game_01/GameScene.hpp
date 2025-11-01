#pragma once

#include "Config.hpp"
#include "MoraSystem.hpp"
#include "RhythmRenderer.hpp"
#include "SceneBase.hpp"
#include "UiButton.hpp"
#include "VoiceDetector.hpp"

class GameScene : public KigoGameApp::Scene
{
   private:
	Array<LayoutChar> m_chars;
	bool m_showExplanation = false;
	bool m_result = false;
	double m_hintGauge = 0.0; // 0...1
	double m_hintCheckPoint = 0.0; // 0...1
	double m_flowTime = 0.0f;
	Vec2 m_flowStartPos{};
	double m_flowPower = 0.0f;
	ui::Button m_noKigoBtn;
	ui::Button m_startStopBtn;
	RenderTexture renderTexture{1280, 720, ColorF(0.0, 0.0)};

	// リズム機能関連
	std::unique_ptr<MoraRenderer> m_moraRenderer;
	std::unique_ptr<IVoiceActivityDetector> m_voiceDetector;
	std::unique_ptr<VoiceReactiveFx> m_voiceReactiveFx;
	BeatTransport m_beatTransport{120.0};  // 120 BPM
	BeatHitDetector m_beatHitDetector;
	ParsedStream m_parsedStream;
	bool m_rhythmModeActive = false;

	void handleClick();
	void ExecWrong();
	void StartFlow(double flowPower);
	void ExecCorrect();
	bool isHitKigo() const;

	// リズム機能メソッド
	void initializeRhythmMode();
	void updateRhythmMode();
	void drawRhythmMode() const;
	bool isRhythmModeEnabled() const;
	double getCurrentBeat() const;

   public:
	GameScene(const InitData& init);
	void startProblem();
	void update() override;
	void draw() const override;
	void drawKigoRect() const;
	void drawWordRect(s3d::int32 i, const UIConfig& ui) const;
	void drawHiakuRect() const;
	void drawGameContent() const;
	void drawRhythmContent() const;
	void drawHintGauge() const;
	Vec2 getKigoRectCenter() const;
	static RectF Inflate(const RectF& r, double padPx, double padScale);
};
