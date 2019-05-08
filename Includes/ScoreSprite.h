//-----------------------------------------------------------------------------
// File: ScoreSprite.cpp
//
// Desc: This file stores the object used to display and update the score
// counter for each player
//
// Created by Vasilescu Vlad
//-----------------------------------------------------------------------------
#ifndef _SCORESPRITE_H_
#define _SCORESPRITE_H_

//-----------------------------------------------------------------------------
// ScoreSprite Specific Includes
//-----------------------------------------------------------------------------
#include "Sprite.h"
#include "Vec2.h"
#include "BackBuffer.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : ScoreSprite (Class)
// Desc : Score Sprite class that handles displaying the score and updating it.
//-----------------------------------------------------------------------------
class ScoreSprite 
{
private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	int					scoreInt;
	
	Vec2				position;
	Vec2				posDig0;
	Vec2				posDig1;
	Vec2				posDig2;
	Vec2				posDig3;
	
	Sprite*				scoreDig0;
	Sprite*				scoreDig1;
	Sprite*				scoreDig2;
	Sprite*				scoreDig3;

	const BackBuffer*	BF;

public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	ScoreSprite(const Vec2 position, const BackBuffer* BF);
	~ScoreSprite();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void	updateScore(int increment);
	void	draw();
	void	move(const Vec2 destination);
	int		getScore();
	void	setScore(int newScore);

private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class.
	//-------------------------------------------------------------------------
	void updateDigit(int digID, int no);
};

#endif