//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "CPlayer.h"
#include "BackBuffer.h"
#include "ImageFile.h"
#include "ScoreSprite.h"
#include "MenuSprite.h"

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CGameApp();
	virtual ~CGameApp();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int		 BeginGame( );
	bool		ShutDown( );

	enum GameState {
		START,
		ONGOING,
		LOST,
		WON,
		PAUSE
	};
	enum Levels {
		LEVEL1,
		LEVEL2,
		LEVEL3,
		LEVEL4,
		LEVEL5
	};

	BackBuffer*				m_pBBuffer;
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects	  ( );
	void		ReleaseObjects	( );
	void		FrameAdvance	  ( );
	bool		CreateDisplay	 ( );
	void		SetupGameState	( );
	void		AnimateObjects	( );
	void		DrawObjects	   ( );
	void		ProcessInput	  ( );
	void		addEnemies(int nrEnemies, int timeVelocity, int velocity);
	void		removeDead();
	bool		Collision();
	bool		bulletCollision(const Sprite& bullet, CPlayer& p1);
	void		fireBullet(const Vec2 position, const Vec2 velocity, int x);
	bool		detectBulletCollision(const Sprite* bullet);
	void		enemyFire();
	void		EnemyMove();
	void		setPLives(int livesP1);
	void		updateGameState();
	void		scrollingBackground(int speed);

	
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer				  m_Timer;			// Game timer
	ULONG				   m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HWND					m_hWnd;			 // Main window HWND
	HICON				   m_hIcon;			// Window Icon
	HMENU				   m_hMenu;			// Window Menu
	
	bool					m_bActive;		  // Is the application active ?

	ULONG				   m_nViewX;		   // X Position of render viewport
	ULONG				   m_nViewY;		   // Y Position of render viewport
	ULONG				   m_nViewWidth;	   // Width of render viewport
	ULONG				   m_nViewHeight;	  // Height of render viewport

	POINT				   m_OldCursorPos;	 // Old cursor position for tracking
	HINSTANCE				m_hInstance;
	Vec2					m_screenSize;

	CImageFile				m_imgBackground;

	CPlayer*				m_pPlayer;
	std::list<CPlayer*>		m_enemies;

	std::list<Sprite*>		m_livesGreen;		// Lives for green player

	ScoreSprite*			m_scoreP1;			// Score for the player 1

	std::list<Sprite*>		bullets;
	int						frameCounter = 0;

	std::list<Sprite*>			p1Life;

	GameState					m_gameState;			// Game state (ongoing, won, lost)
	Sprite*						m_wonSprite;			// Information to be displayed when game is won
	Sprite*						m_lostSprite;		// Information to be displayed when game is lost

	Sprite*						livesText;
	Sprite*						scoreText;

	MenuSprite*					gameMenu;

	GameState					_gameState;

};

#endif // _CGAMEAPP_H_