#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	if(EventManager::Instance().isIMGUIActive())
	{
		GUI_Function();
	}

	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	if(m_pJet->getState() == 3)
	{
		if (CollisionManager::AABBCheck(m_pJet, m_pTarget))
		{
			m_pJet->getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, 0.0f);
			m_pJet->setMaxSpeed(0.0f);
			m_pJet->setArivied(true);

		}
		else if (!CollisionManager::AABBCheck(m_pJet, m_pTarget) && m_pJet->getArivied() == false)
		{
			if (m_pJet->getTransform()->position.x >= m_pTarget->getTransform()->position.x / 2 && m_pJet->getTransform()->position.y >= m_pTarget->getTransform()->position.y / 2)
			{
				m_pJet->setAccelerationRate(1);
				m_pJet->setMaxSpeed(2);
			}
		}
		
		
	}
	
	
	if (CollisionManager::lineRectCheck(m_pJet->m_leftWhisker.Start(), m_pJet->m_leftWhisker.End(), m_pObstacle->getTransform()->position - glm::vec2(100.0f, 50.0f), 200.0f, 100.0f))
	{
		SoundManager::Instance().playSound("yay", 0);
	}
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();


	
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";
	
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = glm::vec2(700.0f, 300.0f);
	addChild(m_pTarget);

	m_pObstacle = new Obstacle();
	m_pObstacle->getTransform()->position = glm::vec2(500.0f, 300.0f);
	addChild(m_pObstacle);

	m_pJet = new Jet();
	m_pJet->getTransform()->position = glm::vec2(100.0f, 300.0f);
	m_pJet->setEnabled(false);
	m_pJet->setDestination(m_pTarget->getTransform()->position);
	m_pJet->m_leftWhisker.SetLine(m_pJet->getTransform()->position, m_pJet->getTransform()->position + m_pJet->getOrientation() * 100.0f);
	addChild(m_pJet);
}

void PlayScene::GUI_Function() const
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - Lab2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	static float speed = 10.0f;
	if (ImGui::SliderFloat("MaxSpeed", &speed, 0.0f, 100.0f))
	{
		m_pJet->setMaxSpeed(speed);
	}

	static float acceleration_rate = 2.0f;
	if (ImGui::SliderFloat("Acceleration Rate", &acceleration_rate, 0.0f, 50.0f))
	{
		m_pJet->setAccelerationRate(acceleration_rate);
	}

	static float angleInRadians = 0.0f;
	if (ImGui::SliderAngle("Orientation angle", &angleInRadians))
	{
		m_pJet->setRotation(angleInRadians * Util::Rad2Deg);
	}

	static float turn_rate = 5.0f;
	if (ImGui::SliderFloat("Turn Rate", &turn_rate, 0.0f, 20.0f))
	{
		m_pJet->setTurnRate(turn_rate);
	}

	if(ImGui::Button("Seek"))
	{
		m_pJet->setEnabled(true);
		m_pJet->setState(1);
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		m_pJet->getTransform()->position = glm::vec2(100.0f, 300.0f);
		m_pJet->setEnabled(false);
		m_pJet->getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
		m_pJet->setRotation(0.0f); //set angle to 0 degrees
		turn_rate = 5.0f;
		acceleration_rate = 2.0f;
		speed = 10.0f;
		angleInRadians = m_pJet->getRotation();
	}

	ImGui::SameLine();
	if (ImGui::Button("Flee"))
	{
		m_pJet->setEnabled(true);
		m_pJet->setState(2);
	}

	ImGui::SameLine();
	if (ImGui::Button("Arival"))
	{
		m_pJet->setEnabled(true);
		m_pJet->setState(3);
	}

	ImGui::Separator();

	static float targetPosition[2] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	if(ImGui::SliderFloat2("Target", targetPosition, 0.0f, 800.0f))
	{
		m_pTarget->getTransform()->position = glm::vec2(targetPosition[0], targetPosition[1]);
		m_pJet->setDestination(m_pTarget->getTransform()->position);
	}
	
	ImGui::End();

	// Don't Remove this
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	ImGui::StyleColorsDark();
}
