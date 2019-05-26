//-----------------------------------------------------------------------------
// File: MenuSprite.cpp
//
// Desc: This file contains the class that handles all the menu options.
//
// Created by Vasilescu Vlad
//-----------------------------------------------------------------------------
#ifndef _MENUSPRITE_H_
#define _MENUSPRITE_H_

//-----------------------------------------------------------------------------
// MenuSprite Specific Includes
//-----------------------------------------------------------------------------
#include "Sprite.h"
#include "BackBuffer.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : MenuSprite (Class)
// Desc : Menu Sprite class that handles displaying the options in the 
// game menu.
//-----------------------------------------------------------------------------
class MenuSprite
{
private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	enum CHOICE {
		START,
		LOAD,
		SAVE,
		EXIT
	};

	CHOICE				select;

	Sprite*				startText;
	Sprite*				loadText;
	Sprite*				saveText;
	Sprite*				exitText1;
	Sprite*				exitText2;
	Sprite*				resumeText;

	const BackBuffer*	BF;

public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	MenuSprite(const Vec2 position, const BackBuffer* BF);
	~MenuSprite();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void	draw(ULONG gameState);
	void	opUp(ULONG gameState);
	void	opDown(ULONG gameState);
	CHOICE	getChoice();

	//-------------------------------------------------------------------------
	// Public Variables for This Class.
	//-------------------------------------------------------------------------
	int frameCounter;

private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class.
	//-------------------------------------------------------------------------
	void updateSelect(CHOICE text, bool sel);
};

#endif