#include "UIPlayer.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"

UIPlayer::UIPlayer(
	Transform* follow, float localHeight,
	float bounceHeight, float imgSize)
	:
	follow_(follow),
	localHeight_(localHeight),
	bounceHeight_(bounceHeight),
	imgSize_(imgSize)
{
}

UIPlayer::~UIPlayer(void)
{
}

void UIPlayer::Update(void)
{
	if (!active_) return;

	// バウンスイージング
	float t = 1.0f;

	// 高さ調整
	float easingOffset = t * bounceHeight_;

	// 追従
	transform_.pos = follow_->pos;

	// UIのベース座標にイージング移動量を加える
	transform_.pos.y += localHeight_ + easingOffset;
}

void UIPlayer::Draw(void)
{
	if (!active_) return;

	// ビックリマーク描画
	DrawBillboard3D(
		transform_.pos,
		0.5f, 0.5f,
		imgSize_, 0.0f, img_, true);
}

void UIPlayer::Start(void)
{
	SetActive(true);
	step_ = 0.0f;
}

void UIPlayer::InitLoad(void)
{
	// UI：ビックリマーク
	img_ = resMng_.Load(ResourceManager::SRC::IMAGE_1P).handleId_;
}

void UIPlayer::InitTransform(void)
{
}

void UIPlayer::InitPost(void)
{
}
