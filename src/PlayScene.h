#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Button.h"
#include "Heuristic.h"
#include "Label.h"
#include "Obstacle.h"
#include "ship.h"
#include "SpaceShip.h"
#include "Target.h"
#include "Tile.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;	
	glm::vec2 m_mousePosition;

	Target* m_pTarget;
	SpaceShip* m_pShip;
	Label* inst[7];
	Obstacle* m_Obs[5];

	//Ship* m_pShip;
	
	//Pathfinding functions and obj
	/*Tile* m_pTile;*/
	void m_buildGrid();
	void m_computeTileCost() const; 
	void m_findShortestPath();
	void m_displayPathList();
	void m_move();
	void m_reset();
	
	void m_setGridEnabled(bool state) const; 
	std::vector<Tile*> m_pGrid;

	//convenience functions
	Tile* m_getTile(int col, int row) const;
	Tile* m_getTile(glm::vec2 grid_position) const;

	//Heuristic
	Heuristic currentHeuristic;
	// Open,Close and path lists
	std::vector<Tile*> m_pOpenList;
	std::vector<Tile*> m_pClosedList;
	std::vector<Tile*>m_pPathList;

	int moveCounter = 0;
	bool isMoving = false;
	bool gridEnabled = false;
	int keyCd = 0;
	bool pathEnabled = false;
	int xIn = 0, yIn = 0;
	
};
#endif /* defined (__PLAY_SCENE__) */
