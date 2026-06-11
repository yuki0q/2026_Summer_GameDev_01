#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	Resource* res;

	// タイトル画像
	res = new RES(RES_T::IMG, PATH_IMG + "Title.png");
	resourcesMap_.emplace(SRC::TITLE_IMG, res);

	// 
	res = new RES(RES_T::IMG, PATH_IMG + "PushSpace.png");
	resourcesMap_.emplace(SRC::TITLE_PUSH_SPACE, res);

	// 
	//res = new RES(RES_T::MODEL, PATH_MDL + "Stage/PitfallPlanet/PitfallPlanet.mv1");
	//resourcesMap_.emplace(SRC::PIT_FALL_PLANET, res);

	//// 
	//res = new RES(RES_T::MODEL, PATH_MDL + "Stage/SpherePlanet/SpherePlanet.mv1");
	//resourcesMap_.emplace(SRC::SPHERE_PLANET, res);

	//// プレイヤーモデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Tops/Top.mv1");
	resourcesMap_.emplace(SRC::TOP, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Tops/Top_Blue.mv1");
	resourcesMap_.emplace(SRC::BLUE_TOP, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Tops/Top_Red.mv1");
	resourcesMap_.emplace(SRC::RED_TOP, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Tops/Top_Green.mv1");
	resourcesMap_.emplace(SRC::GREEN_TOP, res);

	res = new RES(RES_T::MODEL, PATH_MDL + "Tops/Top_Yellow.mv1");
	resourcesMap_.emplace(SRC::YELLOW_TOP, res);

	// ステージモデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/Base2.mv1");
	resourcesMap_.emplace(SRC::MAIN_STAGE, res);

	// スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + "SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, res);

	//// プレイヤー影
	//res = new RES(RES_T::IMG, PATH_IMG + "Shadow.png");
	//resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	//// ネズミ
	//res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Rat/Rat.mv1");
	//resourcesMap_.emplace(SRC::ENEMY_RAT, res);

	//// ロボット
	//res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Robot/Robot.mv1");
	//resourcesMap_.emplace(SRC::ENEMY_ROBOT, res);

	// 視野
	res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Robot/Cone.mv1");
	resourcesMap_.emplace(SRC::VIEW_RANGE, res);

	// 視野
	res = new RES(RES_T::IMG, PATH_IMG + "Surprise.png");
	resourcesMap_.emplace(SRC::UI_SURPRISE, res);

	res = new RES(RES_T::IMG, PATH_IMG + "3.png");
	resourcesMap_.emplace(SRC::IMAGE_3, res);

	res = new RES(RES_T::IMG, PATH_IMG + "2.png");
	resourcesMap_.emplace(SRC::IMAGE_2, res);

	res = new RES(RES_T::IMG, PATH_IMG + "1.png");
	resourcesMap_.emplace(SRC::IMAGE_1, res);

	res = new RES(RES_T::IMG, PATH_IMG + "YouWin.png");
	resourcesMap_.emplace(SRC::RESULT_WIN, res);

	res = new RES(RES_T::IMG, PATH_IMG + "YouLose.png");
	resourcesMap_.emplace(SRC::RESULT_LOSE, res);

	res = new RES(RES_T::IMG, PATH_IMG + "1P.png");
	resourcesMap_.emplace(SRC::IMAGE_1P, res);

	res = new RES(RES_T::IMG, PATH_IMG + "2P.png");
	resourcesMap_.emplace(SRC::IMAGE_2P, res);

	res = new RES(RES_T::IMG, PATH_IMG + "CPU.png");
	resourcesMap_.emplace(SRC::IMAGE_CPU, res);
		
	res = new RES(RES_T::IMG, PATH_IMG + "Button.png");
	resourcesMap_.emplace(SRC::IMAGE_BUTTON, res);

	res = new RES(RES_T::IMG, PATH_IMG + "ButtonPushed.png");
	resourcesMap_.emplace(SRC::IMAGE_PUSHED_BUTTON, res);

	res = new RES(RES_T::IMG, PATH_IMG + "returnGame.png");
	resourcesMap_.emplace(SRC::RETURN_GAME, res);

	res = new RES(RES_T::IMG, PATH_IMG + "returnTitle.png");
	resourcesMap_.emplace(SRC::RETURN_TITLE, res);

	res = new RES(RES_T::IMG, PATH_IMG + "returnSelect.png");
	resourcesMap_.emplace(SRC::RETURN_SELECT, res);

	res = new RES(RES_T::IMG, PATH_IMG + "gameEnd.png");
	resourcesMap_.emplace(SRC::GAME_END, res);

	res = new RES(RES_T::IMG, PATH_IMG + "SelectNow.png");
	resourcesMap_.emplace(SRC::SELECT_NOW, res);

	res = new RES(RES_T::IMG, PATH_IMG + "gameStart.png");
	resourcesMap_.emplace(SRC::GAME_START, res);

	res = new RES(RES_T::IMG, PATH_IMG + "instructions.png");
	resourcesMap_.emplace(SRC::IMAGE_INSTRUCTUION, res);

}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
