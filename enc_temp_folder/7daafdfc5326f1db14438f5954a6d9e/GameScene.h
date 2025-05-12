#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>

class GameScene
{
public:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;


	//デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~GameScene();

private:
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	//ブロック用のワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>>worldTransformBlocks_;

	// カメラ
	KamataEngine::Camera camera_;

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;
};
