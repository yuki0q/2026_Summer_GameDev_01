#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	// リソース名
	enum class SRC
	{
		TITLE_IMG,
		TITLE_LOGO,
		TITLE_PUSH_SPACE,
		TOP,
		TOP_SHADOW,
		RED_TOP,
		BLUE_TOP,
		GREEN_TOP,
		YELLOW_TOP,
		MAIN_STAGE,
		SKY_DOME,
		VIEW_RANGE,
		UI_SURPRISE,
		IMAGE_3,
		IMAGE_2,
		IMAGE_1,
		IMAGE_WIN,
		IMAGE_INTRO_AT,
		IMAGE_INTRO_DF,
		IMAGE_INTRO_ST,
		IMAGE_INTRO_BL,
		RESULT_WIN,
		RESULT_LOSE,
		RESULT_1P_WIN,
		RESULT_2P_WIN,
		IMAGE_1P,
		IMAGE_2P,
		IMAGE_CPU,
		IMAGE_BUTTON,
		IMAGE_PUSHED_BUTTON,
		RETURN_GAME,
		RETURN_TITLE,
		RETURN_SELECT,
		GAME_END,
		SELECT_NOW,
		GAME_START,
		CONFIG_IMG,
		IMAGE_INSTRUCTUION,
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
