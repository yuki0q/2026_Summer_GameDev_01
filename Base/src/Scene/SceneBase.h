#pragma once
#include <DxLib.h>
class AnimationController;

class SceneBase
{

public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		ATTACK,
		WALK,
		JUMP,
		DAMAGE,
		BEFOR,
		MAX,
	};

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	// 初期化処理
	virtual void Init(void) = 0;

	// 更新ステップ
	virtual void Update(void) = 0;

	// 描画処理
	virtual void Draw(void) = 0;

	// 解放処理
	virtual void Release(void) = 0;

protected:

	// アニメーション制御
	AnimationController* animationController_;

	int modelId_;

	VECTOR pos_;
	VECTOR angles_;
	VECTOR localAngles_;
	VECTOR scale_;

	// 移動方向
	VECTOR moveDir_;
};
