#include "pch.h"
#include "Door.h"

Door::Door(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, int self_target_req) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	target_req = self_target_req;
}

Door::~Door()
{
}

void Door::Open()
{
	SetPos(Vector3 (GetPos().x, GetPos().y + 1.0f, GetPos().z));
	moved++;
}

void Door::Tick(GameData* _GD)
{
	if (targets_hit = target_req && moved < 150.0f)
	{
		Open();
	}

	CMOGO::Tick(_GD);
}