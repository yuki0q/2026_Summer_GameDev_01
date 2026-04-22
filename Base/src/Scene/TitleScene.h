#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		ATTACK,
		JUMP,
		DAMAGE,
		BEFOR,
		MAX,
	};
	
	// 初期位置
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// 角度
	static constexpr VECTOR DEFAULT_ANGLES = { 0.0f,0.0f,0.0f };

	// 大きさ
	static constexpr VECTOR DEFAULT_SCALE = { 0.5f,0.5f,0.5f };

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	// 画像
	int logoTitle_;
	int imgTitle_;

	// BGM
	int bgmTitle_;
};

