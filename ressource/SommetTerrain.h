#pragma once
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace PM3D {
	class CSommetTerrain {
	public:
		CSommetTerrain() = default;
		CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal);
		CSommetTerrain(const XMFLOAT3& position);
	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];
		XMFLOAT3 m_Position;
		XMFLOAT3 m_Normal;
	};
}