//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"
#include <cstdlib> 
#include <ctime> 
#include <fstream>
#include <cmath>

extern HINSTANCE g_hInst;
bool		p1Shoot = false;
bool		p2Shoot = false;
int			frameCounter = 0;
bool		okLoad = 0;
static UINT fTimer;
int			incrementScore = 0;
int			powerUp = 0;
int			powerUpShield = 0;
int			powerUpGun = 0;
int			powerUpDouble = 0;
int			horn = 0;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_scoreP1		= NULL;
	m_wonSprite		= NULL;
	m_lostSprite	= NULL;
	livesText		= NULL;
	scoreText		= NULL;
	gameMenu		= NULL;
	m_level1Text	= NULL;
	m_level2Text	= NULL;
	m_level3Text	= NULL;
	m_level4Text	= NULL;
	m_level5Text	= NULL;
	m_LastFrameRate = 0;
	shootText		= NULL;
	doubleText		= NULL;
	shieldText		= NULL;
	shootTextSel	= NULL;
	doubleTextSel	= NULL;
	shieldTextSel	= NULL;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) return NULL;
	m_screenSize = Vec2(mi.rcMonitor.right, mi.rcMonitor.bottom);

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= mi.rcMonitor.right;
	USHORT			Height			= mi.rcMonitor.bottom;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}
	
