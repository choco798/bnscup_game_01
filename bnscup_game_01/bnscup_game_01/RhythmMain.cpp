# include <Siv3D.hpp>

// ---- ユーティリティ ----
double Frac(double x) { return x - std::floor(x); }

// ---- データ記法（hm.min3）サンプル ----
// 1文字=1モーラ、"="=語連結、"|"=句切れ(軽=無音+1拍)、"||"=強い句切れ(無音+2拍)、"~"=余韻(無音+1拍)
static const String KANA_STREAM =
U"きゃ=れ=え=だ=に | か=ら=す の と=ま=り~ け=り~ || あ=き の く=れ~";

// ---- 休止の既定拍数（教材向けプリセット）----
struct RestPreset {
	int lightCutBeats = 1;   // |
	int strongCutBeats = 2;  // ||
	int fermataBeats = 1;   // ~（無音は音後）
	int tailEndBeats = 2;   // 末尾の余韻（全体の流れが止まる用）
};

// ---- モーラ要素 ----
struct Mora {
	String text;         // 表示（1モーラ）
	double startBeat{}; // このモーラの開始拍（スクロール計算用）
	bool beamNext = false;    // 次モーラと連結ビーム
	bool fermata = false;     // このモーラの直後に無音休止（余韻）
};

// 句切れ（どのモーラの“直後”か）
struct Cut {
	size_t afterIndex{}; // mora配列の何番目の直後か（0-based）
	int beats{};         // 無音休止拍数
	bool strong = false;  // 表示スタイル用
};

// ---- パース：hm.min3 の1フィールドを moras / cuts に落とす ----
struct ParsedStream {
	Array<Mora> moras;
	Array<Cut>  cuts;
	double totalBeats = 0.0;  // 最終積算拍（休止含む）
};

class KanaStreamParser {
public:
	static ParsedStream Parse(const String& stream, const RestPreset& rest);
private:
	static bool IsYouonSmall(char32 ch);
};

// ★ 拗音の小書き判定（ゃ/ゅ/ょ/ャ/ュ/ョ）
bool KanaStreamParser::IsYouonSmall(char32 ch)
{
	return (ch == U'ゃ' || ch == U'ゅ' || ch == U'ょ'
		 || ch == U'ャ' || ch == U'ュ' || ch == U'ョ');
}

