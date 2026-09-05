#pragma once

#include <Physics/CollisionDebugStats.h>
#include <Algorithm/QuadTree/QuadTreeBounds.h>

#include <vector>
#include <memory>
#include <unordered_set>

namespace Craft
{
	// 전방 선언
	class Actor;

	// 액터 간 충돌을 테스트하는 클래스
	class QuadTreeCollisionSystem
	{
	private:
		// 충돌 이벤트 발생을 위해 액터를 저장할 구조체
		struct CollisionPair
		{
			std::shared_ptr<Actor> actor;
			std::shared_ptr<Actor> other;

			int leftIndex = -1;
			int rightIndex = -1;
		};

	public:
		QuadTreeCollisionSystem() = default;
		~QuadTreeCollisionSystem() = default;

		// 액터를 순회하면서 충돌을 확인하는 함수
		void ProcessCollision(const std::vector<std::shared_ptr<Actor>>& actorList);

		void SetBroadPhaseMode(CollisionBroadPhaseMode newMode);

		void ToggleBroadPhaseMode();

		inline CollisionBroadPhaseMode GetBroadPhaseMode() const { return broadPhaseMode; }

		// 정확도 비교 ON / OFF
		inline void SetPairValidationEnabled(bool enabled) { pairValidationEnabled = enabled; }

		inline bool IsPairValidationEnabled() const { return pairValidationEnabled; }

		// Stats
		inline const CollisionDebugStats& GetDebugStats() const { return debugStats; }

	private:
		// Broad Phase
		// 기존 O(N²) Pair 생성
		std::vector<CollisionPair> BuildBruteForcePairs(const std::vector<std::shared_ptr<Actor>>& actorList);

		// QuadTree 기반 Pair 생성
		std::vector<CollisionPair> BuildQuadTreePairs(const std::vector<std::shared_ptr<Actor>>& actorList);

		// Narrow Phase
		// 후보 Pair에서 실제 충돌 Pair 생성
		std::vector<CollisionPair> EvaluateCollisionPairs(const std::vector<CollisionPair>& candidatePairs, bool recordStats);

		// 실제 OnCollision 전달
		void DispatchCollisionEvents(const std::vector<CollisionPair>& collidedPairs);

		// Collision Test
		// 두 액터의 CollisionLayer / CollisionMask를 비교해 충돌하도록 설정되어 있는지 확인
		bool ShouldCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);

		// 두 액터가 충돌했는지 확인(테스트)하는 함수
		bool Test(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right);

		// QuadTree Collision Bounds
		// Actor 이동 전/후를 모두 포함하는 Swept Bounds
		QuadTreeBounds BuildSweptBounds(const std::shared_ptr<Actor>& actor) const;

		// 현재 모든 충돌 Actor를 포함하는 QuadTree Root 영역
		QuadTreeBounds BuildQuadTreeWorldBounds(const std::vector<std::shared_ptr<Actor>>& actorList) const;

		// 정확도 비교
		void ValidateQuadTreePairs(const std::vector<std::shared_ptr<Actor>>& actorList);

		// Pair를 비교 가능한 하나의 정수 Key로 변환
		unsigned long long MakePairKey(const CollisionPair& pair) const;

		// Debug / Stats
		void ResetFrameStats();

		void UpdateTiming(double elapsedMs);

		void PrintDebugStats();

	private:
		// 기본값은 기존 방식으로 시작
		CollisionBroadPhaseMode broadPhaseMode = CollisionBroadPhaseMode::BruteForce;

		// 개발 중에는 정확도 검증 ON
		bool pairValidationEnabled = true;

		CollisionDebugStats debugStats;

		// 시간 평균 계산용
		double accumulatedProcessTimeMs = 0.0;

		unsigned long long timingSampleCount = 0;

		// Debug 출력 주기
		unsigned int debugFrameCounter = 0;
	};
}