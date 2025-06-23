#define NOMINMAX

#include "Player.h"
#include <cassert>
#include "MyMath.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(Model* model, Camera* camera,const Vector3& position) {
	/// インゲームの初期化処理///

	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;
	//position_ = position;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
		
	// 初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	/// インゲームの更新処理///

	//移動入力
	//設置状態
	if (onGround_)
	{
		/// 慣性移動入力
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				acceleration.x += kAcceleration;

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				// 左右加速
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				acceleration.x -= kAcceleration;

				// 右移動中は左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				// 左右加速
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速／減速
			velocity_ += acceleration;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
		else
		{
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		//ジャンプ入力
		if (Input::GetInstance()->PushKey(DIK_UP))
		{
			//ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	}
	else
	{
		//落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		//落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	//旋回制御
	if (turnTimer_ > 0.0f)
	{
		turnTimer_ -= 1.0f / 60.0f;

		//左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		//状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		//自キャラの角度を設定する
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	//移動
	worldTransform_.translation_ += velocity_;

	//着地フラグ
	bool landing = false;

	//地面との当たり判定
	if (velocity_.y < 0)
	{
		//Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f)
		{
			landing = true;
		}
	}

	//接地判定
	if (onGround_)
	{
		//ジャンプ開始
		if (velocity_.y > 0.0f)
		{
			//空中状態に移行
			onGround_ = false;
		}
	}
	else
	{
		//着地
		if (landing)
		{
			//めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			//摩擦で横方向速度で減衰する
			velocity_.x *= (1.0f - kAttenuation);
			//下方向速度をリセット
			velocity_.y = 0.0f;
			//接地状態に移行
			onGround_ = true;
		}
	}

	//行列更新
	worldTransform_.TransferMatrix();
	
	//アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	/*//全体の流れを整理する///
	//①移動入力

	//②移動量を加味して衝突判定する
	//衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.displacement = velocity_;
	//マップ衝突チェック
	mapCollisionDetection(collisionMapInfo);

	mapCollisionDetection(info);

	//指定した角の座標計算
	Vector3 CornerPosition(const Vector3& center, Corner corner)
	{
		if (corner == kRightBottom)
		{
			return center + {kWidth / 2.0f, kHeight / 2.0f, 0};
		}
		else if (corner == kLeftBottom)
		{
			return center + {-kWidth / 2.0f, -kHeight / 2.0f, 0};
		}
		else if (corner == kRightTop)
		{
			return center + {kWidth / 2.0f, kHeight / 2.0f, 0};
		}
		else
		{
			return center + {-kWidth / 2.0f, +kHeight / 2.0f, 0};
		}
	}

	//判定結果を反映して移動させる

	//④天井に接触しているとき

	//⑤壁に接触しているとき

	//⑥接地判定の切り替え

	//⑦旋回制御

	//⑧行列計算*/
}

void Player::Draw() {
	/// 描画処理///

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}