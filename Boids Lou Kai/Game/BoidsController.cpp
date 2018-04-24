#include "BoidsController.h"
#include "BoidsData.h"
#include <ctime>
#include <iostream>
#include <vector>


BoidsController::BoidsController(int _numBoids, Vector3 _pos, BoidsData* _data, ID3D11Device*_pd3dDevice, bool _isPredator)
{
	srand(unsigned int(time(NULL)));
	for (int i = 0; i < _numBoids; i++)
	{
		m_Boids.push_back(new Boids(_pos, _data, _pd3dDevice, _isPredator));


	}
	for (int i = 0; i < PredatorCount; i++)
	{
		m_Boids[i + 1]->SetPredator(true);
		m_Boids[i + 1]->SetScale(3);
		m_Boids[i + 1]->SetPos(Vector3(rand() % 100, rand() % 100, rand() % 100));
	}

	for (int i = 0; i < m_Boids.size(); i++)
	{
		m_Boids[i]->setBoids(m_Boids);
	}

}


BoidsController::~BoidsController()
{

}

void BoidsController::Tick(GameData* _GD)
{
	for (vector<Boids *>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{

		(*it)->Tick(_GD);

	}


}

void BoidsController::Draw(DrawData * _DD)
{
	for (vector<Boids*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		(*it)->Draw(_DD);
	}
}

