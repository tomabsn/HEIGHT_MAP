#include <iostream>
#include "ressource/Terrain.h"
using namespace PM3D;

int main() {
	CTerrain terrain(0.03921f, 0.03921f, "image/hills.bmp");
	terrain.CalculerNormale();
	terrain.ConstruireIndex();
	terrain.Sauver("sauvegarde/hills.obj");
}