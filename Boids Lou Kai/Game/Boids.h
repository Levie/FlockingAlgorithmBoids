#pragma once
#include "CMOGO.h"
#include "VBGO.h"
#include "vertex.h"
#include "Effects.h"
#include "BoidsData.h"

class Boids : public VBGO
{
public:
	
	Boids(Vector3 _pos, BoidsData* _data, ID3D11Device * _pd3dDevice, bool ispredator);
	~Boids();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	void setBoids(vector<Boids*> _boids);
	Vector3 GetVelocity() { return m_velocity; }
	void SetVelocity(Vector3 _vel) { m_velocity = _vel; }
	Vector3 Seperation();
	Vector3 Align();
	Vector3 Cohesion();
	Vector3 Seek(Vector3 _target);
	Vector3 PredatorSeek();
	Vector3 Escape();
	void Bounds();
	void ApplyRules();

protected:

	vector<Boids*> m_boids;
	bool m_move;
	Vector3 m_target;
	Vector3 m_dir;
	Vector3 m_velocity;
	Vector3 m_up;
	int m_size;
	bool m_alive;
	myVertex* m_vertices;
	int sizeX = 10;
	float CheckDist = 0;
	float closestBoid = 0;
	float closest = 100;
	BoidsData* m_BoidData;
	bool ispredator = false;
	bool isprey = false;
};