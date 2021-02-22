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
	SoundManager::Instance().load("../Assets/audio/TBgm.mp3", "Bgm", SOUND_MUSIC);
	SoundManager::Instance().load("../Assets/audio/Start.mp3", "Start", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/Goal.ogg", "Goal", SOUND_SFX);
	SoundManager::Instance().playMusic("Bgm", -1, 0);
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	
	if(EventManager::Instance().isIMGUIActive())
	{
		GUI_Function();
	}

	//drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	//std::cout << keyCd << std::endl;
	if(Game::Instance()->getDeltaTime())
	{
		keyCd++;
	}
	
	if (isMoving)
	{
		m_move();
		m_pShip->move = true;
	}
	else
	{
		m_pShip->move = false;
	}

	//Display Grid
	if(EventManager::Instance().isKeyDown(SDL_SCANCODE_H)&&keyCd>=10)
	{
		gridEnabled=!gridEnabled;
		if (gridEnabled == true)
			m_setGridEnabled(true);
		else if (gridEnabled == false)
			m_setGridEnabled(false);

		keyCd = 0;
	}

	//Display Shortest Path
	if(EventManager::Instance().isKeyDown(SDL_SCANCODE_F) && keyCd >= 10)
	{
		keyCd = 0;
		pathEnabled = true;
		//m_computeTileCost();
		m_findShortestPath();
		for (auto node : m_pPathList)
		{
			node->setLabelsEnabled(true);
		}
		//if (pathEnabled==false)
		//{
		//	keyCd = 0;
		//	pathEnabled = true;
		//	//m_computeTileCost();
		//	m_findShortestPath();
		//	for (auto node : m_pPathList)
		//	{
		//		node->setLabelsEnabled(true);
		//	}
		//}

		//else if(pathEnabled==true)
		//{
		//	keyCd = 0;
		//	pathEnabled = false;
		//	for (auto node : m_pPathList)
		//	{
		//		node->setLabelsEnabled(false);
		//	}
		//	m_pPathList.clear();
		//	m_pPathList.shrink_to_fit();
		//}
	}

	//Move
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_M) && !isMoving)
	{
		//m_findShortestPath();
		SoundManager::Instance().playSound("Start", 0);
		isMoving=true;
	}
	//Reset
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_R) && keyCd >= 10)//
	{
		auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

		m_reset();
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset;
		m_pTarget->setGridPosition(15, 11);
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
		m_computeTileCost();
		SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
		SDL_RenderPresent(Renderer::Instance()->getRenderer());
	}
	//Mouse
		auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

		if (gridEnabled==true)
		{
			if (EventManager::Instance().getMouseButton(0) || EventManager::Instance().getMouseButton(2))
			{

				xIn = (EventManager::Instance().getMousePosition().x / 40);
				yIn = (EventManager::Instance().getMousePosition().y / 40);
				std::cout << xIn << std::endl << yIn << std::endl;
				if (EventManager::Instance().getMouseButton(0))
				{
					m_getTile(m_pShip->getGridPosition())->setTileStatus(UNVISITED);
					m_pShip->getTransform()->position.x = (float)xIn * 40 + 20;
					m_pShip->getTransform()->position.y = (float)yIn * 40 + 20;
					m_pShip->setGridPosition(xIn, yIn);
					m_getTile(m_pShip->getGridPosition())->setTileStatus(START);
					//m_computeTileCost();
					SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
					SDL_RenderPresent(Renderer::Instance()->getRenderer());
				}
				else if (EventManager::Instance().getMouseButton(2))
				{
					m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
					m_pTarget->getTransform()->position.x = (float)xIn * 40 + 20;
					m_pTarget->getTransform()->position.y = (float)yIn * 40 + 20;
					m_pTarget->setGridPosition(xIn, yIn);
					m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
					m_computeTileCost();
					SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
					SDL_RenderPresent(Renderer::Instance()->getRenderer());
				}
			}
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
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_buildGrid();
	
	currentHeuristic = EUCLIDEAN;
	

	



	m_pShip = new SpaceShip();
	m_pShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset;
	m_pShip->setGridPosition(1, 3);
	m_getTile(1, 3)->setTileStatus(START);
	addChild(m_pShip);

	m_pTarget = new Target();
	m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset;
	m_pTarget->setGridPosition(15, 11);
	m_getTile(15, 11)->setTileStatus(GOAL);
	addChild(m_pTarget);
	m_computeTileCost();


	//TODO
	
	m_Obs[0] = new Obstacle();
	m_Obs[0]->getTransform()->position = m_getTile(3, 3)->getTransform()->position;
	addChild(m_Obs[0]);
	m_getTile(3, 2)->isObstacle = true;
	m_getTile(3, 3)->isObstacle = true;
	m_getTile(2, 2)->isObstacle = true;
	m_getTile(2, 3)->isObstacle = true;

	m_Obs[3] = new Obstacle();
	m_Obs[3]->getTransform()->position = m_getTile(3, 7)->getTransform()->position;
	addChild(m_Obs[3]);
	m_getTile(3, 6)->isObstacle = true;
	m_getTile(3, 7)->isObstacle = true;
	m_getTile(2, 6)->isObstacle = true;
	m_getTile(2, 7)->isObstacle = true;

	m_Obs[1] = new Obstacle();
	m_Obs[1]->getTransform()->position = m_getTile(3, 14)->getTransform()->position;
	addChild(m_Obs[1]);
	m_getTile(3, 13)->isObstacle = true;
	m_getTile(3, 14)->isObstacle = true;
	m_getTile(2, 13)->isObstacle = true;
	m_getTile(2, 14)->isObstacle = true;

	m_Obs[5] = new Obstacle();
	m_Obs[5]->getTransform()->position = m_getTile(9, 3)->getTransform()->position;
	addChild(m_Obs[5]);
	m_getTile(9, 2)->isObstacle = true;
	m_getTile(9, 3)->isObstacle = true;
	m_getTile(8, 2)->isObstacle = true;
	m_getTile(8, 3)->isObstacle = true;

	m_Obs[7] = new Obstacle();
	m_Obs[7]->getTransform()->position = m_getTile(9, 7)->getTransform()->position;
	addChild(m_Obs[7]);
	m_getTile(9, 6)->isObstacle = true;
	m_getTile(9, 7)->isObstacle = true;
	m_getTile(8, 6)->isObstacle = true;
	m_getTile(8, 7)->isObstacle = true;

	m_Obs[9] = new Obstacle();
	m_Obs[9]->getTransform()->position = m_getTile(9, 1)->getTransform()->position;
	addChild(m_Obs[9]);
	m_getTile(9, 0)->isObstacle = true;
	m_getTile(9, 1)->isObstacle = true;
	m_getTile(8, 0)->isObstacle = true;
	m_getTile(8, 1)->isObstacle = true;

	m_Obs[11] = new Obstacle();
	m_Obs[11]->getTransform()->position = m_getTile(9, 14)->getTransform()->position;
	addChild(m_Obs[11]);
	m_getTile(9, 13)->isObstacle = true;
	m_getTile(9, 14)->isObstacle = true;
	m_getTile(8, 13)->isObstacle = true;
	m_getTile(8, 14)->isObstacle = true;

	m_Obs[12] = new Obstacle();
	m_Obs[12]->getTransform()->position = m_getTile(13, 5)->getTransform()->position;
	addChild(m_Obs[12]);
	m_getTile(12, 4)->isObstacle = true;
	m_getTile(13, 5)->isObstacle = true;
	m_getTile(12, 4)->isObstacle = true;
	m_getTile(13, 5)->isObstacle = true;

	m_Obs[13] = new Obstacle();
	m_Obs[13]->getTransform()->position = m_getTile(17, 5)->getTransform()->position;
	addChild(m_Obs[13]);
	m_getTile(16, 4)->isObstacle = true;
	m_getTile(17, 5)->isObstacle = true;
	m_getTile(16, 4)->isObstacle = true;
	m_getTile(17, 5)->isObstacle = true;

	m_Obs[14] = new Obstacle();
	m_Obs[14]->getTransform()->position = m_getTile(13, 11)->getTransform()->position;
	addChild(m_Obs[14]);
	m_getTile(12, 10)->isObstacle = true;
	m_getTile(13, 11)->isObstacle = true;
	m_getTile(12, 10)->isObstacle = true;
	m_getTile(13, 11)->isObstacle = true;

	m_Obs[15] = new Obstacle();
	m_Obs[15]->getTransform()->position = m_getTile(17, 11)->getTransform()->position;
	addChild(m_Obs[15]);
	m_getTile(16, 10)->isObstacle = true;
	m_getTile(17, 11)->isObstacle = true;
	m_getTile(16, 10)->isObstacle = true;
	m_getTile(17, 11)->isObstacle = true;

	m_Obs[16] = new Obstacle();
	m_Obs[16]->getTransform()->position = m_getTile(15, 8)->getTransform()->position;
	addChild(m_Obs[16]);
	m_getTile(14, 7)->isObstacle = true;
	m_getTile(15, 8)->isObstacle = true;
	m_getTile(14, 7)->isObstacle = true;
	m_getTile(15, 8)->isObstacle = true;
	
	//Labels
	const SDL_Color DarkYellow = { 0, 0, 0, 255 };
	
	inst[1] = new Label("Press H for Debug View", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 30.0f));
	inst[1]->setParent(this);
	addChild(inst[1]);

	inst[2] = new Label("Mouse Left Click to change Actor position (While Debug view is on)", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 60.0f));
	inst[2]->setParent(this);
	addChild(inst[2]);

	inst[3] = new Label("Mouse Right Click to change Target position (While Debug view is on)", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 90.0f));
	inst[3]->setParent(this);
	addChild(inst[3]);

	inst[4] = new Label("Press F to Find and Display the Shortest Path (needs to reset to use again", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 120.0f));
	inst[4]->setParent(this);
	addChild(inst[4]);

	inst[5] = new Label("Press M to start moving (After founding Shortest Path)", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 150.0f));
	inst[5]->setParent(this);
	addChild(inst[5]);

	inst[6] = new Label("Press R to Reset All", "Consolas", 20, DarkYellow, glm::vec2(420.0f, 180.0f));
	inst[6]->setParent(this);
	addChild(inst[6]);

	
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("GAME3001 - Lab 3", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	static bool isGridEnabled = false;
	if (ImGui::Checkbox("Grid Enabled", &isGridEnabled))
	{
		//turn the grid on/off
		m_setGridEnabled(isGridEnabled);
	}

	ImGui::Separator();

	auto radio = static_cast<int>(currentHeuristic);
	ImGui::LabelText("", "Heuristic Type");
	ImGui::RadioButton("Euclidean", &radio, static_cast<int>(EUCLIDEAN));
	ImGui::SameLine();
	ImGui::RadioButton("Manhattan", &radio, static_cast<int>(MANHATTAN));

	if (currentHeuristic != Heuristic(radio))
	{
		currentHeuristic = Heuristic(radio);
		m_computeTileCost();
	}

	ImGui::Separator();

	static int startPosition[] = { m_pShip->getGridPosition().x,m_pShip->getGridPosition().y };
	if (ImGui::SliderInt2("Start Position", startPosition, 0, Config::COL_NUM - 1))
	{
		//Row adjustment so that it does not go out screen
		if (startPosition[1] > Config::ROW_NUM - 1)
		{
			startPosition[1] = Config::ROW_NUM - 1;
		}
		SDL_RenderClear(Renderer::Instance()->getRenderer());
		m_getTile(m_pShip->getGridPosition())->setTileStatus(UNVISITED);
		m_pShip->getTransform()->position = m_getTile(startPosition[0], startPosition[1])->getTransform()->position + offset;
		m_pShip->setGridPosition(startPosition[0], startPosition[1]);
		m_getTile(m_pShip->getGridPosition())->setTileStatus(START);
		SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
		SDL_RenderPresent(Renderer::Instance()->getRenderer());
	}
	
	static int targetPosition[] = { m_pTarget->getGridPosition().x,m_pTarget->getGridPosition().y };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, Config::COL_NUM - 1))
	{
		//Row adjustment so that it does not go out screen
		if(targetPosition[1]>Config::ROW_NUM-1)
		{
			targetPosition[1] = Config::ROW_NUM - 1;
		}
		SDL_RenderClear(Renderer::Instance()->getRenderer());
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(targetPosition[0], targetPosition[1])->getTransform()->position + offset;
		m_pTarget->setGridPosition(targetPosition[0], targetPosition[1]);
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
		m_computeTileCost();
		SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
		SDL_RenderPresent(Renderer::Instance()->getRenderer());
	}
	ImGui::Separator();
	if(ImGui::Button("Start"))
	{
		//m_findShortestPath();
		isMoving = true;
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Reset"))
	{
		m_reset();
	}

	ImGui::Separator();

	ImGui::End();

	// Don't Remove this
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	ImGui::StyleColorsDark();
}

