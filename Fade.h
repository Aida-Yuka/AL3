#pragma once
#include "KamataEngine.h"

/// <summary>
/// フェード
/// </summary>
class Fade
{
public:
	// フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	//フェード開始
	void Start(Status status, float duration);

	//フェード終了
	void Stop();

	//フェードの終了判定
	bool IsFinished() const;

private:

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	//フェードの持続時間
	float duration_ = 0.0f;

	//経過時間カウンター
	float counter_ = 0.0f;

	// 現在のフェードの状態
	Status status_ = Status::None;
};
