
#include "ScoreSprite.h"

#include <string>

//-----------------------------------------------------------------------------
// Name : ScoreSprite () (Constructor)
// Desc : Constructor for the ScoreSprite class.
//-----------------------------------------------------------------------------
ScoreSprite::ScoreSprite(const Vec2 position, const BackBuffer* BF)
{
	this->position = position;
	this->BF = BF;

	posDig0 = Vec2(position.x - 70, position.y + 75);
	posDig1 = Vec2(position.x - 35, position.y + 75);
	posDig2 = Vec2(position.x, position.y + 75);
	posDig3 = Vec2(position.x + 35, position.y + 75);

	scoreDig0 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig0->mPosition = posDig0;
	scoreDig0->mVelocity = Vec2(0, 0);
	scoreDig0->setBackBuffer(BF);

	scoreDig1 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig1->mPosition = posDig1;
	scoreDig1->mVelocity = Vec2(0, 0);
	scoreDig1->setBackBuffer(BF);

	scoreDig2 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig2->mPosition = posDig2;
	scoreDig2->mVelocity = Vec2(0, 0);
	scoreDig2->setBackBuffer(BF);

	scoreDig3 = new Sprite("data/numbers/0.bmp", RGB(0xff, 0x00, 0xff));
	scoreDig3->mPosition = posDig3;
	scoreDig3->mVelocity = Vec2(0, 0);
	scoreDig3->setBackBuffer(BF);

	scoreInt = 0;
}

//-----------------------------------------------------------------------------
// Name : ~ScoreSprite () (Destructor)
// Desc : Destructor for the ScoreSprite class.
//-----------------------------------------------------------------------------
ScoreSprite::~ScoreSprite()
{
	delete scoreDig0;
	delete scoreDig1;
	delete scoreDig2;
	delete scoreDig3;
}

//-----------------------------------------------------------------------------
// Name : updateScore () (Public)
// Desc : Updates the internal score and sets the digits accordingly.
//-----------------------------------------------------------------------------
void ScoreSprite::updateScore(int increment)
{
	scoreInt += increment;

	updateDigit(3, scoreInt % 10);
	updateDigit(2, scoreInt % 100 / 10);
	updateDigit(1, scoreInt % 1000 / 100);
	updateDigit(0, scoreInt % 10000 / 1000);
}

//-----------------------------------------------------------------------------
// Name : updateDigit () (Private)
// Desc : Sets a given digit to a given number.
//-----------------------------------------------------------------------------
void ScoreSprite::updateDigit(int digID, int no)
{
	std::string path = "data/numbers/" + std::to_string(no) + ".bmp";
	
	switch (digID) {
	case 0:
		delete scoreDig0;
		scoreDig0 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig0->mPosition = posDig0;
		scoreDig0->mVelocity = Vec2(0, 0);
		scoreDig0->setBackBuffer(BF);
		break;

	case 1:
		delete scoreDig1;
		scoreDig1 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig1->mPosition = posDig1;
		scoreDig1->mVelocity = Vec2(0, 0);
		scoreDig1->setBackBuffer(BF);
		break;

	case 2:
		delete scoreDig2;
		scoreDig2 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig2->mPosition = posDig2;
		scoreDig2->mVelocity = Vec2(0, 0);
		scoreDig2->setBackBuffer(BF);
		break;

	case 3:
		delete scoreDig3;
		scoreDig3 = new Sprite(path.c_str(), RGB(0xff, 0x00, 0xff));
		scoreDig3->mPosition = posDig3;
		scoreDig3->mVelocity = Vec2(0, 0);
		scoreDig3->setBackBuffer(BF);
		break;

	default:
		break;
	}

}

//-----------------------------------------------------------------------------
// Name : draw () (Public)
// Desc : Draws all the objects inside the class.
//-----------------------------------------------------------------------------
void ScoreSprite::draw()
{
	scoreDig0->draw();
	scoreDig1->draw();
	scoreDig2->draw();
	scoreDig3->draw();
}

//-----------------------------------------------------------------------------
// Name : move () (Public)
// Desc : Moves the object to a different position on the screen.
//-----------------------------------------------------------------------------
void ScoreSprite::move(const Vec2 destination)
{
	position = destination;

	posDig0 = Vec2(position.x - 75, position.y + 75);
	posDig1 = Vec2(position.x - 25, position.y + 75);
	posDig2 = Vec2(position.x + 25, position.y + 75);
	posDig3 = Vec2(position.x + 75, position.y + 75);

	scoreDig0->mPosition = posDig0;
	scoreDig1->mPosition = posDig1;
	scoreDig2->mPosition = posDig2;
	scoreDig3->mPosition = posDig3;
}

//-----------------------------------------------------------------------------
// Name : getScore () (Public)
// Desc : Returns score for given player as integer.
//-----------------------------------------------------------------------------
int ScoreSprite::getScore()
{
	return scoreInt;
}

//-----------------------------------------------------------------------------
// Name : setScore () (Public)
// Desc : Sets score for given player.
//-----------------------------------------------------------------------------
void ScoreSprite::setScore(int newScore)
{
	scoreInt = 0;
	updateScore(newScore);
}
