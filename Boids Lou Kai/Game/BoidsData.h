#pragma once
struct BoidsData
{
	float MaxSpeed = 0.5f;
	float MinSpeed = 0.0f;
	float MaxForce = 0.3f;
	float MinForce = 0.0f;
	float NeighbourDist = 50.0f;
	float NeighbourDistCoh = 20.0f;
	float SeparationVal = 5.0f;
	float EscapeValue = 10.0f;
	float EscapeMod = 2.0f;
	float AlignmentMod = 1.0f;
	float CohesionMod = 5.0f;
	float SeperationMod = 2.0f;
	float PredatorSeekMod = 5.0f;
	float PredatorSeekDist = 50.0f;



};