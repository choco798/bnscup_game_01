#include "SaveDataManager.hpp"

bool SaveDataManager::initialize(const StringView& path)
{
	m_saveDataPath = path;

	// セーブデータファイルが存在しない場合は初期状態で作成
	if (!FileSystem::Exists(path))
	{
		// 初期状態のセーブデータを作成
		m_saveData = SaveData{};

		// 保存を試みる
		return save();
	}

	try
	{
		// JSONファイルを読み込む
		const JSON json = JSON::Load(path);

		// セーブデータをデシリアライズ
		m_saveData.deserialize(json);

		return true;
	}
	catch (const Error& e)
	{
		// エラーログを出力
		Print << U"Failed to load save data: " << e.what();

		// 初期状態のセーブデータを作成
		m_saveData = SaveData{};

		// 保存を試みる
		return save();
	}
}

bool SaveDataManager::save() const
{
	try
	{
		// セーブデータをJSONにシリアライズ
		JSON json;
		m_saveData.serialize(json);

		// JSONをファイルに保存
		return json.save(m_saveDataPath);
	}
	catch (const Error& e)
	{
		// エラーログを出力
		Print << U"Failed to save data: " << e.what();
		return false;
	}
}
