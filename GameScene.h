#pragma once
#include "KamataEngine.h"

class GameScene
{
public:
	//デストラクタ
	~GameScene();

	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw();

private:
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデル
	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	//カメラ
	KamataEngine::Camera camera_;

	//サウンドデータハンドル
	uint32_t soundDateHandle_ = 0;

	//音声再生ハンドル
	uint32_t voiceHandle_ = 0;

	//ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};
};
