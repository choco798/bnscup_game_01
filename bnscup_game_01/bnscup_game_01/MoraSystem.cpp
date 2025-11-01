#include "MoraSystem.hpp"
#include <cmath>

// ---- ユーティリティ関数実装 ----
namespace MoraUtils
{
double Frac(double x)
{
	return x - std::floor(x);
}

bool IsYouonSmall(char32 ch)
{
	return (ch == U'ゃ' || ch == U'ゅ' || ch == U'ょ' || ch == U'ャ' ||
			ch == U'ュ' || ch == U'ョ');
}
}  // namespace MoraUtils

// ---- KanaStreamParser実装 ----
bool KanaStreamParser::IsYouonSmall(char32 ch)
{
	return MoraUtils::IsYouonSmall(ch);
}

ParsedStream KanaStreamParser::Parse(const String& stream,
									 const RestPreset& rest)
{
	ParsedStream out;
	Array<Mora>& moras = out.moras;
	Array<Cut>& cuts = out.cuts;

	// 1) 記号の意味
	//   - 文字: モーラを追加
	//   - '=' : 直前モーラと次モーラを連結（直前の beamNext = true）
	//   - '~' : 直前モーラにフェルマータ（余韻）を付与
	//   - '|' : 句切れ（次が '|' なら強）→ 直前モーラの直後に休止を追加
	//   - ' ' : 無視（区切り）
	for (size_t i = 0; i < stream.size(); ++i)
	{
		const char32 ch = stream[i];

		if (ch == U' ') continue;

		if (ch == U'=')
		{
			if (!moras.empty()) moras.back().beamNext = true;
			continue;
		}

		if (ch == U'~')
		{
			if (!moras.empty()) moras.back().fermata = true;
			continue;
		}

		if (ch == U'|')
		{
			if (!moras.empty())
			{
				bool strong = false;
				if ((i + 1 < stream.size()) && stream[i + 1] == U'|')
				{
					strong = true;
					++i;  // '||' を消費
				}
				cuts << Cut{moras.size() - 1,
							(strong ? rest.strongCutBeats : rest.lightCutBeats),
							strong};
			}
			continue;
		}

		// ★ 拗音（小書き ゃ/ゅ/ょ）は直前モーラに結合して1モーラ化
		if (IsYouonSmall(ch) && !moras.empty())
		{
			moras.back().text.push_back(ch);
			continue;  // ← 新しいモーラは作らない
		}

		// それ以外は1モーラとして扱う（ひらがな/カタカナ/記号も表示可能）
		Mora m;
		m.text = String(1, ch);
		moras << m;
	}

	// 2) startBeat と totalBeats を割り当て（1モーラ=1拍、休止は後置）
	double t = 0.0;
	for (size_t i = 0; i < moras.size(); ++i)
	{
		moras[i].startBeat = t;
		t += 1.0;  // 1モーラ=1拍

		// フェルマータ（直後の無音）
		if (moras[i].fermata)
		{
			t += rest.fermataBeats;
		}

		// 句切れ（該当するものを加算）
		for (const auto& c : cuts)
		{
			if (c.afterIndex == i)
			{
				t += c.beats;
			}
		}
	}

	// 末尾のゆるやかな余韻（全体停止の見た目）
	t += rest.tailEndBeats;

	out.totalBeats = t;
	return out;
}

// ---- BeatTransport実装 ----
BeatTransport::BeatTransport(double bpm) : m_bpm(bpm)
{
}

void BeatTransport::start()
{
	m_sw.start();
}

void BeatTransport::pause()
{
	m_sw.pause();
}

void BeatTransport::resume()
{
	m_sw.resume();
}

void BeatTransport::reset()
{
	m_sw.reset();
}

bool BeatTransport::isRunning() const
{
	return m_sw.isRunning();
}

double BeatTransport::nowBeat() const
{
	return (m_sw.sF() - m_preRollSec) / secPerBeat();
}

void BeatTransport::setPreRoll(double sec)
{
	m_preRollSec = sec;
}

double BeatTransport::secPerBeat() const
{
	return 60.0 / m_bpm;
}

double BeatTransport::bpm() const
{
	return m_bpm;
}

// ---- BeatHitDetector実装 ----
void BeatHitDetector::reset()
{
	m_prev = 0.0;
	m_nextIndex = 0;
}