void PlayScene::m_buildGrid()
{

	auto tileSize = Config::TILE_SIZE;

	//Add tiles to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile();//Create empty tile
			tile->getTransform()->position = glm::vec2(col*tileSize, row*tileSize);
			tile->setGridPosition(col, row);
			addChild(tile);
			tile->addLabels();	
			tile->setEnabled(false);
			m_pGrid.push_back(tile);

		}
	}

	//Create references for each tile to its neighbours
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			//Topmost Row
			if (row == 0)
			{
				tile->setNeighbourTile(TOP_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(TOP_TILE, m_getTile(col, row - 1));
			}

			//Rightmost column
			if (col == Config::COL_NUM - 1)
			{
				tile->setNeighbourTile(RIGHT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(RIGHT_TILE, m_getTile(col+1, row));

			}

			//BottomMost Row
			if(row==Config::ROW_NUM-1)
			{
				tile->setNeighbourTile(BOTTOM_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));

			}

			//Leftmost Column
			if (col==0)
			{
				tile->setNeighbourTile(LEFT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));
			}
		}
		
	}

 	std::cout << m_pGrid.size() << std::endl;
}

void PlayScene::m_computeTileCost() const
{
	//auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	float distance, dx, dy;


	for (auto tile : m_pGrid)
	{

			switch (currentHeuristic)
			{
			case MANHATTAN:
				//Manhattan Distance
				dx = abs(tile->getGridPosition().x - m_pTarget->getGridPosition().x);
				dy = abs(tile->getGridPosition().y - m_pTarget->getGridPosition().y);
				distance = dx + dy;
				break;
			case EUCLIDEAN:
				//Euclidiean Distance
				distance = Util::distance(m_pTarget->getGridPosition(), tile->getGridPosition());
				break;
			}
			tile->setTileCost(distance);
	}
	m_getTile(3, 2)->setTileCost(30.0f);
	m_getTile(3, 3)->setTileCost(30.0f);
	m_getTile(2, 2)->setTileCost(30.0f);
	m_getTile(2, 3)->setTileCost(30.0f);
	
	m_getTile(3, 6)->setTileCost(30.0f);
	m_getTile(3, 7)->setTileCost(30.0f);
	m_getTile(2, 6)->setTileCost(30.0f);
	m_getTile(2, 7)->setTileCost(30.0f);
	
	m_getTile(3, 13)->setTileCost(30.0f);
	m_getTile(3, 14)->setTileCost(30.0f);
	m_getTile(2, 13)->setTileCost(30.0f);
	m_getTile(2, 14)->setTileCost(30.0f);
	
	m_getTile(9, 2)->setTileCost(30.0f);
	m_getTile(9, 3)->setTileCost(30.0f);
	m_getTile(8, 2)->setTileCost(30.0f);
	m_getTile(8, 3)->setTileCost(30.0f);
	
	m_getTile(9, 6)->setTileCost(30.0f);
	m_getTile(9, 7)->setTileCost(30.0f);
	m_getTile(8, 6)->setTileCost(30.0f);
	m_getTile(8, 7)->setTileCost(30.0f);
	
	m_getTile(9, 0)->setTileCost(30.0f);
	m_getTile(9, 1)->setTileCost(30.0f);
	m_getTile(8, 0)->setTileCost(30.0f);
	m_getTile(8, 1)->setTileCost(30.0f);
	
	m_getTile(9, 13)->setTileCost(30.0f);
	m_getTile(9, 14)->setTileCost(30.0f);
	m_getTile(8, 13)->setTileCost(30.0f);
	m_getTile(8, 14)->setTileCost(30.0f);
	
	m_getTile(12, 4)->setTileCost(30.0f);
	m_getTile(12, 5)->setTileCost(30.0f);
	m_getTile(13, 4)->setTileCost(30.0f);
	m_getTile(13, 5)->setTileCost(30.0f);

	m_getTile(16, 4)->setTileCost(30.0f);
	m_getTile(16, 5)->setTileCost(30.0f);
	m_getTile(17, 4)->setTileCost(30.0f);
	m_getTile(17, 5)->setTileCost(30.0f);

	m_getTile(12, 10)->setTileCost(30.0f);
	m_getTile(12, 11)->setTileCost(30.0f);
	m_getTile(13, 10)->setTileCost(30.0f);
	m_getTile(13, 11)->setTileCost(30.0f);

	m_getTile(16, 10)->setTileCost(30.0f);
	m_getTile(16, 11)->setTileCost(30.0f);
	m_getTile(17, 10)->setTileCost(30.0f);
	m_getTile(17, 11)->setTileCost(30.0f);

	m_getTile(14, 7)->setTileCost(30.0f);
	m_getTile(14, 8)->setTileCost(30.0f);
	m_getTile(15, 7)->setTileCost(30.0f);
	m_getTile(15, 8)->setTileCost(30.0f);
}

