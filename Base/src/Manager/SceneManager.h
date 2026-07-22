#pragma once
#include <chrono>
#include <DxLib.h>
class SceneBase;
class Fader;
class Camera;

class SceneManager
{

public:

	// 背景色
	static constexpr int BACKGROUND_COLOR_R = 0;
	static constexpr int BACKGROUND_COLOR_G = 139;
	static constexpr int BACKGROUND_COLOR_B = 139;

	// ディレクショナルライトの方向
	static constexpr VECTOR LIGHT_DIRECTION = { 0.3f, -0.7f, 0.8f };

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		STAGE_SELECT,
		TOP_SELECT,
		GAME,
		PAUSE,
		RESULT,
		DEBUG,
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 3Dの初期化
	void Init3D(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	// カメラの取得
	Camera* GetCamera(void) const;

	// 勝敗フラグをセットする (GameSceneから呼ぶ)
	void SetPlayerWin(bool isWin) { isPlayerWin_ = isWin; }

	// 勝敗フラグを取得する (ResultSceneから呼ぶ)
	bool IsPlayerWin(void) const { return isPlayerWin_; }

	// コマのタイプをセットする
	void SetPlayerTopType(int type) { playerType_ = type; }
	int GetPlayerTopType(void)const { return playerType_; }

	void SetPlayerTopType2P(int type) { playerType2_ = type; }
	int GetPlayerTopType2P(void)const { return playerType2_; }

	void SetPlayerNo(int playerNo) { playerNo_ = playerNo; }
	int GetPlayerNo(void)const { return playerNo_; }

	int GetMainScreen(void) const;

private:

	// 静的インスタンス
	static SceneManager* instance_;

	bool isPlayerWin_ = false; // プレイヤーが勝ったかどうか（生存フラグ）

	int playerType_;
	int playerType2_;
	int playerNo_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// フェード
	Fader* fader_;

	// 各種シーン
	SceneBase* scene_;
	// ポーズ画面用のシーン保持変数
	SceneBase* pauseScene_;  

	// カメラ
	Camera* camera_;

	// メインスクリーン
	int mainScreen_;

	// シーン遷移中判定
	bool isSceneChanging_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);

	// コピーコンストラクタも同様
	SceneManager(const SceneManager& instance) = default;

	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};