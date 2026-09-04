#include "CollisionSystem.h"
#include "Actor/Actor.h"

namespace Craft
{
	void CollisionSystem::ProcessCollision(
		const std::vector<std::shared_ptr<Actor>>& actorList)
	{
		// 예외처리
		if (actorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트를 한번에 정리해 전달하기 위한 배열
		std::vector<CollisionPair> collidedActorList;

		// 레벨에 배치된 액터 수
		const int count = static_cast<int>(actorList.size());

		// 모든 액터를 순회하면서 충돌 검사
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

				// Collision Layer / Mask 검사
				// 불필요한 충돌을 여기서 제거
				if (!ShouldCollide(left, right))
				{
					continue;
				}

				// 충돌 검사
				if (Test(left, right))
				{
					// 이벤트 발행할 목록에 추가할 데이터 생성
					CollisionPair pair = {};
					pair.actor = left;
					pair.other = right;

					// 목록에 추가
					collidedActorList.emplace_back(pair);
				}
			}
		}

		// 충돌 발생한 액터 목록 확인. 충돌한 액터가 없으면 함수 종료
		if (collidedActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트 전달
		for (const CollisionPair& pair : collidedActorList)
		{
			// 이미 삭제되거나 비활성화된 액터는 제외
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 수집 중 삭제되거나 비활성화된 액터는 제외
			// 현재 Collision Layer / Mask 재검사
			if (!ShouldCollide(pair.actor, pair.other))
			{
				continue;
			}

			// 충돌 이벤트 전달
			pair.actor->OnCollision(pair.other);

			// 이미 삭제되거나 비활성화된 액터는 제외
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 첫 번째 Collision callback 중에도 Mask가 변경될 수 있으므로 한 번 더 확인
			if (!ShouldCollide(pair.actor, pair.other))
			{
				continue;
			}

			// 충돌 이벤트 전달
			pair.other->OnCollision(pair.actor);
		}
	}

	bool CollisionSystem::ShouldCollide(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
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

	bool CollisionSystem::Test(
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
}