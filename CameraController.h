#pragma once
#include "KamataEngine.h"

//前方宣言
class Player;

/// <summary>
/// カメラコントローラー
/// </summary>
class CameraController
{
public:
	void Intialize();

	void Initialize(KamataEngine::Camera* camera);

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	//瞬間合わせ
	void Reset();

	// 矩形
	struct Rect
	{
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	KamataEngine::Camera camera_;

	//プレイヤーにポインタ
	Player* target_ = nullptr;

	//瞬間合わせ
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	//カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	//カメラの目標座標
	KamataEngine::Vector3 targetPosition_;

	//座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	//速度掛け率
	static inline const float kVelocityBias = 10;
};

