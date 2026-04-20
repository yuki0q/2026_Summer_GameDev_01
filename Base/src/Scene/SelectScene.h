#pragma once
#include "SceneBase.h"

class SelectScene : public SceneBase
{
public:
	// コンストラクタ
	SelectScene(void);
	// デストラクタ
	~SelectScene(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:

	// 画像
	int imgStage_;
	int imgTitleText_;
	int imgSelectBack_;
};

