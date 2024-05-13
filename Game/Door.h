#pragma once
#include "CMOGO.h"
#include "GameData.h"

class Door :
    public CMOGO

{
public:
    Door(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, int self_target_req);
    ~Door();

    void Open();
    virtual void Tick(GameData* _GD) override;

    int target_req = 0;
};