void PlayScene::m_findShortestPath()
{
	if (m_pPathList.empty())
	{
		//Step1-Add Starting position to the open list
		auto startTile = m_getTile(m_pShip->getGridPosition());
		startTile->setTileStatus(OPEN);
		m_pOpenList.push_back(startTile);

		bool goalFound = false;

		//Step2-Loop until the OpenList is empty or the goal is found
		while (!m_pOpenList.empty() && !goalFound)
		{
			auto min = INFINITY;
			Tile* minTile;
			int minTileIndex = 0;
			int count = 0;

			std::vector<Tile*>neighbourList;
			for (int index = 0; index < NUM_OF_NEIGHBOUR_TILES; ++index)
			{
				neighbourList.push_back(m_pOpenList[0]->getNeighbourTile(NeighbourTile(index)));
			}

			for (auto neighbour : neighbourList)
			{
				if (neighbour->getTileStatus() != GOAL)
				{
					if ((neighbour->getTileCost() < min))
					{
						min = neighbour->getTileCost();
						minTile = neighbour;
						minTileIndex = count;
					}
					count++;
				}
				else if(neighbour->getTileStatus() == GOAL)
				{
					minTile = neighbour;
					m_pPathList.push_back(minTile);
					goalFound = true;
					break;
				}
			}

			//Remove the reference of the current tile open list
			m_pPathList.push_back(m_pOpenList[0]);
			m_pOpenList.pop_back();

			//add the minTile to the open list
			m_pOpenList.push_back(minTile);
			minTile->setTileStatus(OPEN);
			neighbourList.erase(neighbourList.begin() + minTileIndex);

			//Push all remaining to the close list
			for (auto neighbour : neighbourList)
			{
				if(neighbour->isObstacle==true)
				{
					neighbour->setTileStatus(IMPASSABLE);
					m_pClosedList.push_back(neighbour);
				}
				if (neighbour->getTileStatus() == UNVISITED)
				{
					neighbour->setTileStatus(CLOSED);
					m_pClosedList.push_back(neighbour);
				}
			}
		}
		m_displayPathList();
	}
}

