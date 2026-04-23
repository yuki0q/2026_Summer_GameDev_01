#pragma once
#include "SceneBase.h"


class ResultScene : public SceneBase
{
public:
	// コンストラクタ
	ResultScene(void);
	// デストラクタ
	~ResultScene(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:

	// 画像
	int imgResult_;
	int imgResultBack_;

	// BGM
	int bgmResult_;

};