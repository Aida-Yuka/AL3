#pragma once

#include "KamataEngine.h"

KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);

// AABB
struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
};

struct Rect {
	float left, top;
	float right, bottom;
};

//イージング
float EaseInOut(float x1, float x2, float t);

//衝突判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);