void PlayScene::m_displayPathList()
{
	for (auto node : m_pPathList)
	{
		std::cout << "(" << node->getGridPosition().x <<", "<< node->getGridPosition().y << ")" << std::endl;
	}
	std::cout << "Path Lenght: " << m_pPathList.size() << std::endl;
}

void PlayScene::m_move()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	int index = m_pPathList.size();
	if (moveCounter < index-1)
	{
		//m_pShip->move = true;
		m_pShip->setDestination(m_getTile(m_pPathList[moveCounter]->getGridPosition())->getTransform()->position + offset);
		m_pShip->setGridPosition(m_pPathList[moveCounter]->getGridPosition().x, m_pPathList[moveCounter]->getGridPosition().y);
		std::cout << "COUNTER: " << moveCounter << std::endl;
	
		if (Game::Instance()->getFrames() % 40 == 0)
		{
			moveCounter++;
		}
	}

	else if(Util::distance(m_pShip->getTransform()->position,m_pTarget->getTransform()->position)<=5.0f)
	{
		isMoving = false;
		m_pShip->move = false;
		SoundManager::Instance().playSound("Goal", 0);
		std::cout << "Over" << std::endl;
	}
}

void PlayScene::m_reset()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	currentHeuristic = MANHATTAN;
	
	isMoving = false;

	gridEnabled = false;
	if (!gridEnabled)
	{
		m_setGridEnabled(gridEnabled);
	}

	pathEnabled = false;
	if (!pathEnabled)
	{
		for (auto node : m_pPathList)
		{
			node->setLabelsEnabled(false);
		}
	}

	for (auto node : m_pPathList)
	{
		if (node->getTileStatus() == OPEN)
		{
			node->setTileStatus(UNVISITED);
			m_pOpenList.push_back(node);
		}
	}
	for (auto node : m_pClosedList)
	{
		if (node->getTileStatus() == CLOSED)
		{
			node->setTileStatus(UNVISITED);
			//m_pOpenList.push_back(node);
		}
	}
		m_pPathList.clear();
		m_pPathList.shrink_to_fit();

		if (m_pPathList.empty())
		{
			std::cout << "PATH EMPTY" << std::endl;
		}
			
		m_pClosedList.clear();
		m_pClosedList.shrink_to_fit();


		m_pOpenList.clear();
		m_pOpenList.shrink_to_fit();

		moveCounter = 0;

	SDL_RenderPresent(Renderer::Instance()->getRenderer());
	m_getTile(m_pShip->getGridPosition())->setTileStatus(UNVISITED);
	m_pShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset;
	m_pShip->setGridPosition(1, 3);
	m_getTile(m_pShip->getGridPosition())->setTileStatus(START);

	SDL_RenderClear(Renderer::Instance()->getRenderer());
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
	m_pTarget->getTransform()->position = m_getTile(15,11)->getTransform()->position + offset;
	m_pTarget->setGridPosition(51,11);
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
	currentHeuristic = EUCLIDEAN;
	m_computeTileCost();
	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
	SDL_RenderPresent(Renderer::Instance()->getRenderer());

}

void PlayScene::m_setGridEnabled(bool state) const
{
	for (auto tile : m_pGrid)
	{
		tile->setEnabled(state);
		tile->setLabelsEnabled(state);
	}
	if(state==false)
	{
		SDL_RenderClear(Renderer::Instance()->getRenderer());
	}
}

Tile* PlayScene::m_getTile(const int col, const int row) const
{
 	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(glm::vec2 grid_position) const
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;
	return m_pGrid[(row * Config::COL_NUM) + col];
}
