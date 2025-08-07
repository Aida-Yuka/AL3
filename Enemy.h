#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Player;

/// <summary>
/// 敵
/// </summary>
class Enemy
{
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	//衝突応答
	void OnCollision(const Player* player);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// キャラクターの当たり判定
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kDepth = 0.8f;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 90.0f;

	//歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	//速度
	KamataEngine::Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWaldMotionAngleStart = -20.0f;
	// 最後の角度[度]
	static inline const float kWaldMotionAngleEnd = 70.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 0.5f;

	//経過時間
	float walkTimer_ = 0.0f;
};
