#include "CameraController.h"
#include "KamataEngine.h"
#include "Player.h"
#include <cmath>

using namespace KamataEngine;
using namespace MathUtility;

void CameraController::Intialize(/*Camera* camera*/) {
	/// インゲームの初期化処理///

	// 引数として受け取ったデータをメンバ変数に記録する
	//camera_ = camera;

	// カメラの初期化
	camera_.Initialize();
}

void CameraController::Update()
{
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	const Vector3& targetVelocity = target_->GetVelocity();
	targetPosition_= targetWorldTransform.translation_ + targetOffset_+targetVelocity*kVelocityBias;

	//座標補間によりゆったり追従
	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
	camera_.translation_.y = Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);

	//追従対象が画面外に出ないように修正
	camera_.translation_.x = max(camera_.translation_.x, targetPosition_.x + targetMargin.left);
	camera_.translation_.x = max(camera_.translation_.x, targetPosition_.x + targetMargin.right);
	camera_.translation_.y = max(camera_.translation_.y, targetPosition_.y + targetMargin.bottom);
	camera_.translation_.y = max(camera_.translation_.y, targetPosition_.y + targetMargin.top);

	// 移動範囲宣言
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);

	/*/追従対象のワールドトランスフォームを参照する
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ * targetOffset_;*/

	// 行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Reset()
{
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}