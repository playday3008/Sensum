#pragma once

#include "valve_sdk/csgostructs.hpp"
#include "MaterialManager.h"
#include "singleton.hpp"
#include "IStudioRender.h"
#include <deque>

enum class ChamsModes : int
{
	regular,
	flat,
	wireframe,
	glass,
	reflective,
	crystal_blue,
	metal_gibs,
	shards,
	dev_glow,
	regular_xqz,
	flat_xqz,
	reflective_xqz
};

class Chams : public Singleton<Chams>
{
public:
	void OnSceneEnd();
};
