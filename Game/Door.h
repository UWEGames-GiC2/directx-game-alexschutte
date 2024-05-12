#pragma once
#include "CMOGO.h"
class Door :
    public CMOGO

{
public:
    Door(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
    ~Door();

    void Open();
    virtual void Tick(GameData* _GD) override;

    int target_req = 3;
    int targets_hit = 0;

protected:
    int moved = 0;
};

