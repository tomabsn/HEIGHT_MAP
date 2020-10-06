#include "SommetTerrain.h"

// Definir l’organisation de notre sommet 
//contrat avec notre dispositif
//les problèmes de rendu vienne la pluspart du temps d'ici il faut respecter ce contrat 
D3D11_INPUT_ELEMENT_DESC PM3D::CSommetTerrain::layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

UINT PM3D::CSommetTerrain::numElements = ARRAYSIZE(CSommetTerrain::layout);

PM3D::CSommetTerrain::CSommetTerrain(const XMFLOAT3& _position, const XMFLOAT3& _normal) {
	m_Position = _position; m_Normal = _normal;
}

PM3D::CSommetTerrain::CSommetTerrain(const XMFLOAT3& _position) {
	m_Position = _position;
}