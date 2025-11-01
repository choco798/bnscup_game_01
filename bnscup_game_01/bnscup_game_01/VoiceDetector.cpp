#include "VoiceDetector.hpp"
#include "GameConstants.hpp"

// ---- VoiceActivityDetector 実装 ----
VoiceActivityDetector::VoiceActivityDetector(const Params& params,
											 const DebugLog& dbg)
	: m_params(params), m_dbgOpt(dbg)
{
}

VoiceActivityDetector::~VoiceActivityDetector()
{
	finalize();
}

bool VoiceActivityDetector::initialize(const Optional<uint32>& deviceIndex)
{
	if (m_initialized)
	{
		return true;
	}

	// マイク初期化（例外なし）
	m_mic = Microphone{deviceIndex};
	if (not m_mic)
	{
		Print << GameConstants::ErrorMessages::MICROPHONE_INIT_FAILED;
		return false;
	}

	m_mic.start();

	// ログ初期化
	if (m_dbgOpt.enabled)
	{
		if (!initializeLog())
		{
			Print << GameConstants::ErrorMessages::VAD_LOG_FILE_FAILED;
			// ログ失敗は致命的でないので続行
		}
	}

	m_initialized = true;
	return true;
}

bool VoiceActivityDetector::initializeLog()
{
	const OpenMode mode =
		(m_dbgOpt.append ? OpenMode::Append : OpenMode::Trunc);
	if (!m_writer.open(m_dbgOpt.logPath, mode))
	{
		return false;
	}

	if (!m_dbgOpt.append || m_dbgOpt.writeHeader)
	{
		// CSVヘッダ
		m_writer.writeln(
			U"time_ms,energy,noiseEMA,onThresh,offThresh,rawVoice,"
			U"confirmedVoice,i0,i1,hzPerBin,fps");
	}

	return true;
}

void VoiceActivityDetector::finalize()
{
	if (m_writer.isOpen())
	{
		m_writer.close();
	}
	m_initialized = false;
}

bool VoiceActivityDetector::update()
{
	if (!m_initialized)
	{
		return false;
	}

	// スペクトル取得
	m_mic.fft(m_fft);

	// 帯域エネルギー
	const double hzPerBin = m_fft.resolution;  // 1bin あたりの周波数幅
	const int32 i0 = Max(0, static_cast<int32>(m_params.bandLowHz / hzPerBin));
	const int32 i1 =
		Min<int32>(static_cast<int32>(m_fft.buffer.size()) - 1,
				   static_cast<int32>(m_params.bandHighHz / hzPerBin));

	double bandEnergy = 0.0;
	for (int i = i0; i <= i1; ++i)
	{
		bandEnergy += m_fft.buffer[i];
	}
	m_lastEnergy = bandEnergy;

	// ノイズEMA更新
	m_noiseEMA = Math::Lerp(m_noiseEMA, bandEnergy, m_params.alpha);

	// 閾値
	const double onThresh = (m_noiseEMA * m_params.kOn) + m_params.absOn;
	const double offThresh = (m_noiseEMA * m_params.kOff) + m_params.absOff;

	// 「生状態」（瞬間判定。まだ確定しない）
	const bool rawVoice =
		(bandEnergy > onThresh) ||
		(m_rawVoicePrev && (bandEnergy > offThresh));  // ヒステリシス
	if (rawVoice != m_rawVoicePrev)
	{
		// 状態遷移が起きたら、継続時間計測をリセット
		m_stageTimer.restart();
		m_rawVoicePrev = rawVoice;
	}

	// 最小継続時間で確定
	bool changed = false;
	if (m_confirmedVoice != rawVoice)
	{
		const int32 needMs = rawVoice ? m_params.minOnMs : m_params.minOffMs;
		if (m_stageTimer.ms() >= needMs)
		{
			m_confirmedVoice = rawVoice;
			changed = true;
		}
	}

	// ログ書き出し
	writeLog(rawVoice, onThresh, offThresh, i0, i1, hzPerBin);

	return changed;
}

VoiceState VoiceActivityDetector::state() const
{
	VoiceState result{};
	result.active = m_confirmedVoice;
	result.energy = m_lastEnergy;
	result.noise = m_noiseEMA;
	result.onTh = onThreshold();
	result.offTh = offThreshold();
	return result;
}

void VoiceActivityDetector::setParams(const VoiceActivityDetectorParams& params)
{
	m_params = params;
}

void VoiceActivityDetector::drawUI() const
{
	if (!m_initialized) return;

	using namespace GameConstants::Rhythm;

	// 簡易UI
	const double e = energy();
	const double n = noiseEMA();

	RectF{40, 580, 600, 24}.draw(Palette::White);
	RectF{40, 580, 600 * Saturate(e / (n * 5.0 + 0.1)), 24}.draw(
		isVoice() ? ColorF{0.9, 0.6, 0.2} : ColorF(Palette::Gray));

	FontAsset(KEY_UI_SMALL)(U"energy:{:.4f}  noise:{:.4f}  state:{}"_fmt(
								e, n, (isVoice() ? U"VOICE" : U"SILENCE")))
		.draw(40, 540, Palette::Black);

	// しきい値目安線
	const double thOn = onThreshold();
	const double thOff = offThreshold();
	const double base = (n * 5.0 + 0.1);
	const double xOn = 40 + 600 * Saturate(thOn / base);
	const double xOff = 40 + 600 * Saturate(thOff / base);
	Line{xOn, 580, xOn, 604}.draw(2, Palette::Orange);
	Line{xOff, 580, xOff, 604}.draw(2, Palette::Skyblue);
}

bool VoiceActivityDetector::isVoice() const
{
	return m_confirmedVoice;
}

double VoiceActivityDetector::energy() const
{
	return m_lastEnergy;
}

double VoiceActivityDetector::noiseEMA() const
{
	return m_noiseEMA;
}

double VoiceActivityDetector::onThreshold() const
{
	return (m_noiseEMA * m_params.kOn) + m_params.absOn;
}

double VoiceActivityDetector::offThreshold() const
{
	return (m_noiseEMA * m_params.kOff) + m_params.absOff;
}

const VoiceActivityDetector::Params& VoiceActivityDetector::params() const
{
	return m_params;
}

void VoiceActivityDetector::writeLog(bool rawVoice, double onTh, double offTh,
									 int32 i0, int32 i1, double hzPerBin)
{
	if (!m_dbgOpt.enabled || !m_writer.isOpen()) return;

	// できるだけ解析しやすいように ms 単位、真偽は 0/1 で
	const int64 tms = Time::GetMillisec();	// 起動からのms
	const double fps = Profiler::FPS();

	// CSV 1行
	m_writer.writeln(Format(tms, U",", m_lastEnergy, U",", m_noiseEMA, U",",
							onTh, U",", offTh, U",", (rawVoice ? 1 : 0), U",",
							(m_confirmedVoice ? 1 : 0), U",", i0, U",", i1,
							U",", hzPerBin, U",", fps));

	++m_logFrameCount;
}

// ---- DummyVoiceActivityDetector 実装 ----
bool DummyVoiceActivityDetector::initialize(const Optional<uint32>& deviceIndex)
{
	(void)deviceIndex;
	// ダミーは常に成功
	return true;
}

void DummyVoiceActivityDetector::finalize()
{
	// 何もしない
}

bool DummyVoiceActivityDetector::update()
{
	// ダミーは状態変化なし
	return false;
}

VoiceState DummyVoiceActivityDetector::state() const
{
	return m_state;
}

void DummyVoiceActivityDetector::setParams(
	const VoiceActivityDetectorParams& params)
{
	m_params = params;
}
