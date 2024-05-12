#include "pch.h"
#include "Door.h"

Door::Door(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
}

Door::~Door()
{
}

void Door::Open()
{
	SetPos(Vector3 (GetPos().x, GetPos().y + 1.0f, GetPos().z));
}

void Door::Tick(GameData* _GD)
{
	if (targets_hit = target_req && moved < 150.0f)
	{
		Open();
	}
}