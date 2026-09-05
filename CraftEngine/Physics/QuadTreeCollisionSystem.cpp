#include "QuadTreeCollisionSystem.h"
#include "Actor/Actor.h"
#include <Algorithm/QuadTree/QuadTree.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>

namespace Craft
{
	void QuadTreeCollisionSystem::ProcessCollision(const std::vector<std::shared_ptr<Actor>>& actorList)
	{
		ResetFrameStats();

		// 활성 Actor 수 측정
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (actor && actor->IsActive())
			{
				++debugStats.actorCount;
			}
		}

		// 예외처리
		if (actorList.empty())
		{
			return;
		}

		// 정확도 검증
		if (pairValidationEnabled)
		{
			ValidateQuadTreePairs(actorList);
		}

		// 실제 CollisionSystem 측정 시작
		const auto begin = std::chrono::steady_clock::now();
		std::vector<CollisionPair> candidates;

		// 선택된 Broad Phase
		switch (broadPhaseMode)
		{
		case CollisionBroadPhaseMode::BruteForce:
			candidates = BuildBruteForcePairs(actorList);
			break;

		case CollisionBroadPhaseMode::QuadTree:
			candidates = BuildQuadTreePairs(actorList);
			break;
		}

		// Narrow Phase
		const std::vector<CollisionPair> collidedPairs = EvaluateCollisionPairs(candidates, true);

		// 실제 Collision Event
		DispatchCollisionEvents(collidedPairs);

		const auto end = std::chrono::steady_clock::now();

		const double elapsedMs = std::chrono::duration<double, std::milli>(end - begin).count();

		UpdateTiming(elapsedMs);

