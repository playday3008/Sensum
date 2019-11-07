#pragma once

#include "valve_sdk/csgostructs.hpp"
#include "MaterialManager.h"
#include "singleton.hpp"
#include "IStudioRender.h"
#include <deque>

enum class ChamsModes : int
{
	NORMAL,
	FLAT,
	WIREFRAME,
	GLASS,
	METALLIC,
	XQZ,
	METALLIC_XQZ,
	FLAT_XQZ
};

class Chams : public Singleton<Chams>
{
public:
	void OnSceneEnd();
	void OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags);
	void DMEChams(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags);
private:
	IMaterial* materialRegular = nullptr;
	IMaterial* materialRegularIgnoreZ = nullptr;
	IMaterial* materialFlat = nullptr;
	IMaterial* materialFlatIgnoreZ = nullptr;
	IMaterial* materialReflective = nullptr;
	IMaterial* materialReflectiveIgnoreZ = nullptr;
	IMaterial* materialGlow = nullptr;
	IMaterial* materialGlowIgnoreZ = nullptr;
};

