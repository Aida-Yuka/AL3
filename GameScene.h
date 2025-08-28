#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "MapChipField.h"
#include "Skydome.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include <vector>
#include "TitleScene.h"
#include "Fade.h"

class GameScene
{
public:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelDeathParticle_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelMenu_ = nullptr;
	KamataEngine::Model* modelMenu2_ = nullptr;

	//デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	//カメラコントローラー
	CameraController* cameraController_ = nullptr;

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~GameScene();

	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	//天球
	Skydome* skydome_ = nullptr;

	//敵
	std::list<Enemy*> enemies_;

	//デスパーティクル
	DeathParticles* deathParticles_ = nullptr;

	//表示ブロックの生成
	void GenerateBlocks();

	//全ての当たり判定を行う
	void CheckAllCollisions();

	//フェーズの切り替え
	void ChangePhase();

	//終了フラグのgetter
	bool IsFinished() const { return finished_; }

	//終了フラグ
	bool finished_ = false;

private:

	// ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,    // フェードイン
		kPlay,      // ゲームプレイ
		kDeath,     // デス演出
		kPauseMenu, // ポーズメニュー
		kFadeOut,   // フェードアウト
	};

	// ゲームの現在フェーズ
	Phase phase_;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	//ブロック用のワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>>worldTransformBlocks_;

	// カメラ
	KamataEngine::Camera camera_;

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// フェード
	Fade* fade_ = nullptr;
};