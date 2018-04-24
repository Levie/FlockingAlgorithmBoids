#pragma once
#include "BoidsController.h"
#include "gameobject.h"
#include <vector>
#include "Boids.h"

class Boids;
struct BoidsData;
class BoidsController : public GameObject
{
public:
	BoidsController(int _numBoids, Vector3 _pos, BoidsData* _data, ID3D11Device* _pd3dDevice, bool _ispredator);
	~BoidsController();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;

protected:
	vector<Boids*> m_Boids;
	float  count = 0.0f;


	Vector3 CentreMass = Vector3::Zero;
	Vector3 Velocity = Vector3::Zero;
	Vector3 m_seperation;
	Vector3 seperation_radius;
	int PredatorCount = 2;

	

};