//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				if (m_gameState == GameState::ONGOING) m_gameState = GameState::PAUSE;
				else if (m_gameState == GameState::LOST) m_gameState = GameState::START;
				else if (m_gameState == GameState::WON) m_gameState = GameState::START;
				else PostQuitMessage(0);
				break;
			case 'R':
				m_pPlayer->Rotate();
				break;
			}
			break;

		case WM_TIMER:
			switch (wParam)
			{
			case 1:
				if (!m_pPlayer->AdvanceExplosion()) 
					fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				for (auto enem : m_enemies)
				{
					if (!enem->AdvanceExplosion())
						fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				}
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer, "data/car4.bmp");
	m_scoreP1 = new ScoreSprite(Vec2(95, 100), m_pBBuffer);
	livesText = new Sprite("data/lives_text.bmp", RGB(0xff, 0x00, 0xff));
	scoreText = new Sprite("data/score_text.bmp", RGB(0xff, 0x00, 0xff));
	m_wonSprite = new Sprite("data/winscreen.bmp", RGB(0xff, 0x00, 0xff));
	m_lostSprite = new Sprite("data/losescreen.bmp", RGB(0xff, 0x00, 0xff));

	gameMenu = new MenuSprite(Vec2(m_screenSize.x / 2, m_screenSize.y / 2 - 200), m_pBBuffer);

	m_wonSprite->setBackBuffer(m_pBBuffer);
	m_lostSprite->setBackBuffer(m_pBBuffer);
	addEnemies(20, 2, 70);
	m_level1Text = new Sprite("data/level1_text.bmp", RGB(0xff, 0x00, 0xff));
	m_level2Text = new Sprite("data/level2_text.bmp", RGB(0xff, 0x00, 0xff));
	m_level3Text = new Sprite("data/level3_text.bmp", RGB(0xff, 0x00, 0xff));
	m_level4Text = new Sprite("data/level4_text.bmp", RGB(0xff, 0x00, 0xff));
	m_level5Text = new Sprite("data/level5_text.bmp", RGB(0xff, 0x00, 0xff));

	m_level1Text->setBackBuffer(m_pBBuffer);
	m_level2Text->setBackBuffer(m_pBBuffer);
	m_level3Text->setBackBuffer(m_pBBuffer);
	m_level4Text->setBackBuffer(m_pBBuffer);
	m_level5Text->setBackBuffer(m_pBBuffer);

	shootText = new Sprite("data/shoot_text.bmp", RGB(0xff, 0x00, 0xff));
	shootText->setBackBuffer(m_pBBuffer);

	shieldText = new Sprite("data/shield_text.bmp", RGB(0xff, 0x00, 0xff));
	shieldText->setBackBuffer(m_pBBuffer);

	doubleText = new Sprite("data/doublepoints_text.bmp", RGB(0xff, 0x00, 0xff));
	doubleText->setBackBuffer(m_pBBuffer);

	shootTextSel = new Sprite("data/shootsel_text.bmp", RGB(0xff, 0x00, 0xff));
	shootTextSel->setBackBuffer(m_pBBuffer);

	shieldTextSel = new Sprite("data/shieldsel_text.bmp", RGB(0xff, 0x00, 0xff));
	shieldTextSel->setBackBuffer(m_pBBuffer);

	doubleTextSel = new Sprite("data/doublepointssel_text.bmp", RGB(0xff, 0x00, 0xff));
	doubleTextSel->setBackBuffer(m_pBBuffer);
	
	livesText->setBackBuffer(m_pBBuffer);
	scoreText->setBackBuffer(m_pBBuffer);

	addPowerUp(powerUp);
	setPLives(3);

	if(!m_imgBackground.LoadBitmapFromFile("data/Background.bmp", GetDC(m_hWnd)))
		return false;

	if (!m_imgBackgroundMenu.LoadBitmapFromFile("data/backgroundMenu.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(690, 600);

	m_pPlayer->frameCounter() = 250;

	livesText->mPosition = Vec2(80, 30);
	scoreText->mPosition = Vec2(80, 125);
	m_level1Text->mPosition = Vec2(80, 230);
	m_level2Text->mPosition = Vec2(80, 230);
	m_level3Text->mPosition = Vec2(80, 230);
	m_level4Text->mPosition = Vec2(80, 230);
	m_level5Text->mPosition = Vec2(80, 230);

	shootText->mPosition = Vec2(75, 330);
	shieldText->mPosition = Vec2(75, 280);
	doubleText->mPosition = Vec2(80, 400);

	shootTextSel->mPosition = Vec2(75, 330);
	shieldTextSel->mPosition = Vec2(75, 280);
	doubleTextSel->mPosition = Vec2(80, 400);

	m_wonSprite->mPosition = Vec2(int(m_screenSize.x / 2), int(m_screenSize.y / 2));
	m_lostSprite->mPosition = Vec2(int(m_screenSize.x / 2), int(m_screenSize.y / 2));

	m_gameState = GameState::START;
	m_levels = Levels::LEVEL1;
	levelForSave = "level1";
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if (m_scoreP1 != NULL) {
		delete m_scoreP1;
		m_scoreP1 = NULL;
	}

	if (m_wonSprite != NULL) {
		delete m_wonSprite;
		m_wonSprite = NULL;
	}

	if (m_lostSprite != NULL) {
		delete m_lostSprite;
		m_lostSprite = NULL;
	}

	if (livesText != NULL) {
		delete livesText;
		livesText = NULL;
	}

	if (scoreText != NULL) {
		delete scoreText;
		scoreText = NULL;
	}

	if (m_level1Text != NULL)
	{
		delete m_level1Text;
		m_level1Text = NULL;
	}

	if (m_level2Text != NULL)
	{
		delete m_level2Text;
		m_level2Text = NULL;
	}

	if (m_level3Text != NULL)
	{
		delete m_level3Text;
		m_level3Text = NULL;
	}

	if (m_level4Text != NULL)
	{
		delete m_level4Text;
		m_level4Text = NULL;
	}

	if (m_level5Text != NULL)
	{
		delete m_level5Text;
		m_level5Text = NULL;
	}

	if (gameMenu != NULL) {
		delete gameMenu;
		gameMenu = NULL;
	}

	if (doublerPower != NULL)
	{
		delete doublerPower;
		doublerPower = NULL;
	}
	
	if (addLivePower != NULL)
	{
		delete addLivePower;
		addLivePower = NULL;
	}

	if (gunPower != NULL)
	{
		delete gunPower;
		gunPower = NULL;
	}

	if (shieldPower != NULL)
	{
		delete shieldPower;
		shieldPower = NULL;
	}

	if (shootText != NULL)
	{
		delete shootText;
		shootText = NULL;
	}

	if (shieldText != NULL)
	{
		delete shieldText;
		shieldText = NULL;
	}

	if (doubleText != NULL)
	{
		delete doubleText;
		doubleText = NULL;
	}

	if (shootTextSel != NULL)
	{
		delete shootTextSel;
		shootTextSel = NULL;
	}

	if (shieldTextSel != NULL)
	{
		delete shieldTextSel;
		shieldTextSel = NULL;
	}

	if (doubleTextSel != NULL)
	{
		delete doubleTextSel;
		doubleTextSel = NULL;
	}

	while (!m_enemies.empty()) delete m_enemies.front(), m_enemies.pop_front();
	while (!bullets.empty()) delete bullets.front(), bullets.pop_front();
	while (!m_livesGreen.empty()) delete m_livesGreen.front(), m_livesGreen.pop_front();

	if (m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Remove dead enemies
	removeDead();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;
	ULONG		Direction1 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	if (m_gameState == GameState::START || m_gameState == GameState::PAUSE) {
		if (pKeyBuffer[VK_UP] & 0xF0 && gameMenu->frameCounter >= 20) {
			gameMenu->opUp(m_gameState);
			mciSendString("play data/sounds/menu_select.wav", NULL, 0, NULL);
			gameMenu->frameCounter = 0;
		}
		if (pKeyBuffer[VK_DOWN] & 0xF0 && gameMenu->frameCounter >= 20) {
			gameMenu->opDown(m_gameState);
			mciSendString("play data/sounds/menu_select.wav", NULL, 0, NULL);
			gameMenu->frameCounter = 0;
		}

		if (pKeyBuffer[VK_RETURN] & 0xF0) {
			mciSendString("play data/sounds/menu_sel.wav", NULL, 0, NULL);
			if (gameMenu->getChoice() == 0)
				m_gameState = GameState::ONGOING;

			if (gameMenu->getChoice() == 1)
				loadGame();

			if (gameMenu->getChoice() == 2)
				saveGame();

			if(gameMenu->getChoice() == 3)
				PostQuitMessage(0);
		}
	}

	if (m_gameState == GameState::ONGOING) {
		if (pKeyBuffer[VK_PAUSE] & 0xF0) {
			m_gameState = GameState::PAUSE;
		}
	}

	// Check the relevant keys
	if (!m_pPlayer->isDead)
	{
		if (pKeyBuffer[VK_UP] & 0xF0) Direction |= CPlayer::DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) Direction |= CPlayer::DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= CPlayer::DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= CPlayer::DIR_RIGHT;

		if (pKeyBuffer[VK_SPACE] & 0xF0 && m_pPlayer->frameCounter() >= 20 && m_pPlayer->gunPowerUp == 1)
		{
			fireBullet(m_pPlayer->Position(), Vec2(0, -250));
			mciSendString("play data/sounds/shoot.wav", NULL, 0, NULL);
			m_pPlayer->frameCounter() = 0;
		}
	}
	
	// Move the player
	m_pPlayer->Move(Direction);

	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	updateGameState();
	//updateLevelState();

	switch (m_gameState)
	{
	case GameState::START:
		mciSendString("play data/sounds/song.wav", NULL, 0, NULL);
		break;
	case GameState::ONGOING:
		mciSendString("stop data/sounds/song.wav", NULL, 0, NULL);
		if (!m_pPlayer->isDead)
		{
			if (!m_pPlayer->hasExploded())
			{
				if (m_pPlayer->doublerPowerUp)
				{
					powerUpDouble++;
					incrementScore ++;
					if (incrementScore % 2 == 0)
						m_scoreP1->updateScore(1);
					if (powerUpDouble == 400)
					{
						m_pPlayer->doublerPowerUp = 0;
						powerUpDouble = 0;
					}
						
				}
				else
				{
					incrementScore++;
					if (incrementScore % 10 == 0)
						m_scoreP1->updateScore(1);
				}

				if (m_pPlayer->invincibility == 1)
				{
					powerUp++;
					if (powerUp == 400)
					{
						powerUp = 0;
						m_pPlayer->invincibility = 0;
					}
				}

				if (m_pPlayer->gunPowerUp == 1)
				{
					powerUpGun++;
					if (powerUpGun == 500)
					{
						powerUpGun = 0;
						m_pPlayer->gunPowerUp = 0;
					}
				}

				if (m_pPlayer->shield == 1)
				{
					powerUpShield++;
					if (powerUpShield == 400)
					{
						powerUpShield = 0;
						m_pPlayer->shield = 0;
						m_pPlayer->invincibility = 0;
					}
				}
			}
			
			m_pPlayer->Update(m_Timer.GetTimeElapsed());
			m_pPlayer->frameCounter()++;
		}

		for (auto enem : m_enemies)
		{
			enem->Update(m_Timer.GetTimeElapsed());

			if (enem->Position().y + (enem->getSize().y / 2) >= GetSystemMetrics(SM_CYSCREEN) + 350)
			{
				enem->isDead = 1;
			}
		}

		if (Collision())
		{
			m_scoreP1->updateScore(-100);
			m_pPlayer->invincibility = 1;
		}

		addLivePower->update(m_Timer.GetTimeElapsed());
		shieldPower->update(m_Timer.GetTimeElapsed());
		gunPower->update(m_Timer.GetTimeElapsed());
		doublerPower->update(m_Timer.GetTimeElapsed());

		if (powerUpCollision(addLivePower, m_pPlayer))
		{
			
			if (m_pPlayer->getLives() < 3)
			{
				setPLives(m_pPlayer->getLives() + 1);
			}
		}

		if (powerUpCollision(doublerPower, m_pPlayer))
		{
			m_pPlayer->doublerPowerUp = 1;
		}

		if (powerUpCollision(shieldPower, m_pPlayer))
		{
			m_pPlayer->shield = 1;
			m_pPlayer->invincibility = 1;
		}

		if (powerUpCollision(gunPower, m_pPlayer))
		{
			m_pPlayer->gunPowerUp = 1;
		}

		for (auto bul : bullets)
		{
			bul->update(m_Timer.GetTimeElapsed());

			if (detectBulletCollision(bul) || bul->mPosition.y >= m_screenSize.y || bul->mPosition.y <= 0)
			{
				bullets.remove(bul);
				break;
			}
		}

		horn++;
		if(horn % 500 == 0) mciSendString("play data/sounds/car+horn+x.wav", NULL, 0, NULL);

		mciSendString("play data/sounds/car4_relanti.wav", NULL, 0, NULL);

		break;
	
	case GameState::WON:
		switch (m_levels)
		{
		case Levels::LEVEL5:
			m_scoreP1->move(Vec2(m_screenSize.x / 2, m_screenSize.y / 2 + 100));
			break;
		}
		break;
	
	case GameState::LOST:
		m_scoreP1->move(Vec2(m_screenSize.x / 2, m_screenSize.y / 2 + 100));
		break;
	}
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	int speedBackground = 25;
	m_pBBuffer->reset();
	m_imgBackgroundMenu.Paint(m_pBBuffer->getDC(), 0, 0);
	gameMenu->draw(m_gameState);
	switch (m_gameState)
	{
	case GameState::START:
		m_pPlayer->Velocity() = Vec2(0, 0);
		break;
	case GameState::ONGOING:
		scrollingBackground(speedBackground);
		livesText->draw();
		scoreText->draw();
		m_scoreP1->draw();
		if (!m_pPlayer->isDead) m_pPlayer->Draw();

		for (auto lg : m_livesGreen) lg->draw();

		for (auto bul : bullets)
		{
			bul->draw();
		}

		for (auto enem : m_enemies)
		{
			enem->Draw();
		}

		if(!addLivePower->deleted) addLivePower->draw();
		if(!shieldPower->deleted) shieldPower->draw();
		if(!gunPower->deleted) gunPower->draw();
		if(!doublerPower->deleted) doublerPower->draw();

		if (!m_pPlayer->gunPowerUp) shootText->draw();
		else shootTextSel->draw();
		
		if (!m_pPlayer->shield) shieldText->draw();
		else shieldTextSel->draw();
		
		if (!m_pPlayer->doublerPowerUp) doubleText->draw();
		else doubleTextSel->draw();

		switch (m_levels)
		{
		case Levels::LEVEL1:
			m_level1Text->draw();
			break;
			
		case Levels::LEVEL2:
			m_level2Text->draw();
			break;

		case Levels::LEVEL3:
			m_level3Text->draw();
			break;

		case Levels::LEVEL4:
			m_level4Text->draw();
			break;

		case Levels::LEVEL5:
			m_level5Text->draw();
			break;
		}
		
		break;
	case GameState::LOST:
		scrollingBackground(speedBackground);
		m_scoreP1->draw();
		m_lostSprite->draw();
		mciSendString("play data/sounds/lose.wav", NULL, 0, NULL);
		break;
	case GameState::WON:
		scrollingBackground(speedBackground);
		switch (m_levels) 
		{
		case Levels::LEVEL5:
			m_scoreP1->draw();
			m_wonSprite->draw();
			mciSendString("play data/sounds/win.wav", NULL, 0, NULL);
			break;
		}
		break;
	case GameState::PAUSE:
		livesText->draw();
		scoreText->draw();
		m_scoreP1->draw();
		for (auto lg : m_livesGreen) lg->draw();
		switch (m_levels)
		{
		case Levels::LEVEL1:
			m_level1Text->draw();
			break;

		case Levels::LEVEL2:
			m_level2Text->draw();
			break;

		case Levels::LEVEL3:
			m_level3Text->draw();
			break;

		case Levels::LEVEL4:
			m_level4Text->draw();
			break;

		case Levels::LEVEL5:
			m_level5Text->draw();
			break;
		}
		break;
	default:
		break;
	}
	
	m_pBBuffer->present();
}

void CGameApp::addEnemies(int nrEnemies, int timeVelocity, int velocity)
{	
	int velocityY, positionX, positionY, auxPositionY[1002] = {}, ok = 0, speedBackground = 10, i, j, okPos = 0, timeVelocityLocal;
	srand(time(NULL));

	velocityY = velocity;
	positionY = -100;
	auxPositionY[0] = positionY;
	for (i = 0; i < nrEnemies; i++)
	{
		if (i % 3 == 0 && i > 2)
		{
			m_enemies.push_back(new CPlayer(m_pBBuffer, "data/truck.bmp"));
		}
		else if (i % 5 == 0 && i > 2)
		{
			m_enemies.push_back(new CPlayer(m_pBBuffer, "data/police.bmp"));
		}
		else
		{
			m_enemies.push_back(new CPlayer(m_pBBuffer, "data/car2.bmp"));
		}
		
		positionX = 10;
		ok = 0;
		while (ok == 0)
		{
			positionX = rand() % GetSystemMetrics(SM_CXSCREEN);
			
			//banda 1
			if (positionX == 290 && ok == 0)
				ok = 1;
			
			//banda 2
			if (positionX == 490 && ok == 0)
				ok = 1;

			//banda 3
			if (positionX == 690 && ok == 0)
				ok = 1;

			//banda 4
			if (positionX == 890 && ok == 0)
				ok = 1;


			//banda 5
			if (positionX == 1110 && ok == 0)
				ok = 1;

			//banda 6
			if (positionX == (GetSystemMetrics(SM_CXSCREEN) - 230) && ok == 0)
				ok = 1;
		}

		m_enemies.back()->Position() = Vec2(positionX, positionY);
		m_enemies.back()->Velocity() = Vec2(0, velocityY);

		auxPositionY[i + 1] = rand() % (nrEnemies * 250) + 100;
		positionY = auxPositionY[i + 1] - (2 * auxPositionY[i + 1]);
		auxPositionY[i + 1] = positionY;
		for (j = 0; j < i + 1; j++)
		{
			okPos = 0;
			while (okPos == 0)
			{
				if (auxPositionY[j] > auxPositionY[i + 1])
				{
					if (auxPositionY[j] - auxPositionY[i + 1] > 240)
					{
						okPos = 1;
					}	
				}
				
				if (auxPositionY[j] < auxPositionY[i + 1])
				{
					if (auxPositionY[j] - auxPositionY[i + 1] < -240)
					{
						okPos = 1;
					}
				}

				if (okPos == 0)
				{
					auxPositionY[i + 1] = rand() % (nrEnemies * 250) + 100;
					positionY = auxPositionY[i + 1] - (2 * auxPositionY[i + 1]);
					auxPositionY[i + 1] = positionY;
				}
			}
		}

		if (i > 2)
		{
			int iMod = i / 2;
			if (iMod % nrEnemies == 0)
			{
				if (timeVelocityLocal <= timeVelocity)
				{
					timeVelocityLocal++;
					velocityY += 5;
				}
			}
		}
	}
	
}

void CGameApp::removeDead()
{

	if (!m_pPlayer->getLives() && !m_pPlayer->hasExploded())
	{
		fTimer = SetTimer(m_hWnd, 1, 70, NULL);
		m_pPlayer->Explode();
	}

	for (auto enem : m_enemies)
	{
		if (enem->isDead)
		{
			m_enemies.remove(enem);
			break;
		}
	}
}

bool CGameApp::Collision()
{
	for (auto enem : m_enemies)
	{
		if (!m_pPlayer->invincibility)
		{
			if (m_pPlayer->Position().x + (m_pPlayer->getSize().x / 2) > enem->Position().x - (enem->getSize().x / 2))
				if (m_pPlayer->Position().x - (m_pPlayer->getSize().x / 2) < enem->Position().x + (enem->getSize().x / 2))
					if (m_pPlayer->Position().y + (m_pPlayer->getSize().y / 2) > enem->Position().y - (enem->getSize().y / 2))
						if (m_pPlayer->Position().y - (m_pPlayer->getSize().y / 2) < enem->Position().y + (enem->getSize().y / 2))
						{
							if (!m_pPlayer->hasExploded() && m_livesGreen.size() > 0)
							{
								fTimer = SetTimer(m_hWnd, 1, 70, NULL);
								m_pPlayer->takeDamage();
								delete m_livesGreen.back();
								m_livesGreen.pop_back();
								m_pPlayer->Position() = Vec2(690, 600);
								m_pPlayer->Velocity() = Vec2(0, 0);
								enem->Explode();
								mciSendString("play data/sounds/explosion.wav", NULL, 0, NULL);
								return true;
							}
						}
		}

	}

	return false;
}

bool CGameApp::detectBulletCollision(const Sprite* bullet)
{
	for (auto enem : m_enemies)
	{
		if (bulletCollision(*bullet, *enem))
		{
			m_scoreP1->updateScore(100);
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			enem->Explode();
			mciSendString("play data/sounds/explosion.wav", NULL, 0, NULL);
			return true;
		}
	}

	return false;
}

bool CGameApp::bulletCollision(const Sprite& bullet, CPlayer& p1)
{
	if (bullet.mPosition.x >= p1.Position().x - (p1.getSize().x / 2))
		if (bullet.mPosition.x <= p1.Position().x + (p1.getSize().x / 2))
			if (bullet.mPosition.y >= p1.Position().y - (p1.getSize().y / 2))
				if (bullet.mPosition.y <= p1.Position().y + (p1.getSize().y / 2))
					return true;

	return false;

}

void CGameApp::fireBullet(const Vec2 position, const Vec2 velocity)
{

	bullets.push_back(new Sprite("data/bullet.bmp", RGB(0xff, 0x00, 0xff)));

	bullets.back()->setBackBuffer(m_pBBuffer);
	bullets.back()->mPosition = position;
	bullets.back()->mVelocity = velocity;


	if (velocity.y < 0)
	{
		bullets.back()->mPosition.y -= 75;
	}
	else
	{
		bullets.back()->mPosition.y += 75;
	}
}

void CGameApp::setPLives(int livesP1)
{
	m_pPlayer->setLives(livesP1);

	Vec2 greenPos(30, 80);
	Vec2 increment(45, 0);

	for (int it = 0; it != livesP1; ++it) {
		m_livesGreen.push_back(new Sprite("data/heart.bmp", RGB(0xff, 0x00, 0xff)));
		auto& lastGreen = m_livesGreen.back();

		lastGreen->mPosition = greenPos;
		lastGreen->mVelocity = Vec2(0, 0);
		lastGreen->setBackBuffer(m_pBBuffer);

		greenPos += increment;
	}
}

void CGameApp::updateGameState()
{
	if (m_pPlayer->isDead && m_gameState == ONGOING) {
		m_gameState = LOST;
	}
	else if (!m_enemies.size() && m_gameState == ONGOING) {
		m_gameState = WON;
	}
	else if (!m_enemies.size() && m_gameState == WON && m_levels == LEVEL1)
	{
		addEnemies(25, 3, 70);
		mciSendString("play data/sounds/car4_acc.wav", NULL, 0, NULL);
		addPowerUp(powerUp);
		m_pPlayer->Position() = Vec2(690, 600);
		m_pPlayer->Velocity() = Vec2(0, 0);
		m_levels = LEVEL2;
		levelForSave = "level2";
		m_gameState = ONGOING;
	}
	else if (!m_enemies.size() && m_gameState == WON && m_levels == LEVEL2)
	{
		addEnemies(28, 3, 75);
		mciSendString("play data/sounds/car4_acc.wav", NULL, 0, NULL);
		addPowerUp(powerUp);
		m_pPlayer->Position() = Vec2(690, 600);
		m_pPlayer->Velocity() = Vec2(0, 0);
		m_levels = LEVEL3;
		levelForSave = "level3";
		m_gameState = ONGOING;
	}
	else if (!m_enemies.size() && m_gameState == WON && m_levels == LEVEL3)
	{
		addEnemies(30, 3, 75);
		mciSendString("play data/sounds/car4_acc.wav", NULL, 0, NULL);
		addPowerUp(powerUp);
		m_pPlayer->Position() = Vec2(690, 600);
		m_pPlayer->Velocity() = Vec2(0, 0);
		m_levels = LEVEL4;
		levelForSave = "level4";
		m_gameState = ONGOING;
	}
	else if (!m_enemies.size() && m_gameState == WON && m_levels == LEVEL4)
	{
		addEnemies(35, 4, 80);
		mciSendString("play data/sounds/car4_acc.wav", NULL, 0, NULL);
		addPowerUp(powerUp);
		m_pPlayer->Position() = Vec2(690, 600);
		m_pPlayer->Velocity() = Vec2(0, 0);
		m_levels = LEVEL5;
		levelForSave = "level5";
		m_gameState = ONGOING;
	}
	else if (!m_enemies.size() && m_gameState == WON && m_levels == LEVEL5)
	{
		m_gameState = WON;
	}
	else if (m_gameState == ONGOING) {
		m_gameState = ONGOING;
	}
}

void CGameApp::scrollingBackground(int speed)
{
	int x, y, i;
	int currentSpeed = speed;
	static int currentY = m_imgBackground.Height();
	static size_t lastTime = ::GetTickCount();
	size_t currentTime = ::GetTickCount();

	if (currentTime - lastTime > 100)
	{
		lastTime = currentTime;
		currentY -= speed;
		if (currentY < 0)
			currentY = m_imgBackground.Height();

		
	}

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, currentY);
	
}

