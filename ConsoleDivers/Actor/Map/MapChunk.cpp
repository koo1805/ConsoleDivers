#include "MapChunk.h"

MapChunk::MapChunk(const Craft::Vector2F& position, const Craft::PixelSprite& chunkSprite)
	: Actor(chunkSprite, position)
{
	sortingOrder = 0;

	SetCollisionMask(Craft::CollisionMaskNone);
}