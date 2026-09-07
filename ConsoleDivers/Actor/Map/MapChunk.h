#pragma once

#include <Actor/Actor.h>
#include <Render/Sprite/PixelSprite.h>
#include <Math/Vector2F.h>

class MapChunk : public Craft::Actor
{
	TYPE_DECLARATIONS(MapChunk, Actor)

public:
	MapChunk(const Craft::Vector2F& position, const Craft::PixelSprite& chunkSprite);
	virtual ~MapChunk() override = default;
};