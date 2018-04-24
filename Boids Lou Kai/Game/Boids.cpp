#include "Boids.h"
#include "BoidsController.h"
#include <iostream>
#include "CMOGO.h"
#include <stdlib.h>
#include "gameobject.h"
#include <dinput.h>
#include "GameData.h"
#include <SimpleMath.h>

Boids::Boids(Vector3 _pos, BoidsData* _data, ID3D11Device * _pd3dDevice, bool _ispredator) : VBGO()
{
	//boid set up
	
	int m_size = 6;
	m_BoidData = _data;

	m_pos = Vector3(rand() % 100, rand() % 100, rand() % 100);
	

	//calculate number of vertices and primatives
	int numVerts = 12;
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	////as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	//top
	m_vertices[0].Color = Color(0.0f, 5.0f, 15.0f, 3.0f);
	m_vertices[0].Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_vertices[1].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[1].Pos = Vector3(0.0f, 0.0f, 2.0f);
	m_vertices[2].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[2].Pos = Vector3(2.0f, -0.5f, 1.0f);

	//back
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[3].Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[4].Pos = Vector3(0.0f, 0.0f, 2.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[5].Pos = Vector3(0.0f, -1.0f, 1.0f);

	//right
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[6].Pos = Vector3(0.0f, 0.0f, 2.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[7].Pos = Vector3(0.0f, -1.0f, 1.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[8].Pos = Vector3(2.0f, -0.5f, 1.0f);

	//left
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[9].Pos = Vector3(0.0f, 0.0f, 0.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[10].Pos = Vector3(0.0f, -1.0f, 1.0f);
	m_vertices[2].Color = Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[11].Pos = Vector3(2.0f, -0.5f, 1.0f);

	for (int i = 0; i < m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(_pd3dDevice, indices);
	BuildVB(_pd3dDevice, numVerts, m_vertices);

	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	m_vertices = nullptr;

	float theNumber = 0.1 + (rand() % (int)(359 - 0.1 + 1));
	m_pos = Vector3((float)(rand() % sizeX - _pos.x) + _pos.x, (float)(rand() % sizeX - _pos.y) + _pos.y, (float)(rand() % sizeX - _pos.z) + _pos.z);
	m_vel = Vector3(cos(theNumber), cos(theNumber), sin(theNumber));
	m_up = Vector3::Transform(Vector3::Up, m_fudge.Invert() * m_worldMat) - m_pos;
}

Boids::~Boids()
{
	for (int i = 0; i < m_boids.size(); i++)
	{
		m_boids[i] = nullptr;
	}
}

void Boids::Tick(GameData * _GD)
{
	ApplyRules();
	Bounds();
	m_vel += m_acc * _GD->m_dt;
	m_vel = XMVector3ClampLength(m_vel, m_BoidData->MinSpeed, m_BoidData->MaxSpeed);
	m_pos += m_vel;
	m_acc = Vector3::Zero;
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	Matrix  transMat = Matrix::CreateTranslation(m_pos);
	Matrix rotTransMat = Matrix::CreateWorld(m_pos, m_vel, Vector3::Up);
	m_rotMat = Matrix::CreateWorld(m_pos, m_vel, Vector3::Up);
	m_worldMat = m_fudge * scaleMat * rotTransMat;

}

void Boids::Draw(DrawData * _DD)
{

	VBGO::Draw(_DD);

}
void Boids::setBoids(vector<Boids*> _boids)
{
	m_boids = _boids;
}
Vector3 Boids::Seperation()
{
	Vector3 tendTowardsSep = Vector3::Zero;
	int count = 0;
	for (int i = 0; i < m_boids.size(); i++)
	{
		float CheckDist = Vector3::Distance(m_pos, m_boids[i]->GetPos());
		if ((CheckDist > 0) && (CheckDist < m_BoidData->SeparationVal) && m_boids[i]->GetisPredator() == false)
		{
			Vector3 distance = m_pos - m_boids[i]->GetPos();
			distance.Normalize();
			distance /= CheckDist;
			tendTowardsSep += distance;
			count++;
		}
	}
	if (count > 0)
	{
		tendTowardsSep /= count;
	}
	if (tendTowardsSep != Vector3::Zero)
	{
		tendTowardsSep.Normalize();
		tendTowardsSep *= m_BoidData->MaxSpeed;
		tendTowardsSep -= m_vel;
		tendTowardsSep = XMVector3ClampLength(tendTowardsSep, m_BoidData->MinForce, m_BoidData->MaxForce);

	}
	return tendTowardsSep;
}

Vector3 Boids::Align()
{
	Vector3 num = Vector3::Zero;
	int counter = 0;

	for (int i = 0; i < m_boids.size(); i++)
	{
		float d = Vector3::Distance(m_pos, m_boids[i]->GetPos());
		if ((d > 0) && (d < m_BoidData->NeighbourDist) && (m_boids[i]->GetisPredator() == false))
		{
			num += m_boids[i]->GetVel();
			counter++;
		}
		if (counter > 0)
		{
			num /= (float)counter;
			num.Normalize();
			num *= m_BoidData->MaxSpeed;
			Vector3 tendTowards = num - m_vel;
			tendTowards = XMVector3ClampLength(tendTowards, m_BoidData->MinForce, m_BoidData->MaxForce);
			return tendTowards;
		}
		else
		{
			return Vector3::Zero;
		}
	}
	return Vector3();
}

Vector3 Boids::Cohesion()
{
	Vector3 num = Vector3::Zero;
	int counter = 0;
	for (int i = 0; i < m_boids.size(); i++)
	{
		float d = Vector3::Distance(m_pos, m_boids[i]->GetPos());
		if ((d > 0) && (d < m_BoidData->NeighbourDist) && (m_boids[i]->GetisPredator() == false))
		{
			num += m_boids[i]->GetPos();
			counter++;
		}
		if (counter > 0)
		{
			num /= counter;
			return Seek(num);
		}
		else
		{
			return Vector3::Zero;
		}
	}
	return Vector3();
}

Vector3 Boids::Seek(Vector3 _target)
{
	Vector3 desired = _target - m_pos;
	desired.Normalize();

	desired *= m_BoidData->MaxSpeed;
	desired -= m_vel;

	Vector3 tendTowards = desired - m_vel;
	desired = XMVector3ClampLength(tendTowards, m_BoidData->MinForce, m_BoidData->MaxForce);
	return tendTowards;
}

Vector3 Boids::PredatorSeek()
{
	if (m_ispredator == true)
	{
		Vector3 TendtowardsPrey = Vector3::Zero;

		for (int i = 0; i < m_boids.size(); i++)
		{
			if (m_boids[i]->GetisPredator() == false)
			{
				CheckDist = Vector3::Distance(m_pos, m_boids[i]->GetPos());
				if (CheckDist < closest)
				{
					closest = CheckDist;
					closestBoid = i;
				}
			}

		}
		if ((Vector3::Distance(m_pos, m_boids[closestBoid]->GetPos()) > 10) &&
			(Vector3::Distance(m_pos, m_boids[closestBoid]->GetPos()) < m_BoidData->PredatorSeekDist))
		{
			Vector3 distance = m_pos - m_boids[closestBoid]->GetPos();
			distance.Normalize();
			distance /= CheckDist;
			TendtowardsPrey += distance;
		}

		if (TendtowardsPrey != Vector3::Zero)
		{
			TendtowardsPrey.Normalize();
			TendtowardsPrey *= m_BoidData->MaxSpeed;
			TendtowardsPrey -= m_vel;
			TendtowardsPrey = XMVector3ClampLength(TendtowardsPrey, m_BoidData->MinForce + 1, m_BoidData->MaxForce + 1);

		}
		return TendtowardsPrey;
	}


}

Vector3 Boids::Escape()
{
	Vector3 tendTowardsSep = Vector3::Zero;
	int count = 0;
	for (int i = 0; i < m_boids.size(); i++)
	{
		float CheckDist = Vector3::Distance(m_pos, m_boids[i]->GetPos());
		if ((CheckDist > 0) && (CheckDist < m_BoidData->EscapeValue) && m_boids[i]->GetisPredator() == true)
		{
			Vector3 distance = m_pos - m_boids[i]->GetPos();
			distance.Normalize();
			distance /= CheckDist;
			tendTowardsSep += distance;
			count++;
		}
	}
	if (count > 0)
	{
		tendTowardsSep /= count;
	}
	if (tendTowardsSep != Vector3::Zero)
	{
		tendTowardsSep.Normalize();
		tendTowardsSep *= m_BoidData->MaxSpeed;
		tendTowardsSep -= m_vel;
		tendTowardsSep = XMVector3ClampLength(tendTowardsSep, m_BoidData->MinForce + 1, m_BoidData->MaxForce + 1);

	}
	return tendTowardsSep;
}

void Boids::Bounds()
{
	int Xmin = -100, Xmax = 100, Ymin = -100, Ymax = 100;
	int Zmin = -100, Zmax = 100;


	if (m_pos.x < Xmin)
	{
		m_pos.x += 200;
	}
	else if (m_pos.x > Xmax)
	{
		m_pos.x -= 200;
	}
	if (m_pos.y < Ymin)
	{
		m_pos.y += 200;
	}
	else if (m_pos.y > Ymax)
	{
		m_pos.y -= 200;
	}
	if (m_pos.z < Ymin)
	{
		m_pos.z += 200;
	}
	else if (m_pos.z > Ymax)
	{
		m_pos.z -= 200;
	}
}

void Boids::ApplyRules()
{
	if (m_ispredator == false)
	{
		Vector3 cohesion = Vector3::One;
		Vector3 alignment = Vector3::One;
		Vector3 seperation = Vector3::One;
		Vector3 escapeVec = Vector3::One;

		Vector3 coh = Cohesion();
		Vector3 ali = Align();
		Vector3 sep = Seperation();
		Vector3 esc = Escape();

		cohesion *= m_BoidData->CohesionMod;
		seperation *= m_BoidData->SeperationMod;
		alignment *= m_BoidData->AlignmentMod;
		escapeVec *= m_BoidData->EscapeMod;


		sep *= seperation;
		coh *= cohesion;
		ali *= alignment;
		esc *= escapeVec;

		m_acc += (coh + sep + ali + esc);
	}
	else
	{
		Vector3 PreyMove = PredatorSeek();
		Vector3 pSeek = Vector3::One;
		pSeek *= m_BoidData->PredatorSeekMod;
		PreyMove = pSeek;
		m_acc += (PreyMove);
	}

}
