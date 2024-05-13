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
	SetPos(Vector3 (GetPos().x, GetPos().y + 100.0f, GetPos().z));
}

void Door::Tick(GameData* _GD)
{
	CMOGO::Tick(_GD);
}