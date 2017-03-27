#include "AppClass.h"
void AppClass::InitWindow(String a_sWindowName)
{
	super::InitWindow("W7R_SLERP_DEMO"); // Window Name
}

void AppClass::InitVariables(void)
{
	//Set the camera position
	m_pCameraMngr->SetPositionTargetAndView(
		vector3(0.0f, 2.5f, 15.0f),//Camera position
		vector3(0.0f, 2.5f, 0.0f),//What Im looking at
		REAXISY);//What is up
	//Load a model onto the Mesh manager
	m_pMeshMngr->LoadModel("Minecraft\\Cow.obj", "Cow");
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

	//Call the arcball method
	ArcBall();
	
	//Set the model matrix for the first model to be the arcball
	m_pMeshMngr->SetModelMatrix(ToMatrix4(m_qArcBall), 0);

	//Create the quaternions to interpolate
	glm::quat q1 = glm::angleAxis(0.0f, vector3(0.0f, 0.0f, 1.0f));
	quaternion q2 = glm::angleAxis(359.0f, vector3(0.0f, 0.0f, 1.0f)); //if 360 there is a div by 0 somewhere
	quaternion q3;
	
	static float fTimer = 0.0f; //static timer to keep track
	static int clockIndex = m_pSystem->GenClock();//generate a new clock in the system
	fTimer += m_pSystem->LapClock(clockIndex);//get the delta time of that clock

	float fCycleTime = 5.0f;// time the animation will take to perform
	float fPercentage = MapValue(fTimer, 0.0f, fCycleTime, 0.0f, 1.0f); //map the value in a percentage scale

	q3 = glm::mix(q1, q2, fPercentage); //slerp the quaternions
	
	m_pMeshMngr->SetModelMatrix(ToMatrix4(q3), "Cow");//transform the quaternion to a matrix and assign it to the model
	
	//Adds all loaded instance to the render list
	m_pMeshMngr->AddSkyboxToRenderList();
	m_pMeshMngr->AddInstanceToRenderList("ALL");

	//Indicate the FPS
	int nFPS = m_pSystem->GetFPS();
	//print info into the console
	//printf("FPS: %d            \r", nFPS);//print the Frames per Second
	//Print info on the screen
	m_pMeshMngr->PrintLine("");
	m_pMeshMngr->PrintLine(m_pSystem->GetAppName(), REYELLOW);

	m_pMeshMngr->Print("Timer: ");
	m_pMeshMngr->PrintLine(std::to_string(fTimer), REYELLOW);
	
	m_pMeshMngr->Print("FPS:");
	m_pMeshMngr->Print(std::to_string(nFPS), RERED);
}

void AppClass::Display(void)
{
	//clear the screen
	ClearScreen();
	//Render the grid based on the camera's mode:
	m_pMeshMngr->AddGridToRenderListBasedOnCamera(m_pCameraMngr->GetCameraMode());
	m_pMeshMngr->Render(); //renders the render list
	m_pMeshMngr->ClearRenderList(); //Reset the Render list after render
	m_pGLSystem->GLSwapBuffers(); //Swaps the OpenGL buffers
}

void AppClass::Release(void)
{
	super::Release(); //release the memory of the inherited fields
}