#include <fstream>
#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "TopDataManager.h"

TopDataManager* TopDataManager::instance_ = nullptr;

void TopDataManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new TopDataManager();
	}
	instance_->Init();
}

TopDataManager& TopDataManager::GetInstance(void)
{
	return *instance_;
}

void TopDataManager::Init(void) 
{
	LoadCsvData("TopSte.csv");
}

void TopDataManager::LoadCsvData(const std::string& filePath)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + filePath);

	if (!ifs)
	{
		// エラーが発生
		return;
	}

	// ファイルを１行ずつ読み込む
	std::string line;// 1行の文字情報
	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割
	bool isHeader = true;

	while (getline(ifs, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		// １行をカンマ区切りで分割
		strSplit = AsoUtility::Split(line, ',');

		// 構造体に合わせて読込データを格納
		TopBase::TopData data = TopBase::TopData();

		int idx = 0;

		// 種別
		data.type = static_cast<TopBase::TOP_TYPE>(stoi(strSplit[idx++]));

		// コマのスタミナ
		data.topsSpin = stoi(strSplit[idx++]);

		// 軸の安定度（高いほどブレがすぐ収まる）
		data.stability = stoi(strSplit[idx++]);

		// 平常時の傾き
		data.defaultTilt = stoi(strSplit[idx++]);

		// 歳差運動の速さ
		data.wobbleSpeed = stoi(strSplit[idx++]);

		// コマの重さ
		data.topsWeight = stoi(strSplit[idx++]);

		// コマごとの衝突の強さ
		data.topsShock = stoi(strSplit[idx++]);

		// コマの通常時の速さ
		data.speed = stoi(strSplit[idx++]);

		// コマの通常時の速さ
		data.dashSpeed = stoi(strSplit[idx++]);

		// スキル使用時のスピード
		data.skillSpeed = stoi(strSplit[idx++]);

		// スタミナの削れる速度
		data.scrapSpeed = stoi(strSplit[idx++]);

		// スタミナが15以下の時の削れる速度
		data.dyingScrapSpeed = stoi(strSplit[idx++]);

		// コマの公転半径
		data.radiusFactor = stoi(strSplit[idx++]);

		dataMap_[data.type] = data;
	}

	ifs.close();
}

const TopBase::TopData* TopDataManager::GetTopData(TopBase::TOP_TYPE type) const
{
	auto it = dataMap_.find(type);
	if (it != dataMap_.end())
	{
		return &(it->second);
	}
	return nullptr; // 見つからなかった場合
}

void TopDataManager::Destroy(void)
{
	// インスタンスのメモリ解放
	delete instance_;
}