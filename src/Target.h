#pragma once
#ifndef __TARGET__
#define __TARGET__

#include "DisplayObject.h"
#include "NavigationObject.h"

class Target final : public NavigationObject {
public:
	Target();
	~Target();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	//glm::vec2 getGridPosition();
	//void setGridPosition(float col, float row);


private:
	void m_move();
	void m_checkBounds();
	void m_reset();

	//glm::vec2 m_gridPosition;
};


#endif /* defined (__TARGET__) */