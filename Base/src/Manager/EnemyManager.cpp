#include <string>
#include <fstream>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/NormalEnemy.h"
#include "TopDataManager.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(Player* player, TopDataManager& dataMng)
	:
	player_(player),
	dataMng_(dataMng)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// エネミーのデータ読み込み
	//LoadCsvData();
	const TopBase::TopData* enemyData = 
		dataMng_.GetTopData(static_cast<TopBase::TOP_TYPE>(GetRand(3)));
	if (enemyData)
	{
		enemies_.push_back(Create(*enemyData));
	}
}

void EnemyManager::Update(void)
{
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (auto& enemy : enemies_)
	{
		enemy->Release();
		delete enemy;
	}


}

void EnemyManager::AddHitCollider(const ColliderBase* hitCollider)
{
	for (auto& enemy : enemies_)
	{
		enemy->AddHitCollider(hitCollider);
	}
}
//
//void EnemyManager::LoadCsvData(void)
//{
//	// ファイルの読込
//	std::ifstream ifs = std::ifstream(Application::PATH_CSV + "TopSte.csv");
//
//	if (!ifs)
//	{
//		// エラーが発生
//		return;
//	}
//
//	// ファイルを１行ずつ読み込む
//	std::string line;// 1行の文字情報
//	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割
//	bool isHeader = true;
//
//	while (getline(ifs, line))
//	{
//		if (isHeader)
//		{
//			isHeader = false;
//			continue;
//		}
//
//		// １行をカンマ区切りで分割
//		strSplit = AsoUtility::Split(line, ',');
//
//		EnemyBase* enemy = nullptr;
//
//		// 構造体に合わせて読込データを格納
//		TopBase::TopData data = TopBase::TopData();
//
//		int idx = 0;
//
//		// 種別
//		data.type = static_cast<TopBase::TOP_TYPE>(stoi(strSplit[idx++]));
//
//		// コマのスタミナ
//		data.topsSpin = stoi(strSplit[idx++]);
//
//		// 軸の安定度（高いほどブレがすぐ収まる）
//		data.stability = stoi(strSplit[idx++]);
//
//		// 平常時の傾き
//		data.defaultTilt = stoi(strSplit[idx++]);
//
//		// 歳差運動の速さ
//		data.wobbleSpeed = stoi(strSplit[idx++]);
//
//		// コマの重さ
//		data.topsWeight = stoi(strSplit[idx++]);
//
//		// コマごとの衝突の強さ
//		data.topsShock = stoi(strSplit[idx++]);
//
//		// コマの通常時の速さ
//		data.speed = stoi(strSplit[idx++]);
//
//		// コマの通常時の速さ
//		data.dashSpeed = stoi(strSplit[idx++]);
//
//		// スキル使用時のスピード
//		data.skillSpeed = stoi(strSplit[idx++]);
//
//		// スタミナの削れる速度
//		data.scrapSpeed = stoi(strSplit[idx++]);
//
//		// スタミナが15以下の時の削れる速度
//		data.dyingScrapSpeed = stoi(strSplit[idx++]);
//
//		// コマの公転半径
//		data.radiusFactor = stoi(strSplit[idx++]);
//
//		// エネミー生成
//		Create(data);
//	}
//
//	ifs.close();
//}

EnemyBase* EnemyManager::Create(const TopBase::TopData& data)
{
	EnemyBase* enemy = nullptr;

	switch (data.type)
	{
	case TopBase::TOP_TYPE::ATTACK:
		enemy = new NormalEnemy(data, player_);
		break;
	case TopBase::TOP_TYPE::DEFENSE:
		enemy = new NormalEnemy(data, player_);
		break;
	case TopBase::TOP_TYPE::STAMINA:
		enemy = new NormalEnemy(data, player_);
		break;
	case TopBase::TOP_TYPE::BALANCE:
		enemy = new NormalEnemy(data, player_);
		break;
	default:
		break;
	}

	if (enemy != nullptr) {
		enemy->Init();
		enemies_.emplace_back(enemy);
	}

	return enemy;
}