#pragma once
#include "KamataEngine.h"
#include "Fade.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene
{
public:
	// フェードの状態
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部分
		kFadeOut, // フェードアウト
	};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~TitleScene();

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//モデルの生成
	KamataEngine::Model* modelTitle_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	//終了フラグ
	bool finished_ = false;

	//死亡フラグのgetter
	bool IsFinished() const { return finished_; }

private:
	// カメラ
	KamataEngine::Camera camera_;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransformTitle_;
	KamataEngine::WorldTransform worldTransformPlayer_;
	
	//フェード
	Fade* fade_ = nullptr;

	// 現在のフェードの状態
	Phase phase_ = Phase::kFadeIn;
};
