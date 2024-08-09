// Debug memory leaks
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

// Include project
#include "Game.h"

// Include console I/O methods (conio.h for windows)
#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif

// Enable dedicated graphics card
extern "C"
{
	_declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(void)
{
	// Check for memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		// Call game constructor
		Game game;

		// Game loop
		while (game.m_window)
			game.Update();
	}

	return 0;
}