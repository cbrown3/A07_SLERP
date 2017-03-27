#include "AppClass.h"
void AppClass::InitWindow(String a_sWindowName)
{
	super::InitWindow("SLERP - Christopher Brown"); // Window Name

	//Setting the color to black
	m_v4ClearColor = vector4(0.0f);
}

void AppClass::InitVariables(void)
{
	//Setting the position in which the camera is looking and its interest point
	m_pCameraMngr->SetPositionTargetAndView(vector3(12.12f, 28.52f, 11.34f), ZERO_V3, REAXISY);

	//Setting the color to black
	m_v4ClearColor = vector4(0.0f);

	//Loading the models
	m_pMeshMngr->LoadModel("Planets\\00_Sun.obj", "Sun");
	m_pMeshMngr->LoadModel("Planets\\03_Earth.obj", "Earth");
	m_pMeshMngr->LoadModel("Planets\\03A_Moon.obj", "Moon");

	//Setting the days duration
	m_fDay = 1.0f;
}

void AppClass::Update(void)
{
	//Update the system's time
	m_pSystem->UpdateTime();

	//Update the mesh manager's time without updating for collision detection
	m_pMeshMngr->Update();

	//First person camera movement
	if (m_bFPC == true)
		CameraRotation();

	//Getting the time between calls
	double fCallTime = m_pSystem->LapClock();
	//Counting the cumulative time
	static double fRunTime = 0.0f;
	fRunTime += fCallTime;

	//Earth Orbit
	double fEarthHalfOrbTime = 182.5f * m_fDay; //Earths orbit around the sun lasts 365 days / half the time for 2 stops
	float fEarthHalfRevTime = 0.5f * m_fDay; // Move for Half a day
	float fMoonHalfOrbTime = 14.0f * m_fDay; //Moon's orbit is 28 earth days, so half the time for half a route

#pragma region Earth Rotation

	glm::quat earthQ1 = glm::angleAxis(0.0f, vector3(0.0f, 0.0f, 1.0f));
	quaternion earthQ2 = glm::angleAxis(359.0f, vector3(0.0f, 0.0f, 1.0f)); //if 360 there is a div by 0 somewhere
	quaternion earthQ3;

	glm::quat earthQ4 = glm::angleAxis(0.0f, vector3(0.0f, 0.0f, 1.0f));
	quaternion earthQ5 = glm::angleAxis(359.0f, vector3(0.0f, 0.0f, 1.0f)); //if 360 there is a div by 0 somewhere
	quaternion earthQ6;

	//rotate around sun
	float fEarthPercentage = MapValue((float)fRunTime, 0.0f, (float)fEarthHalfOrbTime, 0.0f, 1.0f);

	//revolutions
	float fEarthRotPercentage = MapValue((float)fRunTime, 0.0f, fEarthHalfRevTime, 0.0f, 1.0f);

	earthQ3 = glm::mix(earthQ1, earthQ2, fEarthPercentage);

	earthQ6 = glm::mix(earthQ4, earthQ5, fEarthRotPercentage);

	matrix4 m_m4Earth = IDENTITY_M4;
	
	matrix4 rotEarth = glm::mat4_cast(earthQ3);

	matrix4 transEarth = glm::translate(vector3(11.0f,0.0f,0.0f)); //translate the earth from the center

	matrix4 selfRotEarth = glm::mat4_cast(earthQ6);

	matrix4 scaleEarth = glm::scale(vector3(0.524f));

	matrix4 earthMatrix = rotEarth * transEarth * scaleEarth;

	m_m4Earth = rotEarth * transEarth * selfRotEarth * scaleEarth;

#pragma endregion

#pragma region Moon Rotation
	
	glm::quat moonQ1 = glm::angleAxis(0.0f, vector3(0.0f, 0.0f, 1.0f));
	quaternion moonQ2 = glm::angleAxis(359.0f, vector3(0.0f, 0.0f, 1.0f)); //if 360 there is a div by 0 somewhere
	quaternion moonQ3;

	float fMoonPercentage = MapValue((float)fRunTime, 0.0f, (float)fMoonHalfOrbTime, 0.0f, 1.0f);

	moonQ3 = glm::mix(moonQ1, moonQ2, fMoonPercentage);

	matrix4 m_m4Moon = IDENTITY_M4;
		
	matrix4 rotMoon = earthMatrix * glm::mat4_cast(moonQ3);

	matrix4 transMoon = glm::translate(vector3(2.0f, 0.0f, 0.0f)); //translate the Moon from the center

	matrix4 scaleMoon = glm::scale(vector3(0.141f));

	m_m4Moon = rotMoon * transMoon * scaleMoon;

#pragma endregion

#pragma region Sun Matrix
	
	matrix4 m_m4Sun = glm::scale(vector3(5.936f));

#pragma endregion

	//Setting the matrices
	m_pMeshMngr->SetModelMatrix(m_m4Sun, "Sun");
	m_pMeshMngr->SetModelMatrix(m_m4Earth, "Earth");
	m_pMeshMngr->SetModelMatrix(m_m4Moon, "Moon");

	//Adds all loaded instance to the render list
	m_pMeshMngr->AddInstanceToRenderList("ALL");

	static int nEarthOrbits = 0;
	static int nEarthRevolutions = 0;
	static int nMoonOrbits = 0;


	if (fEarthRotPercentage == 1.0f)
	{
		nEarthRevolutions++;
	}

	if (fEarthPercentage == 1.0f)
	{
		nEarthOrbits++;
	}

	if (fMoonPercentage == 1.0f)
	{
		nMoonOrbits++;
	}


	//Indicate the FPS
	int nFPS = m_pSystem->GetFPS();

	//Print info on the screen
	m_pMeshMngr->PrintLine(m_pSystem->GetAppName(), REYELLOW);
	
	m_pMeshMngr->Print("Time:");
	m_pMeshMngr->PrintLine(std::to_string(fRunTime));

	m_pMeshMngr->Print("Day:");
	m_pMeshMngr->PrintLine(std::to_string(m_fDay));

	m_pMeshMngr->Print("E_Orbits:");
	m_pMeshMngr->PrintLine(std::to_string(nEarthOrbits));

	m_pMeshMngr->Print("E_Revolutions:");
	m_pMeshMngr->PrintLine(std::to_string(nEarthRevolutions));

	m_pMeshMngr->Print("M_Orbits:");
	m_pMeshMngr->PrintLine(std::to_string(nMoonOrbits));

	m_pMeshMngr->Print("FPS:");
	m_pMeshMngr->Print(std::to_string(nFPS), RERED);
}

void AppClass::Display(void)
{
	//clear the screen
	ClearScreen();

	//Render the grid based on the camera's mode:
	switch (m_pCameraMngr->GetCameraMode())
	{
	default:
		break;
	case CAMERAMODE::CAMROTHOX:
		m_pMeshMngr->AddGridToRenderList(1.0f, REAXIS::YZ, RERED * 0.75f); //renders the YZ grid with a 100% scale
		break;
	case CAMERAMODE::CAMROTHOY:
		m_pMeshMngr->AddGridToRenderList(1.0f, REAXIS::XZ, REGREEN * 0.75f); //renders the XZ grid with a 100% scale
		break;
	case CAMERAMODE::CAMROTHOZ:
		m_pMeshMngr->AddGridToRenderList(1.0f, REAXIS::XY, REBLUE * 0.75f); //renders the XY grid with a 100% scale
		break;
	}
	
	m_pMeshMngr->Render(); //renders the render list

	m_pMeshMngr->ClearRenderList(); //Reset the Render list after render

	m_pGLSystem->GLSwapBuffers(); //Swaps the OpenGL buffers
}

void AppClass::Release(void)
{
	super::Release(); //release the memory of the inherited fields
}