#include "Terrain.h"
#include "CImg.h"
#include <fstream>
#include <vector>
#include <cstdlib>
using namespace cimg_library;
using namespace std;

namespace PM3D {

	CTerrain::CTerrain(const float dxy,const float dz, const char* image)  {
		const CImg<float> img(image);
		width = img.width();
		height = img.height();

		nbrSommets = width * height;
		nbrPolygones = (width - 1) * (height - 1) * 2;
		sommets = new CSommetTerrain[nbrSommets];

		for (int y = 0; y != height; ++y) {
			for (int x = 0; x != width; ++x) {
				sommets[width * y + x] = CSommetTerrain(XMFLOAT3(x * dxy, y * dxy, -img.atXY(x, height - y) * dz));
				//-img.atXY(x, height - y)
				//le "-" car mon z sera négatif en main gauche
				//height - y pour garder le (0,0) en haut à gauche
			}
		}
	}

	void CTerrain::CalculerNormale() {

		/*
						p1    p2
						|    /
						|   /
						|  /
						| /
				p6-----p1----p3
					   /|
					  / |
					 /	|
					/	|
					p5	p4

		On tourne dans le sens horraire donc p2 puis p3 puis p4 etc...
		*/

		for (int y = 0; y != height; ++y) {
			for (int x = 0; x != width; ++x) {
				XMVECTOR n1, n2, n3, n4, n5, n6;
				XMVECTOR v1, v2, v3, v4, v5, v6;

				XMFLOAT3 p0 = ObtenirPosition(x, y);
				XMFLOAT3 p1;
				XMFLOAT3 p2;
				XMFLOAT3 p3;
				XMFLOAT3 p4;
				XMFLOAT3 p5;
				XMFLOAT3 p6;

				v1 = v2 = v3 = v4 = v5 = v6 = XMVectorSet(0, 0, 0, 0);

				//notre normal sera négative (main gauche donc elle doit venir vers "nous")
				n1 = n2 = n3 = n4 = n5 = n6 = XMVectorSet(0, 0, -1, 0);

				//calcul de vecteur
				if (y < height - 1) {
					p1 = ObtenirPosition(x, y + 1);
					v1 = XMVectorSet(abs(p1.x - p0.x), abs(p1.y - p0.y), abs(p1.z - p0.z), 0);
				}
				if (y < height - 1 && x < width - 1) {
					p2 = ObtenirPosition(x + 1, y + 1);
					v2 = XMVectorSet(abs(p2.x - p0.x), abs(p2.y - p0.y), abs(p2.z - p0.z), 0);
				}
				if (x < width - 1) {
					p3 = ObtenirPosition(x + 1, y);
					v3 = XMVectorSet(abs(p3.x - p0.x), abs(p3.y - p0.y), abs(p3.z - p0.z), 0);
				}
				if (y > 0) {
					p4 = ObtenirPosition(x, y - 1);
					v4 = XMVectorSet(abs(p4.x - p0.x), abs(p4.y - p0.y), abs(p4.z - p0.z), 0);
				}
				if (y > 0 && x > 0) {
					p5 = ObtenirPosition(x - 1, y - 1);
					v5 = XMVectorSet(abs(p5.x - p0.x), abs(p5.y - p0.y), abs(p5.z - p0.z), 0);
				}
				if (x < width - 1 && y > 0) {
					p6 = ObtenirPosition(x + 1, y - 1);
					v6 = XMVectorSet(abs(p6.x - p0.x), abs(p6.y - p0.y), abs(p6.z - p0.z), 0);
				}


				//calcul de produit scalaire
				if (y < height - 1 && x < width - 1) {
					n1 = XMVector3Cross(v1, v2);
					n2 = XMVector3Cross(v2, v3);
				}
				if (y > 0 && x < width - 1) {
					n3 = XMVector3Cross(v3, v4);
				}
				if (y > 0 && x > 0) {
					n4 = XMVector3Cross(v4, v5);
					n5 = XMVector3Cross(v5, v6);
				}
				if (y < height - 1 && x > 0) {
					n6 = XMVector3Cross(v6, v1);
				}

				n1 = n2 + n3 + n4 + n5 + n6 + n1;

				n1 = XMVector3Normalize(n1);
				XMFLOAT3 resultat;
				XMStoreFloat3(&resultat, n1);

				sommets[width * y + x].m_Normal = resultat;
			}
		}
	}

	void CTerrain::ConstruireIndex() {

		/*
			p2---p3			 p3
			|	/		   /  |
			|  /		  /	  |
			p1			p1---p4

		Sens horaire !
		On fait donc [p1,p2,p3] premier triangle puis [p1,p3,p4] deuxième triangle
		*/
		pIndices = new unsigned int[nbrPolygones * 3];
		int k = 0;
		for (int y = 0; y != height - 1; ++y) {
			for (int x = 0; x != width - 1; ++x) {
				pIndices[k++] = y * width + x; //p1
				pIndices[k++] = (y + 1) * width + x; //p2
				pIndices[k++] = (y + 1) * width + (x + 1); //p3
				pIndices[k++] = y * width + x; //p1
				pIndices[k++] = (y + 1) * width + (x + 1); //p3
				pIndices[k++] = y * width + (x + 1); //p4

			}
		}
	}

	void CTerrain::Sauver(const char* name) {
		ofstream file;
		file.open(name);
		if (file.is_open()) {
			for (int i = 0; i != nbrSommets; ++i) {
				file << "v " << sommets[i].m_Position.x << " " << sommets[i].m_Position.y << " " << sommets[i].m_Position.z << "\n";
			}
			file << " \n";
			for (int i = 0; i != nbrSommets; ++i) {
				file << "vn " << sommets[i].m_Normal.x << " " << sommets[i].m_Normal.y << " " << sommets[i].m_Normal.z << "\n";
			}
			file << " \n";
			for (int j = 0; j < nbrPolygones * 3 - 3; j += 3) {
				//f v1 / vt1 / vn1 v2 / vt2 / vn2 v3 / vt3 / vn3
				file << "f " << pIndices[j] + 1 << "//" << pIndices[j] + 1 << " ";
				file << pIndices[j + 1] + 1 << "//" << pIndices[j + 1] + 1 << " ";
				file << pIndices[j + 2] + 1 << "//" << pIndices[j + 2] + 1 << " ";
				file << '\n';
			}
		}
		file.close();
	}

	CTerrain::~CTerrain() {

	}

	XMFLOAT3 CTerrain::ObtenirPosition(int x, int y) {
		return sommets[width * y + x].m_Position;
	}
}