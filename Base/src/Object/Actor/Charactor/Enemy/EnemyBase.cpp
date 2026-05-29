#include "../../../../Utility/AsoUtility.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../../Manager/SceneManager.h"
#include "EnemyBase.h"
#include "../Player.h"

EnemyBase::EnemyBase(const EnemyBase::EnemyData& data, Player* player)
	:
	TopBase(),
	type_(data.type),
	hp_(data.hp),
	defaultPos_(AsoUtility::VECTOR_ZERO),
	defaultCenterPos_(AsoUtility::VECTOR_ZERO),
	moveRange_(data.moveRange),
	stateBase_(),
	stateChanges_(),
	player_(player)
{
	// 初期座標の設定
	transform_.pos = data.defaultPos;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Draw(void)
{

	TopBase::Draw();

#ifdef _DEBUG
	// 移動可能範囲のデバッグ描画
	//DrawSphere3D(defaultPos_, moveRange_, 16, 0x000099, 0x000099, false);
#endif // _DEBUG
}

bool EnemyBase::InMovableRange(void) const
{
	bool ret = false;

	// 初期位置からの距離
	float dis = static_cast<float>(
		AsoUtility::SqrMagnitude(defaultPos_, centerPos_));

	// 指定距離判定
	if (dis < moveRange_ * moveRange_)
	{
		ret = true;
	}
	else {
		ret = false;
	}

	return ret;
}

void EnemyBase::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void EnemyBase::ProcessTopMove(void)
{
	TopBase::ProcessTopMove();
}
