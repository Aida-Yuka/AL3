#include "Enemy.h"
#include <cassert>
#include "MyMath.h"
#include <numbers>
#include "MapChipField.h"

using namespace KamataEngine;
using namespace MathUtility;

float ToRadian(float degree) { return degree * (3.14159265f / 180.0f); }

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