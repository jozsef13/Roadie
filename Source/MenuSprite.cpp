//-----------------------------------------------------------------------------
// File: MenuSprite.cpp
//
// Desc: This file contains the class that handles all the menu options.
//
// Created by Vasilescu Vlad
//-----------------------------------------------------------------------------

#include "MenuSprite.h"

//-----------------------------------------------------------------------------
// Name : MenuSprite () (Constructor)
// Desc : Constructor for the MenuSprite class.
//-----------------------------------------------------------------------------
MenuSprite::MenuSprite(const Vec2 position, const BackBuffer* BF)
{
	this->BF = BF;
	frameCounter = 0;

	auto posLoad = Vec2(position.x, position.y + 125);
	auto posSave = Vec2(position.x, position.y + 250);
	auto posExit = Vec2(position.x, position.y + 375);

	resumeText = new Sprite("data/resume_text.bmp", RGB(0xff, 0x00, 0xff));
	resumeText->mPosition = position;
	resumeText->mVelocity = Vec2(0, 0);
	resumeText->setBackBuffer(BF);

	startText = new Sprite("data/newgame_text.bmp", RGB(0xff, 0x00, 0xff));
	startText->mPosition = position;
	startText->mVelocity = Vec2(0, 0);
	startText->setBackBuffer(BF);
	
	loadText = new Sprite("data/loadgame_text.bmp", RGB(0xff, 0x00, 0xff));
	loadText->mPosition = posLoad;
	loadText->mVelocity = Vec2(0, 0);
	loadText->setBackBuffer(BF);
	
	saveText = new Sprite("data/savegame_text.bmp", RGB(0xff, 0x00, 0xff));
	saveText->mPosition = posSave;
	saveText->mVelocity = Vec2(0, 0);
	saveText->setBackBuffer(BF);

	exitText1 = new Sprite("data/exit_text.bmp", RGB(0xff, 0x00, 0xff));
	exitText1->mPosition = posSave;
	exitText1->mVelocity = Vec2(0, 0);
	exitText1->setBackBuffer(BF);

	exitText2 = new Sprite("data/exit_text.bmp", RGB(0xff, 0x00, 0xff));
	exitText2->mPosition = posExit;
	exitText2->mVelocity = Vec2(0, 0);
	exitText2->setBackBuffer(BF);

	select = START;
	updateSelect(START, true);
}

//-----------------------------------------------------------------------------
// Name : ~MenuSprite () (Destructor)
// Desc : Destructor for the MenuSprite class.
//-----------------------------------------------------------------------------
MenuSprite::~MenuSprite()
{
	delete startText;
	delete loadText;
	delete saveText;
	delete exitText1;
	delete exitText2;
	delete resumeText;
}

//-----------------------------------------------------------------------------
// Name : draw () (Public)
// Desc : Draws all the objects inside the class.
//-----------------------------------------------------------------------------
void MenuSprite::draw(ULONG gameState)
{
	frameCounter++;

	switch (gameState) {
	case 0: // start menu
		startText->draw();
		loadText->draw();
		exitText1->draw();
		break;

	case 1: // ongoing game.
		break;

	case 2: // lost game
		break;

	case 3: // won game
		break;

	case 4: // paused game
		resumeText->draw();
		loadText->draw();
		saveText->draw();
		exitText2->draw();
	}
}

//-----------------------------------------------------------------------------
// Name : opUp () (Public)
// Desc : Moves the selection up.
//-----------------------------------------------------------------------------
void MenuSprite::opUp(ULONG gameState)
{
	updateSelect(select, false);

	switch (select) {
	case CHOICE::START:
		select = CHOICE::EXIT;
		break;

	case CHOICE::LOAD:
		select = CHOICE::START;
		break;

	case CHOICE::SAVE:
		select = CHOICE::LOAD;
		break;
	
	case CHOICE::EXIT:
		select = (gameState == 0) ? CHOICE::LOAD : CHOICE::SAVE;
		break;
	}

	updateSelect(select, true);
}

//-----------------------------------------------------------------------------
// Name : opDown () (Public)
// Desc : Moves the selection down.
//-----------------------------------------------------------------------------
void MenuSprite::opDown(ULONG gameState)
{
	updateSelect(select, false);

	switch (select) {
	case CHOICE::START:
		select = CHOICE::LOAD;
		break;

	case CHOICE::LOAD:
		select = (gameState == 0) ? CHOICE::EXIT : CHOICE::SAVE;
		break;

	case CHOICE::SAVE:
		select = CHOICE::EXIT;
		break;

	case CHOICE::EXIT:
		select = CHOICE::START;
		break;
	}

	updateSelect(select, true);
}

//-----------------------------------------------------------------------------
// Name : updateSelect () (Private)
// Desc : Changes the selection.
//-----------------------------------------------------------------------------
void MenuSprite::updateSelect(CHOICE text, bool sel)
{
	Vec2 oldPos;

	switch (text) {
	case CHOICE::START:
		oldPos = startText->mPosition;
		delete startText;
		startText = (sel) ? new Sprite("data/newgameselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/newgame_text.bmp", RGB(0xff, 0x00, 0xff));
		startText->mPosition = oldPos;
		startText->mVelocity = Vec2(0, 0);
		startText->setBackBuffer(BF);

		oldPos = resumeText->mPosition;
		delete resumeText;
		resumeText = (sel) ? new Sprite("data/resumeselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/resume_text.bmp", RGB(0xff, 0x00, 0xff));
		resumeText->mPosition = oldPos;
		resumeText->mVelocity = Vec2(0, 0);
		resumeText->setBackBuffer(BF);
		break;

	case CHOICE::LOAD:
		oldPos = loadText->mPosition;
		delete loadText;
		loadText = (sel) ? new Sprite("data/loadgameselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/loadgame_text.bmp", RGB(0xff, 0x00, 0xff));
		loadText->mPosition = oldPos;
		loadText->mVelocity = Vec2(0, 0);
		loadText->setBackBuffer(BF);
		break;

	case CHOICE::EXIT:
		oldPos = exitText2->mPosition;
		delete exitText2;
		exitText2 = (sel) ? new Sprite("data/exitselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/exit_text.bmp", RGB(0xff, 0x00, 0xff));
		exitText2->mPosition = oldPos;
		exitText2->mVelocity = Vec2(0, 0);
		exitText2->setBackBuffer(BF);

		oldPos = exitText1->mPosition;
		delete exitText1;
		exitText1 = (sel) ? new Sprite("data/exitselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/exit_text.bmp", RGB(0xff, 0x00, 0xff));
		exitText1->mPosition = oldPos;
		exitText1->mVelocity = Vec2(0, 0);
		exitText1->setBackBuffer(BF);
		break;

	case CHOICE::SAVE:
		oldPos = saveText->mPosition;
		delete saveText;
		saveText = (sel) ? new Sprite("data/savegameselected_text.bmp", RGB(0xff, 0x00, 0xff)) :
			new Sprite("data/savegame_text.bmp", RGB(0xff, 0x00, 0xff));
		saveText->mPosition = oldPos;
		saveText->mVelocity = Vec2(0, 0);
		saveText->setBackBuffer(BF);
		break;
	}
}

//-----------------------------------------------------------------------------
// Name : getChoice () (Public)
// Desc : Provides infromation about the current menu state.
//-----------------------------------------------------------------------------
MenuSprite::CHOICE MenuSprite::getChoice()
{
	return select;
}