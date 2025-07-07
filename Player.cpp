#define NOMINMAX

#include "Player.h"
#include <cassert>
#include "MyMath.h"
#include <numbers>
#include <algorithm>
#include "MapChipField.h"

using namespace KamataEngine;
using namespace MathUtility;

//①
void Player::InputMove()
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
	} else {
		// 非入力時は移動減衰をかける
		velocity_.x *= (1.0f - kAttenuation);
	}

	// 接地状態
	if (onGround_)
	{
		// ジャンプ入力
		if (Input::GetInstance()->PushKey(DIK_UP))
		{
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}
	}
	else
	{
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

//②
void Player::CheckMapCollisionUp(CollisionMapInfo& info)
{
	// 上昇あるか
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType{};
	//真上の当たり判定を行う
	bool hit = false;
	//左上点の判定
	MapChipField::IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock)
	{
		hit = true;
	}

	//右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock)
	{
		hit = true;
	}

	//ブロックとの接触判定
	if (hit)
	{
		//めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, kHeight / 2.0f, 0));
		//めり込み先ブロックの範囲選択
		MapChipField::Rect rect = mapChipField_->MapChipField::GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y =std::max(0.0f, (rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank)));
		//天井に当たったことを記録する
		info.hitWall = true;
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info)
{ 	
	CheckMapCollisionUp(info);
}

KamataEngine::Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

//③
void Player::CheckMapMove(const Player::CollisionMapInfo& info)
{
	//移動
	worldTransform_.translation_ += info.move;
}

//④
void Player::CheckMapCeiling(const CollisionMapInfo& info)
{
	// 天井に当たったか
	if (info.landing)
	{
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

//⑦
void Player::AnimateTurn()
{
	// 旋回制御
	if (turnTimer_ > 0.0f)
	{
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

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

void Player::Update()
{
	/// インゲームの更新処理///

	/// ＝＝＝＝＝全体の流れを整理する＝＝＝＝＝///

	/// ＝＝＝＝＝①移動入力＝＝＝＝＝///
	InputMove();

	/// ＝＝＝＝＝②移動量を加味して衝突判定する＝＝＝＝＝///
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	/// ＝＝＝＝＝③判定結果を反映して移動させる＝＝＝＝＝///
	CheckMapMove(collisionMapInfo);

	/// ＝＝＝＝＝④天井に接触しているとき＝＝＝＝＝///
	CheckMapCeiling(collisionMapInfo);

	/// ＝＝＝＝＝⑤壁に接触しているとき＝＝＝＝＝///

	/// ＝＝＝＝＝⑥接地判定の切り替え＝＝＝＝＝///
	//着地フラグ
	bool landing = false;

	 // 下降中
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f)
		{
			landing = true;
		}
	}

	///地面との当たり判定
	// 接地判定
	if (onGround_)
	{
		// ジャンプ開始
		if (velocity_.y > 0.0f)
		{
			// 空中状態に移行
			onGround_ = false;
		}
	}
	else
	{
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度で減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	/// ＝＝＝＝＝⑦旋回制御＝＝＝＝＝///
	AnimateTurn();

	/// ＝＝＝＝＝⑧行列計算＝＝＝＝＝///
	// 行列更新
	worldTransform_.TransferMatrix();

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw()
{
	///描画処理///

	//3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}