//-----------------------------------------------------------------------------
// Name : saveGame () (Private)
// Desc : Save current state of the game in a file.
//-----------------------------------------------------------------------------
void CGameApp::saveGame()
{
	std::ofstream save("savegame/savegame.save");

	save << m_pPlayer->Position().x << " " << m_pPlayer->Position().y << " " << m_pPlayer->getLives() << " ";
	save << m_scoreP1->getScore() << "\n";
	save << levelForSave << "\n";

	save << m_enemies.size() << "\n";

	save.close();
}

//-----------------------------------------------------------------------------
// Name : loadGame () (Private)
// Desc : Loads previous state of the game in a file.
//-----------------------------------------------------------------------------
void CGameApp::loadGame()
{
	std::ifstream save("savegame/savegame.save");
	while (m_enemies.size()) delete m_enemies.back(), m_enemies.pop_back();
	while (bullets.size()) delete bullets.back(), bullets.pop_back();
	while (m_livesGreen.size()) delete m_livesGreen.back(), m_livesGreen.pop_back();

	double cdx, cdy;
	int livesP, score, noEnem;
	string levelLoc;

	save >> cdx >> cdy >> livesP >> score >> levelLoc;
	m_pPlayer->Position() = Vec2(cdx, cdy);
	m_scoreP1->setScore(score);

	setPLives(livesP);

	save >> noEnem;
	addEnemies(noEnem, 2, 50);

	save.close();
	m_gameState = GameState::ONGOING;

	if (levelLoc == "level1")
	{
		m_levels = Levels::LEVEL1;
	}
	else if (levelLoc == "level2")
	{
		m_levels = Levels::LEVEL2;
	}
	else if (levelLoc == "level3")
	{
		m_levels = Levels::LEVEL3;
	}
	else if (levelLoc == "level4")
	{
		m_levels = Levels::LEVEL4;
	}
	else if (levelLoc == "level5")
	{
		m_levels = Levels::LEVEL5;
	}
}