ParsedStream KanaStreamParser::Parse(const String& stream, const RestPreset& rest)
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
			if (!moras.empty())
				moras.back().beamNext = true;
			continue;
		}

		if (ch == U'~')
		{
			if (!moras.empty())
				moras.back().fermata = true;
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
					++i; // '||' を消費
				}
				cuts << Cut{ moras.size() - 1, (strong ? rest.strongCutBeats : rest.lightCutBeats), strong };
			}
			continue;
		}

		// ★ 拗音（小書き ゃ/ゅ/ょ）は直前モーラに結合して1モーラ化
		if (IsYouonSmall(ch) && !moras.empty())
		{
			moras.back().text.push_back(ch);
			continue; // ← 新しいモーラは作らない
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
		t += 1.0; // 1モーラ=1拍

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


// ---- 描画（“音符フォント不使用”でそれっぽく） ----
struct Layout {
	double laneY = 300;       // ベース線Y
	double pxPerBeat = 160;   // 1拍の横幅（表示スケール）
	double hitX = 240;        // “いま”の基準線X
	double moraRadius = 12;   // モーラ玉の半径
	double beamY = 10;        // ビーム（連結線）の縦位置オフセット（玉中心からの相対）
};

class BeatTransport {
public:
	explicit BeatTransport(double bpm) : m_bpm(bpm) {}
	void start() { m_sw.start(); }
	void pause() { m_sw.pause(); }
	void resume() { m_sw.resume(); }
	void reset() { m_sw.reset(); }
	bool isRunning() const { return m_sw.isRunning(); }

	double nowBeat() const { return (m_sw.sF() - m_preRollSec) / secPerBeat(); }
	void   setPreRoll(double sec) { m_preRollSec = sec; } // -1秒からスタート等

	double secPerBeat() const { return 60.0 / m_bpm; }
	double bpm() const { return m_bpm; }
private:
	double   m_bpm{};
	double   m_preRollSec{ 1.0 }; // 既定で -1秒の余裕を設けるなら 1.0
	Stopwatch m_sw{ StartImmediately::No };
};

class MoraRenderer {
public:
	MoraRenderer();
	void drawGuide(const Layout& L, double scrollBeat) const;
	void drawCuts(const ParsedStream& P, const Layout& L, double scrollBeat) const;
	void drawMoras(const ParsedStream& P, const Layout& L, double scrollBeat, double voiceLevel) const;
private:
	void drawMoraGlyph(const Mora& m, const Layout& L, double x, const ColorF& glow) const;
};

namespace
{
	// 内部ヘルパー：Cut線描画
	void DrawCut(double x, const Layout& L, bool strong)
	{
		const double len = strong ? 90.0 : 70.0;
		const double thick = strong ? 6.0 : 3.0;
		Line{ x, L.laneY - len * 0.5, x, L.laneY + len * 0.5 }
		.draw(thick, Color(0, 0, 0, 160));
	}

	// 内部ヘルパー：モーラ間のビーム（語の滑らかさ）
	void DrawBeam(double x1, double x2, const Layout& L)
	{
		if (x2 <= x1) return;
		const double y = L.laneY + L.beamY;
		const double h = 4.0;
		RectF{ x1 + 6, y - h * 0.5, (x2 - x1) - 12, h }
		.draw(ColorF{ 0, 0, 0, 0.75 });
	}

	// 内部ヘルパー：フェルマータ（弧＋点で近似）
	void DrawFermata(double x, const Layout& L)
	{
		const double r = L.moraRadius + 8;
		const Circle c{ x, L.laneY - (L.moraRadius + 12), r };
		c.drawArc(-90.0_deg, 180.0_deg, 2.5, 0.0, Palette::Black);
		Circle{ x, c.y, 4.5 }.draw(Palette::Black);
	}
}

MoraRenderer::MoraRenderer()
{
	FontAsset::Register(U"KANA_BIG", FontMethod::MSDF, 20, Typeface::Regular);
	FontAsset::Register(U"KANA_SMALL", FontMethod::MSDF, 16, Typeface::Regular);
	FontAsset::Register(U"UI_SMALL", FontMethod::MSDF, 18, Typeface::Regular);
}

void MoraRenderer::drawGuide(const Layout& L, double scrollBeat) const
{
	// いまのライン
	Line{ L.hitX, 40, L.hitX, Scene::Height() - 40 }.draw(3, Palette::Orange);

	// ベースライン
	Line{ 0, L.laneY, Scene::Width(), L.laneY }.draw(2, Palette::Black);

	// 拍グリッド
	for (int i = -8; i <= 24; ++i)
	{
		const double gx = L.hitX + (i - Frac(scrollBeat)) * L.pxPerBeat;
		const Color c = (i == 0 ? Palette::Red
								: Color(0, 0, 0, (i % 4 == 0 ? 64 : 28)));
		Line{ gx, L.laneY - 60, gx, L.laneY + 60 }.draw(1, c);
	}
}

void MoraRenderer::drawCuts(const ParsedStream& P, const Layout& L, double scrollBeat) const
{
	for (const auto& c : P.cuts)
	{
		const double cutBeat = P.moras[c.afterIndex].startBeat + 1.0;
		const double x = L.hitX + (cutBeat - scrollBeat) * L.pxPerBeat;
		if (-20 <= x && x <= Scene::Width() + 20)
		{
			DrawCut(x, L, c.strong);
		}
	}
}

void MoraRenderer::drawMoras(const ParsedStream& P, const Layout& L, double scrollBeat, double voiceLevel) const
{
	const ColorF glowColor = ColorF(1.0, 0.9, 0.6, 0.35 * voiceLevel * 10.0);

	for (size_t i = 0; i < P.moras.size(); ++i)
	{
		const auto& m = P.moras[i];
		const double x = L.hitX + (m.startBeat - scrollBeat) * L.pxPerBeat;
		if (x < -120 || x > Scene::Width() + 120)
			continue;

		// ビーム（次モーラ連結）
		if (m.beamNext && (i + 1 < P.moras.size()))
		{
			const double nx = L.hitX + (P.moras[i + 1].startBeat - scrollBeat) * L.pxPerBeat;
			if (nx > x + 24)
				DrawBeam(x, nx, L);
		}

		drawMoraGlyph(m, L, x, glowColor);
	}
}

void MoraRenderer::drawMoraGlyph(const Mora& m, const Layout& L, double x, const ColorF& glow) const
{
	// 声量エフェクト：背景グロー
	if (glow.a > 0.01)
	{
		Circle{ x, L.laneY, L.moraRadius * (1.0 + 0.8 * glow.a) }
		.drawShadow(Vec2{ 0, 0 }, 8, 4, glow);
	}

	// モーラ玉
	Circle{ x, L.laneY, L.moraRadius }.draw(Palette::Black);

	// 読み仮名（拗音など2文字以上は小フォント）
	const Font& f = (m.text.size() >= 2 ? FontAsset(U"KANA_SMALL") : FontAsset(U"KANA_BIG"));
	f(m.text).drawAt(Vec2{ x, L.laneY + 26 }, Palette::Black);

	// フェルマータ
	if (m.fermata)
		DrawFermata(x, L);
}

class BeatHitDetector {
public:
	void reset() { m_prev = 0.0; m_nextIndex = 0; }
	// スクロール更新のたびに呼ぶ。渡したコールバックでSEや任意処理を発火
	template<class Fn>
	void process(const ParsedStream& P, double nowBeat, Fn onHit) {
		constexpr double eps = 1e-6;
		while (m_nextIndex < P.moras.size() &&
			   P.moras[m_nextIndex].startBeat <= nowBeat + eps)
		{
			if (P.moras[m_nextIndex].startBeat > m_prev + eps) {
				onHit(m_nextIndex); // ここで se.playOneShot() 等
			}
			++m_nextIndex;
		}
		m_prev = nowBeat;
	}
private:
	double m_prev{ 0.0 };
	size_t m_nextIndex{ 0 };
};

struct VoiceState { bool active; double energy; double noise; double onTh; double offTh; };
class IVoiceActivityDetector {
public:
	virtual ~IVoiceActivityDetector() = default;
	virtual void finalize() = 0;
	virtual bool update() = 0;             // 状態変化が起きたら true
	virtual VoiceState state() const = 0;  // 現在値
};

class VoiceReactiveFx {
public:
	void update(const VoiceState& s, double dt) {
		// 簡易：エネルギー正規化 → 平滑 → 0..1
		double target = Saturate((s.energy - s.noise + (s.active ? 0.5 : 0)) / (s.noise * 4.0 + 0.1 + 0.5));
		m_level = Math::Lerp(m_level, target, 0.15); // スムージング
	}
	double level() const { return m_level; }
private:
	double m_level{ 0.0 };
};

//---------------------------------------------
// VoiceActivityDetector
//---------------------------------------------
class VoiceActivityDetector : public IVoiceActivityDetector {
public:
	struct Params {
		// ノイズ推定
		double alpha = 0.02;  // ノイズEMAの追従度 (0..1)

		// ヒステリシス閾値（noiseEMA に対する倍率）＋ 絶対足しこみ
		double kOn = 1.3;
		double kOff = 0.8;
		double absOn = 0.005;
		double absOff = 0.01;

		// 人声帯域 [Hz]
		double bandLowHz = 80.0;
		double bandHighHz = 6000.0;

		// 最小継続時間 [ms]
		int32 minOnMs = 80;   // 「声あり」確定までに必要な最小継続
		int32 minOffMs = 200;   // 「無音」確定までに必要な最小継続
	};

	struct DebugLog {
		bool   enabled = false;             // true で記録
		FilePath logPath = U"vad_log.csv";    // 出力先
		bool   append = false;              // 追記（falseで新規作成）
		bool   writeHeader = true;              // ヘッダ行の出力
		int32  flushEveryN = 30;                // Nフレームごとに flush
	};

	VoiceActivityDetector(
		const Optional<uint32>& deviceIndex = unspecified,
		const Params& params = {},
		const DebugLog& dbg = {}) :
		m_params(params), m_dbgOpt(dbg)
	{
		// すぐ録音開始
		m_mic = Microphone{ deviceIndex };
		m_mic.start();

		if (not m_mic) {
			throw Error{ U"Microphone not available" };
		}

		// ログ初期化
		if (m_dbgOpt.enabled) {
			const OpenMode mode = (m_dbgOpt.append ? OpenMode::Append : OpenMode::Trunc);
			if (m_writer.open(m_dbgOpt.logPath, mode)) {
				if (!m_dbgOpt.append || m_dbgOpt.writeHeader) {
					// CSVヘッダ
					// time_ms, energy, noiseEMA, onThresh, offThresh, rawVoice, confirmedVoice, i0, i1, hzPerBin, fps
					m_writer.writeln(U"time_ms,energy,noiseEMA,onThresh,offThresh,rawVoice,confirmedVoice,i0,i1,hzPerBin,fps");
				}
			}
			else {
				Print << U"[VAD] ログファイルを開けません: " << m_dbgOpt.logPath;
			}
		}
	}

	~VoiceActivityDetector()
	{
	}

	// 1フレーム更新。戻り値は「確定状態が変化したか」
	bool update() override
	{
		// スペクトル取得
		m_mic.fft(m_fft);

		// 帯域エネルギー
		const double hzPerBin = m_fft.resolution;               // 1bin あたりの周波数幅
		const int32 i0 = Max(0, static_cast<int32>(m_params.bandLowHz / hzPerBin));
		const int32 i1 = Min<int32>(static_cast<int32>(m_fft.buffer.size()) - 1,
									 static_cast<int32>(m_params.bandHighHz / hzPerBin));

		double bandEnergy = 0.0;
		for (int i = i0; i <= i1; ++i) {
			bandEnergy += m_fft.buffer[i];
		}
		m_lastEnergy = bandEnergy;

		// ノイズEMA更新
		m_noiseEMA = Math::Lerp(m_noiseEMA, bandEnergy, m_params.alpha);

		// 閾値
		const double onThresh = (m_noiseEMA * m_params.kOn) + m_params.absOn;
		const double offThresh = (m_noiseEMA * m_params.kOff) + m_params.absOff;

		// 「生状態」（瞬間判定。まだ確定しない）
		const bool rawVoice = (bandEnergy > onThresh)
			|| (m_rawVoicePrev && (bandEnergy > offThresh)); // ヒステリシス
		if (rawVoice != m_rawVoicePrev) {
			// 状態遷移が起きたら、継続時間計測をリセット
			m_stageTimer.restart();
			m_rawVoicePrev = rawVoice;
		}

		// 最小継続時間で確定
		bool changed = false;
		if (m_confirmedVoice != rawVoice) {
			const int32 needMs = rawVoice ? m_params.minOnMs : m_params.minOffMs;
			if (m_stageTimer.ms() >= needMs) {
				m_confirmedVoice = rawVoice;
				changed = true;
			}
		}

		// ログ書き出し
		writeLog(rawVoice, onThresh, offThresh, i0, i1, hzPerBin);

		// UIの表示
		drawUI();

		return changed;
	}

	void drawUI()
	{
		// 簡易UI
		const double e = energy();
		const double n = noiseEMA();

		RectF{ 40, 380, 600, 24 }.draw(Palette::White);
		RectF{ 40, 380, 600 * Saturate(e / (n * 5.0 + 0.1)), 24 }
		.draw(isVoice() ? ColorF{ 0.9, 0.6, 0.2 } : ColorF(Palette::Gray));

		FontAsset(U"UI_SMALL")(U"energy:{:.4f}  noise:{:.4f}  state:{}"_fmt(
			e, n, (isVoice() ? U"VOICE" : U"SILENCE")))
			.draw(40, 340, Palette::Black);

		// しきい値目安線
		const double thOn = onThreshold();
		const double thOff = offThreshold();
		const double base = (n * 5.0 + 0.1);
		const double xOn = 40 + 600 * Saturate(thOn / base);
		const double xOff = 40 + 600 * Saturate(thOff / base);
		Line{ xOn, 380, xOn, 404 }.draw(2, Palette::Orange);
		Line{ xOff,380, xOff,404 }.draw(2, Palette::Skyblue);
	}

	void finalize() override
	{
		m_writer.close();
	}

	// 現在の「確定」状態
	bool isVoice() const { return m_confirmedVoice; }

	// 可視化・デバッグ用
	double energy() const { return m_lastEnergy; }
	double noiseEMA() const { return m_noiseEMA; }
	double onThreshold() const { return (m_noiseEMA * m_params.kOn) + m_params.absOn; }
	double offThreshold() const { return (m_noiseEMA * m_params.kOff) + m_params.absOff; }
	const Params& params() const { return m_params; }
	VoiceState state() const override
	{
		VoiceState result{};
		result.active = m_confirmedVoice;
		result.energy = energy();
		result.noise = noiseEMA();
		result.onTh = onThreshold();
		result.offTh = offThreshold();
		return result;
	}

private:
	void writeLog(bool rawVoice, double onTh, double offTh,
				  int32 i0, int32 i1, double hzPerBin)
	{
		if (!m_dbgOpt.enabled || !m_writer.isOpen()) return;

		// できるだけ解析しやすいように ms 単位、真偽は 0/1 で
		const int64 tms = Time::GetMillisec(); // 起動からのms
		const double fps = Profiler::FPS();

		// CSV 1行
		// time_ms,energy,noiseEMA,onThresh,offThresh,rawVoice,confirmedVoice,i0,i1,hzPerBin,fps
		m_writer.writeln(Format(
			tms, U",",
			m_lastEnergy, U",",
			m_noiseEMA, U",",
			onTh, U",",
			offTh, U",",
			(rawVoice ? 1 : 0), U",",
			(m_confirmedVoice ? 1 : 0), U",",
			i0, U",",
			i1, U",",
			hzPerBin, U",",
			fps
		));
	}

private:
	Params m_params;
	DebugLog m_dbgOpt;

	Microphone m_mic;
	FFTResult  m_fft;

	double m_noiseEMA = 0.0;
	double m_lastEnergy = 0.0;

	TextWriter m_writer;
	int32      m_logFrameCount = 0;

	bool m_rawVoicePrev = false; // 生状態（最小継続前の“候補”）
	bool m_confirmedVoice = false; // 最小継続を満たした確定状態
	Stopwatch m_stageTimer{ StartImmediately::No };
};

class RhythmApp {
public:
	RhythmApp(const String& kanaStream, double bpm)
		: m_transport(bpm)
		, m_parsed(KanaStreamParser::Parse(kanaStream, RestPreset{}))
	{
	}

	void finalize()
	{
		if (m_vad)
		{
			m_vad->finalize();
		}
	}

	void updateAndDraw() {
		handleInput(); // Space/Pause, R/Reset など

		const double nowBeat = clampBeat(m_transport.nowBeat(), m_parsed.totalBeats);

		// モーラ通過イベント（SE）
		m_hit.process(m_parsed, nowBeat, [&](size_t i) {
			m_se.playOneShot(0.7);
		});

		// VAD
		if (m_vad && m_vad->update()) {
			// 状態変化時のフックがあればここで
		}
		if (m_vad) {
			m_fx.update(m_vad->state(), Scene::DeltaTime());
		}

		// 描画
		drawBackground();
		m_renderer.drawGuide(m_layout, nowBeat);
		m_renderer.drawCuts(m_parsed, m_layout, nowBeat);
		m_renderer.drawMoras(m_parsed, m_layout, nowBeat, m_fx.level());
		drawUI();

		// 末尾更新
		// （BeatHitDetector 内部で prev を保持するのでここは不要）
	}

	void setVAD(std::unique_ptr<IVoiceActivityDetector> vad) { m_vad = std::move(vad); }

private:
	void handleInput() {
		if (KeySpace.down()) {
			if (m_transport.isRunning()) m_transport.pause();
			else                         m_transport.start();
		}
		if (KeyR.down()) {
			m_transport.reset();
			m_hit.reset();
		}
	}
	double clampBeat(double b, double max) const {
		if (b > max) return max;
		return b;
	}
	void drawBackground() const {
		// 例：声量 m_fx.level() で背景の明るさを微調整
		const double a = 0.98 - 0.1 * m_fx.level();
		Scene::SetBackground(ColorF{ a });
	}
	void drawUI() const {
		FontAsset(U"UI_SMALL")(U"[SPACE] 再生/一時停止  [R] リセット  BPM:{}"_fmt(m_transport.bpm()))
			.draw(20, 20, Palette::Black);
	}

private:
	Layout m_layout;
	BeatTransport m_transport;
	ParsedStream  m_parsed;
	MoraRenderer  m_renderer;
	BeatHitDetector m_hit;

	std::unique_ptr<IVoiceActivityDetector> m_vad; // 差し替え可能
	VoiceReactiveFx m_fx;

	Audio m_se{ U"notes.wav" };
};

void Main()
{
	Window::Resize(960, 540);

	RhythmApp app{ KANA_STREAM, /*bpm=*/120.0 };
	{
		// 既存の VAD 実装を包んだアダプタを渡す
		app.setVAD(std::make_unique<VoiceActivityDetector>());
	}

	while (System::Update())
	{
		app.updateAndDraw();
	}

	app.finalize();
}
