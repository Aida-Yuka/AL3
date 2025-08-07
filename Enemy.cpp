#include "Enemy.h"
#include <cassert>
#include "MyMath.h"
#include <numbers>
#include "MapChipField.h"

using namespace KamataEngine;
using namespace MathUtility;

float ToRadian(float degree) { return degree * (3.14159265f / 180.0f); }

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// AABB取得関数
AABB Enemy::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kDepth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kDepth / 2.0f};

	return aabb;
}

//衝突応答
void Enemy::OnCollision(const Player* player)
{
	(void)player;
}

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position)
{
	/// インゲームの初期化処理///

	// NULLポインタチェック
	assert(model);

	//アニメーション処理
	walkTimer_ = 0.0f;

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float>*3/2.0f;

	//速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};
}

void Enemy::Update()
{
	//移動
	worldTransform_.translation_ += velocity_;

	//タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	worldTransform_.rotation_.x = std::sin(walkTimer_);
	float param = std::sin(walkTimer_);
	float degree = kWaldMotionAngleStart + kWaldMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.x = ToRadian(degree);


	//アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Draw()
{
	//3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}