void CGameApp::addPowerUp(int powerUp)
{
	int positionX, ok, iProv = 0, auxPositionY1 = 0, auxPositionY2 = 0, positionY = 0;
	srand(time(NULL));

	doublerPower = new Sprite("data/doubler.bmp", RGB(0xff, 0x00, 0xff));
	doublerPower->setBackBuffer(m_pBBuffer);
	addLivePower = new Sprite("data/heart.bmp", RGB(0xff, 0x00, 0xff));
	addLivePower->setBackBuffer(m_pBBuffer);
	gunPower = new Sprite("data/gun.bmp", RGB(0xff, 0x00, 0xff));
	gunPower->setBackBuffer(m_pBBuffer);
	shieldPower = new Sprite("data/shield.bmp", RGB(0xff, 0x00, 0xff));
	shieldPower->setBackBuffer(m_pBBuffer);

	positionX = 10;
	ok = 0;
	while (ok == 0)
	{
		positionX = rand() % GetSystemMetrics(SM_CXSCREEN);
		//banda 1
		if (positionX == 290 && ok == 0)
			ok = 1;

		//banda 2
		if (positionX == 490 && ok == 0)
			ok = 1;

		//banda 3
		if (positionX == 690 && ok == 0)
			ok = 1;

		//banda 4
		if (positionX == 890 && ok == 0)
			ok = 1;


		//banda 5
		if (positionX == 1110 && ok == 0)
			ok = 1;

		//banda 6
		if (positionX == (GetSystemMetrics(SM_CXSCREEN) - 230) && ok == 0)
			ok = 1;
			
	}

	//add live power up
	auxPositionY1 = rand() % 10000 + 100;
	positionY = auxPositionY1 - (2 * auxPositionY1);

	addLivePower->mPosition = Vec2(positionX, positionY);
	addLivePower->mVelocity = Vec2(0, 40);
	
	//shield power up - invincibility
	auxPositionY2 = rand() % 10000 + 100;
	while (auxPositionY2 == auxPositionY1)
		auxPositionY2 = rand() % 10000 + 100;
	positionY = auxPositionY2 - (2 * auxPositionY2);
	auxPositionY1 = auxPositionY2;

	shieldPower->mPosition = Vec2(positionX, positionY);
	shieldPower->mVelocity = Vec2(0, 40);

	//gun power up - you can shoot
	auxPositionY2 = rand() % 10000 + 100;
	while (auxPositionY2 == auxPositionY1)
		auxPositionY2 = rand() % 10000 + 100;
	positionY = auxPositionY2 - (2 * auxPositionY2);
	auxPositionY1 = auxPositionY2;

	gunPower->mPosition = Vec2(positionX, positionY);
	gunPower->mVelocity = Vec2(0, 40);

	//doubler power up - double your points
	auxPositionY2 = rand() % 10000 + 100;
	while (auxPositionY2 == auxPositionY1)
		auxPositionY2 = rand() % 10000 + 100;
	positionY = auxPositionY2 - (2 * auxPositionY2);

	doublerPower->mPosition = Vec2(positionX, positionY);
	doublerPower->mVelocity = Vec2(0, 40);
}

bool CGameApp::powerUpCollision(Sprite* powerUp, CPlayer* p1)
{
	if (powerUp->mPosition.x >= p1->Position().x - (p1->getSize().x / 2))
		if (powerUp->mPosition.x <= p1->Position().x + (p1->getSize().x / 2))
			if (powerUp->mPosition.y >= p1->Position().y - (p1->getSize().y / 2))
				if (powerUp->mPosition.y <= p1->Position().y + (p1->getSize().y / 2))
				{
					mciSendString("play data/sounds/power_up.wav", NULL, 0, NULL);
					powerUp->deleted = 1;
					return true;
				}
					

	return false;

}