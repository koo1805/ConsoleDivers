#pragma once

namespace Craft
{
	enum class CollisionBroadPhaseMode
	{
		BruteForce,
		QuadTree
	};

	// BruteForce와 QuadTree CollisionSystem의 성능과 정확도를 비교하기 위한 데이터
	struct CollisionDebugStats
	{
		// 현재 활성 Actor 수
		int actorCount = 0;

		// Broad Phase가 만든 후보 Pair 수
		int candidatePairCount = 0;

		// CollisionLayer / Mask를 통과한 Pair 수
		int layerMaskPassCount = 0;

		// 실제 Swept AABB Test 호출 횟수
		int narrowPhaseTestCount = 0;

		// 실제 충돌한 Pair 수
		int collisionCount = 0;

		// 현재 프레임 CollisionSystem 처리 시간
		double processTimeMs = 0.0;

		// 현재 모드에서 누적 평균
		double averageProcessTimeMs = 0.0;

		// 현재 모드에서 가장 오래 걸린 프레임
		double maxProcessTimeMs = 0.0;

		// 측정 프레임 수
		unsigned long long sampleCount = 0;

		// BruteForce | QuadTree 정확도 비교
		// BruteForce에서는 잡혔는데 QuadTree에서는 누락된 실제 충돌 Pair
		int missingCollisionPairCount = 0;

		// QuadTree에서만 실제 충돌로 잡힌 Pair
		int extraCollisionPairCount = 0;

		// 두 결과가 완전히 같은가?
		bool validationPassed = true;
	};
}