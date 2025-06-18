#include "CameraController.h"
#include "KamataEngine.h"
#include "Player.h"
#include <cmath>

using namespace KamataEngine;
using namespace MathUtility;

void CameraController::Intialize()
{
	//カメラの初期化
	camera_.Initialize();
}

static inline const CameraController::Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

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
	/*camera_.translation_.x = max(camera_.translation_.x, targetPosition_.x + targetMargin.left);
	camera_.translation_.x = max(camera_.translation_.x, targetPosition_.x + targetMargin.right);
	camera_.translation_.y = max(camera_.translation_.y, targetPosition_.y + targetMargin.bottom);
	camera_.translation_.y = max(camera_.translation_.y, targetPosition_.y + targetMargin.top);*/

	// 3. マージン補正（左右）
	if (camera_.translation_.x < targetPosition_.x + targetMargin.left) {
		camera_.translation_.x = targetPosition_.x + targetMargin.left;
	} else if (camera_.translation_.x > targetPosition_.x + targetMargin.right) {
		camera_.translation_.x = targetPosition_.x + targetMargin.right;
	}

	// 4. マージン補正（上下）
	if (camera_.translation_.y < targetPosition_.y + targetMargin.bottom) {
		camera_.translation_.y = targetPosition_.y + targetMargin.bottom;
	} else if (camera_.translation_.y > targetPosition_.y + targetMargin.top) {
		camera_.translation_.y = targetPosition_.y + targetMargin.top;
	}

	// 移動範囲宣言
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);

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