		// Debug 출력
		PrintDebugStats();
	}

	void QuadTreeCollisionSystem::SetBroadPhaseMode(CollisionBroadPhaseMode newMode)
	{
		if (broadPhaseMode == newMode)
		{
			return;
		}

		broadPhaseMode = newMode;

		// 서로 다른 Mode의 시간 데이터가 섞이지 않도록 모드 변경 시 누적 측정 초기화
		accumulatedProcessTimeMs = 0.0;
		timingSampleCount = 0;

		debugStats.averageProcessTimeMs = 0.0;
		debugStats.maxProcessTimeMs = 0.0;
		debugStats.sampleCount = 0;
	}

	void QuadTreeCollisionSystem::ToggleBroadPhaseMode()
	{
		if (broadPhaseMode == CollisionBroadPhaseMode::BruteForce)
		{
			SetBroadPhaseMode(CollisionBroadPhaseMode::QuadTree);
		}
		else
		{
			SetBroadPhaseMode(CollisionBroadPhaseMode::BruteForce);
		}
	}

	std::vector<QuadTreeCollisionSystem::CollisionPair> QuadTreeCollisionSystem::BuildBruteForcePairs(const std::vector<std::shared_ptr<Actor>>& actorList)
	{
		std::vector<CollisionPair> pairs;

		const int count = static_cast<int>(actorList.size());

		for (int ix = 0; ix < count; ++ix)
		{
			const std::shared_ptr<Actor>& left = actorList[ix];

			if (!left || !left->IsActive())
			{
				continue;
			}

			for (int jx = ix + 1; jx < count; ++jx)
			{
				const std::shared_ptr<Actor>& right = actorList[jx];

				if (!right || !right->IsActive())
				{
					continue;
				}

				CollisionPair pair;

				pair.actor = left;
				pair.other = right;

				pair.leftIndex = ix;
				pair.rightIndex = jx;

				pairs.emplace_back(pair);
			}
		}

		return pairs;
	}

	std::vector<QuadTreeCollisionSystem::CollisionPair> QuadTreeCollisionSystem::BuildQuadTreePairs(const std::vector<std::shared_ptr<Actor>>& actorList)
	{
		std::vector<CollisionPair> pairs;

		if (actorList.empty())
		{
			return pairs;
		}

		// 1. 현재 모든 Actor를 포함하는 Root Bounds 계산
		const QuadTreeBounds worldBounds = BuildQuadTreeWorldBounds(actorList);

		// 2. Collision 전용 QuadTree 생성
		// GameLevel에 있는 Debug용 QuadTree와는 별개 CollisionSystem 내부 Broad Phase용 임시 트리
		QuadTree tree(worldBounds, 4);

		// Actor* -> actorList index
		std::unordered_map<const Actor*, int> actorIndexMap;

		actorIndexMap.reserve(actorList.size());

		// 3. 모든 Actor Swept Bounds Insert
		for (int index = 0; index < static_cast<int>(actorList.size()); ++index)
		{
			const std::shared_ptr<Actor>& actor = actorList[index];

			if (!actor || !actor->IsActive())
			{
				continue;
			}

			const QuadTreeBounds sweptBounds = BuildSweptBounds(actor);

			if (sweptBounds.width <= 0.0f || sweptBounds.height <= 0.0f)
			{
				continue;
			}

			if (!tree.Insert(actor, sweptBounds))
			{
				continue;
			}

			actorIndexMap.emplace(actor.get(), index);
		}

		// 4. 각 Actor Swept Bounds 주변 후보 검색
		for (int ix = 0; ix < static_cast<int>(actorList.size()); ++ix)
		{
			const std::shared_ptr<Actor>& left = actorList[ix];

			if (!left || !left->IsActive())
			{
				continue;
			}

			const QuadTreeBounds leftBounds = BuildSweptBounds(left);

			if (leftBounds.width <= 0.0f || leftBounds.height <= 0.0f)
			{
				continue;
			}

			const std::vector<std::shared_ptr<Actor>> nearby = tree.Query(leftBounds);


			for (const std::shared_ptr<Actor>& right : nearby)
			{
				if (!right || right.get() == left.get())
				{
					continue;
				}

				auto found = actorIndexMap.find(right.get());

				if (found == actorIndexMap.end())
				{
					continue;
				}

				const int jx = found->second;

				// BruteForce와 동일하게 ix < jx 인 Pair만 생성
				// // 이 한 줄로 A-B / B-A 중복 제거
				if (jx <= ix)
				{
					continue;
				}

				CollisionPair pair;

				pair.actor = left;
				pair.other = right;

				pair.leftIndex = ix;
				pair.rightIndex = jx;

				pairs.emplace_back(pair);
			}
		}

		// Query 반환 순서는 QuadTree 내부 구조에 따라 달라질 수 있음
		// 기존 BruteForce와 같은 Actor 순서를 보장해서 OnCollision 발생 순서 차이를 최대한 제거
		std::sort(pairs.begin(), pairs.end(), [](const CollisionPair& left, const CollisionPair& right)
			{
				if (left.leftIndex != right.leftIndex)
				{
					return left.leftIndex < right.leftIndex;
				}

				return left.rightIndex < right.rightIndex;
			});

		return pairs;
	}

	std::vector<QuadTreeCollisionSystem::CollisionPair> QuadTreeCollisionSystem::EvaluateCollisionPairs(const std::vector<CollisionPair>& candidatePairs, bool recordStats)
	{
		std::vector<CollisionPair> collidedPairs;

		if (recordStats)
		{
			debugStats.candidatePairCount = static_cast<int>(candidatePairs.size());
		}

		for (const CollisionPair& pair : candidatePairs)
		{
			if (!pair.actor || !pair.other || !pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// Collision Layer / Mask
			if (!ShouldCollide(pair.actor, pair.other))
			{
				continue;
			}

			if (recordStats)
			{
				++debugStats.layerMaskPassCount;
			}

			// 실제 Swept AABB 검사
			if (recordStats)
			{
				++debugStats.narrowPhaseTestCount;
			}

			if (!Test(pair.actor, pair.other))
			{
				continue;
			}

			if (recordStats)
			{
				++debugStats.collisionCount;
			}

			collidedPairs.emplace_back(pair);
		}

		return collidedPairs;
	}

	void QuadTreeCollisionSystem::DispatchCollisionEvents(const std::vector<CollisionPair>& collidedPairs)
	{
		for (const CollisionPair& pair : collidedPairs)
		{
			if (!pair.actor || !pair.other)
			{
				continue;
			}

			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// Callback 직전 Mask 재검사
			if (!ShouldCollide(pair.actor, pair.other))
			{
				continue;
			}

			pair.actor->OnCollision(pair.other);

			// 첫 Callback에서 파괴됐을 수 있음
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 첫 Callback에서 Mask 변경 가능
			if (!ShouldCollide(pair.actor, pair.other))
			{
				continue;
			}

			pair.other->OnCollision(pair.actor);
		}
	}

	bool QuadTreeCollisionSystem::ShouldCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}

		// left -> right 검사
		// left의 CollisionMask 안에 right의 CollisionLayer가 포함되어 있는지 검사
		const bool leftAllowRight = HasCollisionLayer(left->GetCollisionMask(), right->GetCollisionLayer());

		// right -> left 검사
		// 반대쪽에서도 충돌하도록 설정되어 있어야 함
		const bool rightAllowLeft = HasCollisionLayer(right->GetCollisionMask(), left->GetCollisionLayer());

		// 양쪽 Actor가 서로 충돌을 허용할 때만 실제 AABB검사 수행
		return leftAllowRight && rightAllowLeft;
	}

	bool QuadTreeCollisionSystem::Test(
		const std::shared_ptr<Actor>& left,
		const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}

		// 충돌 영역 크기
		const float leftWidth = static_cast<float>(left->GetWidth());
		const float leftHeight = static_cast<float>(left->GetHeight());
		const float rightWidth = static_cast<float>(right->GetWidth());
		const float rightHeight = static_cast<float>(right->GetHeight());

		// 크기가 없는 Actor는 충돌 영역도 없다고 판단
		// Actor->position만 사용한다면 잘못충돌하는 것을 방지
		if (leftWidth <= 0.0f || leftHeight <= 0.0f || rightWidth <= 0.0f || rightHeight <= 0.0f)
		{
			return false;
		}

		// ================= AABB (Axis Aligned Bounding Box) =============================
		

		// left 액터의 현재/이전 위치
		const Vector2F leftCurrent = left->GetPosition();
		const Vector2F leftPrevious = left->GetPreviousPosition();

		// right 액터의 현재/이전 위치
		const Vector2F rightCurrent = right->GetPosition();
		const Vector2F rightPrevious = right->GetPreviousPosition();

		// LEFT Actor Swept AABB
		// 이전 프레임 위치와 현재 위치를 모두 포함하는 swept bounds 계산
		// 이동 전/후 위치 중 더 작은 값을 시작점으로 사용
		const float leftXMin = (leftCurrent.x < leftPrevious.x) ? leftCurrent.x : leftPrevious.x;
		const float leftYMin = (leftCurrent.y < leftPrevious.y) ? leftCurrent.y : leftPrevious.y;

		// 현재 위치에서의 오른쪽/아래쪽 끝
		const float leftXMaxCurrent = leftCurrent.x + leftWidth;
		const float leftYMaxCurrent = leftCurrent.y + leftHeight;

		// 이전 위치에서의 오른쪽/아래쪽 끝
		const float leftXMaxPrevious = leftPrevious.x + leftWidth;
		const float leftYMaxPrevious = leftPrevious.y + leftHeight;

		// 이동 전/후를 전부 포함하는 최대 영역
		const float leftXMax = (leftXMaxCurrent > leftXMaxPrevious) ? leftXMaxCurrent : leftXMaxPrevious;
		const float leftYMax = (leftYMaxCurrent > leftYMaxPrevious) ? leftYMaxCurrent : leftYMaxPrevious;

		// RIGHT Actor Swept AABB
		const float rightXMin = (rightCurrent.x < rightPrevious.x) ? rightCurrent.x : rightPrevious.x;
		const float rightYMin = (rightCurrent.y < rightPrevious.y) ? rightCurrent.y : rightPrevious.y;

		const float rightXMaxCurrent = rightCurrent.x + rightWidth;
		const float rightYMaxCurrent = rightCurrent.y + rightHeight;

		const float rightXMaxPrevious = rightPrevious.x + rightWidth;
		const float rightYMaxPrevious = rightPrevious.y + rightHeight;

		const float rightXMax = (rightXMaxCurrent > rightXMaxPrevious) ? rightXMaxCurrent : rightXMaxPrevious;
		const float rightYMax = (rightYMaxCurrent > rightYMaxPrevious) ? rightYMaxCurrent : rightYMaxPrevious;

		// X좌표 기준으로 충돌이 발생할 수 없는 상황 처리 - X축 분리 검사
		// left가 right보다 완전히 왼쪽
		if (rightXMin >= leftXMax)
		{
			return false;
		}

		
		if (rightXMax <= leftXMin)
		{
			return false;
		}

		// y좌표 기준으로 충돌이 발생할 수 없는 상황 처리 - Y축 분리 검사
		// right가 left보다 완전히 왼쪽
		if (rightYMin >= leftYMax)
		{
			return false;
		}

		// right가 left보다 완전히 위쪽
		if (rightYMax <= leftYMin)
		{
			return false;
		}

		// X/Y 어느 축에서도 분리되지 않았으므로 Swept AABB가 서로 겹침
		// 충돌 발생
		return true;
	}

	QuadTreeBounds QuadTreeCollisionSystem::BuildSweptBounds(const std::shared_ptr<Actor>& actor) const
	{
		if (!actor)
		{
			return QuadTreeBounds();
		}

		const float width = static_cast<float>(actor->GetWidth());

		const float height = static_cast<float>(actor->GetHeight());

		if (width <= 0.0f || height <= 0.0f)
		{
			return QuadTreeBounds();
		}

		const Vector2F current = actor->GetPosition();

		const Vector2F previous = actor->GetPreviousPosition();

		// 이동 시작/끝 중 더 작은 쪽이 Swept Bounds 시작점
		const float minX = (current.x < previous.x) ? current.x : previous.x;

		const float minY = (current.y < previous.y) ? current.y : previous.y;

		// 현재 위치의 끝
		const float currentMaxX = current.x + width;

		const float currentMaxY = current.y + height;

		// 이전 위치의 끝
		const float previousMaxX = previous.x + width;

		const float previousMaxY = previous.y + height;

		// 이동 시작/끝을 모두 포함하는 최대 영역
		const float maxX = (currentMaxX > previousMaxX) ? currentMaxX : previousMaxX;

		const float maxY = (currentMaxY > previousMaxY) ? currentMaxY : previousMaxY;

		return QuadTreeBounds(minX, minY, maxX - minX, maxY - minY);
	}

	QuadTreeBounds QuadTreeCollisionSystem::BuildQuadTreeWorldBounds(const std::vector<std::shared_ptr<Actor>>& actorList) const
	{
		bool hasValidActor = false;

		float minX = 0.0f;
		float minY = 0.0f;

		float maxX = 0.0f;
		float maxY = 0.0f;

		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor || !actor->IsActive())
			{
				continue;
			}

			const QuadTreeBounds actorBounds = BuildSweptBounds(actor);

			if (actorBounds.width <= 0.0f || actorBounds.height <= 0.0f)
			{
				continue;
			}

			if (!hasValidActor)
			{
				minX = actorBounds.x;
				minY = actorBounds.y;

				maxX = actorBounds.GetMaxX();
				maxY = actorBounds.GetMaxY();

				hasValidActor = true;

				continue;
			}

			minX = (std::min)(minX, actorBounds.x);
			minY = (std::min)(minY, actorBounds.y);

			maxX = (std::max)(maxX, actorBounds.GetMaxX());

			maxY = (std::max)(maxY, actorBounds.GetMaxY());
		}

		if (!hasValidActor)
		{
			return QuadTreeBounds(0.0f, 0.0f, 1.0f, 1.0f);
		}

		// 경계 오차 방지를 위해 사방에 1칸 Padding.
		const float padding = 1.0f;

		return QuadTreeBounds(minX - padding, minY - padding, (maxX - minX) + padding * 2.0f, (maxY - minY) + padding * 2.0f);
	}

	void QuadTreeCollisionSystem::ValidateQuadTreePairs(const std::vector<std::shared_ptr<Actor>>& actorList)
	{
		// 실제 이벤트는 발생시키지 않음
		// 두 방식의 Collision 결과만 계산해서 비교

		const std::vector<CollisionPair> bruteCandidates = BuildBruteForcePairs(actorList);

		const std::vector<CollisionPair> quadCandidates = BuildQuadTreePairs(actorList);


		// Stats 기록 없이 실제 충돌만 계산
		const std::vector<CollisionPair> bruteCollisions = EvaluateCollisionPairs(bruteCandidates, false);

		const std::vector<CollisionPair> quadCollisions = EvaluateCollisionPairs(quadCandidates, false);

		std::unordered_set<unsigned long long> bruteSet;

		std::unordered_set<unsigned long long> quadSet;


		for (const CollisionPair& pair : bruteCollisions)
		{
			bruteSet.emplace(MakePairKey(pair));
		}

		for (const CollisionPair& pair : quadCollisions)
		{
			quadSet.emplace(MakePairKey(pair));
		}

		int missing = 0;
		int extra = 0;

		// BruteForce에는 있는데 QuadTree에는 없는 Pair -> 이 값은 반드시 0
		for (const auto key : bruteSet)
		{
			if (quadSet.find(key) == quadSet.end())
			{
				++missing;
			}
		}

		// QuadTree에서만 존재하는 실제 Collision
		for (const auto key : quadSet)
		{
			if (bruteSet.find(key) == bruteSet.end())
			{
				++extra;
			}
		}

		debugStats.missingCollisionPairCount = missing;

		debugStats.extraCollisionPairCount = extra;

		debugStats.validationPassed = (missing == 0 && extra == 0);
	}

	unsigned long long QuadTreeCollisionSystem::MakePairKey(const CollisionPair& pair) const
	{
		const unsigned long long left = static_cast<unsigned long long>(static_cast<unsigned int>(pair.leftIndex));

		const unsigned long long right =static_cast<unsigned long long>(static_cast<unsigned int>(pair.rightIndex));

		// 상위 32bit = left index
		// 하위 32bit = right index
		return (left << 32) | right;
	}

	void QuadTreeCollisionSystem::ResetFrameStats()
	{
		debugStats.actorCount = 0;

		debugStats.candidatePairCount = 0;
		debugStats.layerMaskPassCount = 0;
		debugStats.narrowPhaseTestCount = 0;
		debugStats.collisionCount = 0;

		debugStats.processTimeMs = 0.0;

		debugStats.missingCollisionPairCount = 0;
		debugStats.extraCollisionPairCount = 0;

		debugStats.validationPassed = true;
	}

	void QuadTreeCollisionSystem::UpdateTiming(double elapsedMs)
	{
		debugStats.processTimeMs = elapsedMs;

		accumulatedProcessTimeMs += elapsedMs;

		++timingSampleCount;

		debugStats.sampleCount = timingSampleCount;

		debugStats.averageProcessTimeMs = accumulatedProcessTimeMs / static_cast<double>(timingSampleCount);

		if (elapsedMs > debugStats.maxProcessTimeMs)
		{
			debugStats.maxProcessTimeMs = elapsedMs;
		}
	}

	void QuadTreeCollisionSystem::PrintDebugStats()
	{
		++debugFrameCounter;

		// 120프레임에 한 번 출력
		if (debugFrameCounter < 120)
		{
			return;
		}

		debugFrameCounter = 0;


		const char* modeName = (broadPhaseMode == CollisionBroadPhaseMode::BruteForce) ? "BruteForce" : "QuadTree";

		char buffer[512] = {};

		sprintf_s(buffer, sizeof(buffer),

			"\n"
			"[Collision] Mode: %s\n"
			" Actors       : %d\n"
			" Candidates   : %d\n"
			" LayerPass    : %d\n"
			" AABB Tests   : %d\n"
			" Collisions   : %d\n"
			" Current      : %.4f ms\n"
			" Average      : %.4f ms\n"
			" Max          : %.4f ms\n"
			" Samples      : %llu\n"
			" Validation   : %s\n"
			" Missing      : %d\n"
			" Extra        : %d\n",

			modeName,

			debugStats.actorCount,
			debugStats.candidatePairCount,
			debugStats.layerMaskPassCount,
			debugStats.narrowPhaseTestCount,
			debugStats.collisionCount,

			debugStats.processTimeMs,
			debugStats.averageProcessTimeMs,
			debugStats.maxProcessTimeMs,
			debugStats.sampleCount,

			debugStats.validationPassed ? "PASS" : "FAIL",

			debugStats.missingCollisionPairCount,
			debugStats.extraCollisionPairCount);


		OutputDebugStringA(buffer);
	}
}