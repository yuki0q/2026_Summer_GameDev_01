#include "UIEnemy.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"

UIEnemy::UIEnemy(
	Transform* follow, float localHeight,
	float bounceHeight, float imgSize)
	:
	follow_(follow),
	localHeight_(localHeight),
	bounceHeight_(bounceHeight),
	imgSize_(imgSize)
{
}

UIEnemy::~UIEnemy(void)
{
}

void UIEnemy::Update(void)
{
	if (!active_) return;

	// バウンスイージング
	float t = 1.0f;

	if (step_ < TIME_BOUNCE)
	{
		t = EaseOutBounce(step_ / TIME_BOUNCE);
	}

	// 高さ調整
	float easingOffset = t * bounceHeight_;

	// 追従
	transform_.pos = follow_->pos;

	// UIのベース座標にイージング移動量を加える
	transform_.pos.y += localHeight_ + easingOffset;
}

void UIEnemy::Draw(void)
{
	if (!active_) return;

	// ビックリマーク描画
	DrawBillboard3D(
		transform_.pos,
		0.5f, 0.5f,
		imgSize_, 0.0f, img_, true);
}

void UIEnemy::Start(void)
{
	SetActive(true);
	step_ = 0.0f;
}

void UIEnemy::InitLoad(void)
{
	// UI：ビックリマーク
	img_ = resMng_.Load(ResourceManager::SRC::IMAGE_CPU).handleId_;
}

void UIEnemy::InitTransform(void)
{
}

void UIEnemy::InitPost(void)
{
}