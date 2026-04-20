#pragma once
#include "SceneBase.h"

class TopSelectScene : public SceneBase
{
public:
	// コンストラクタ
	TopSelectScene(void);
	// デストラクタ
	~TopSelectScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// 画像
	int imgTopSelectBack_;
	int imgTopSelect_;
	int imgTopSelectText_;
	int imgSkillSelect_;
	int imgSkillSelectText_;

};

