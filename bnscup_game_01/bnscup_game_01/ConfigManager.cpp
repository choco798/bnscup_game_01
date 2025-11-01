#include "ConfigManager.hpp"

bool ConfigManager::initialize(const StringView& path)
{
	m_configPath = path;

	// 設定ファイルが存在しない場合はデフォルト値を使用
	if (!FileSystem::Exists(path))
	{
		// デフォルト値で初期化済みなので、ファイルを作成
		return save();
	}

	try
	{
		const JSON json = JSON::Load(path);
		if (!json)
		{
			Print << U"Failed to load config file: " << path;
			return false;
		}

		// UI設定の読み込み
		if (const auto ui = json[U"ui"])
		{
			m_ui.maxLineWidth =
				ui[U"maxLineWidth"].getOr<double>(m_ui.maxLineWidth);
			m_ui.lineHeightScale =
				ui[U"lineHeightScale"].getOr<double>(m_ui.lineHeightScale);
			m_ui.lineWidthScale =
				ui[U"lineWidthScale"].getOr<double>(m_ui.lineWidthScale);
			m_ui.hitboxPaddingPx =
				ui[U"hitboxPaddingPx"].getOr<double>(m_ui.hitboxPaddingPx);
			m_ui.hitboxPaddingScale = ui[U"hitboxPaddingScale"].getOr<double>(
				m_ui.hitboxPaddingScale);
			m_ui.wordWidth = ui[U"wordWidth"].getOr<int32_t>(m_ui.wordWidth);
			m_ui.clientSizeX =
				ui[U"clientSizeX"].getOr<int32_t>(m_ui.clientSizeX);
			m_ui.clientSizeY =
				ui[U"clientSizeY"].getOr<int32_t>(m_ui.clientSizeY);
		}

		// オーディオ設定の読み込み
		if (const auto audio = json[U"audio"])
		{
			m_audio.bgmVolume =
				audio[U"bgmVolume"].getOr<double>(m_audio.bgmVolume);
			m_audio.seVolume =
				audio[U"seVolume"].getOr<double>(m_audio.seVolume);
		}

		// リズム設定の読み込み
		if (const auto rhythm = json[U"rhythm"])
		{
			m_rhythm.deserialize(rhythm);
		}

		return true;
	}
	catch (const Error& e)
	{
		// エラーログを出力
		Print << U"Error loading config: " << e.what();
		return false;
	}
}

bool ConfigManager::save() const
{
	try
	{
		JSON json;

		// UI設定の保存
		{
			JSON ui;
			ui[U"maxLineWidth"] = m_ui.maxLineWidth;
			ui[U"lineHeightScale"] = m_ui.lineHeightScale;
			ui[U"lineWidthScale"] = m_ui.lineWidthScale;
			ui[U"hitboxPaddingPx"] = m_ui.hitboxPaddingPx;
			ui[U"hitboxPaddingScale"] = m_ui.hitboxPaddingScale;
			ui[U"wordWidth"] = m_ui.wordWidth;
			ui[U"clientSizeX"] = m_ui.clientSizeX;
			ui[U"clientSizeY"] = m_ui.clientSizeY;
			json[U"ui"] = ui;
		}

		// オーディオ設定の保存
		{
			JSON audio;
			audio[U"bgmVolume"] = m_audio.bgmVolume;
			audio[U"seVolume"] = m_audio.seVolume;
			json[U"audio"] = audio;
		}

		// リズム設定の保存
		{
			JSON rhythm;
			m_rhythm.serialize(rhythm);
			json[U"rhythm"] = rhythm;
		}

		return json.save(m_configPath);
	}
	catch (const Error& e)
	{
		Print << U"Error saving config: " << e.what();
		return false;
	}
}
