#pragma once

namespace Features::Resources {

void Init();
void Tick();
int GetScanFound();
int GetScanTotal();

void SetGMP(int value);
void SetHeroism(int value);
void SetDemonPoints(int value);
void MaxAllPlants();
void MaxAllMaterials();

} // namespace Features::Resources
