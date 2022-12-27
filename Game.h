#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>

#include <array>
#include <iostream>
#include <algorithm>

#include "Pieces.h"

/*
	Class that acts as a game engine.
*/

class Game
{
private:
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event ev;

	void initVariables();
	void initWindow();

	// Game logic
	void initBoard();
	std::array<std::array<Piece*, 8>, 8> board;

	Piece* pickedPiece;
	std::vector<sf::Vector2i>* possibleMoves;
	void setPossibleMoves();

	PieceColor turn;
	void handleTurnChange();
	bool isTileKing(sf::Vector2i tile);

	King* whiteKing;
	bool isWhiteCheck;
	King* blackKing;
	bool isBlackCheck;
	void updateIsCheck();

	bool isCheckmate;
	void updateIsCheckmate();
	bool anyPossibleMoves(PieceColor color);
	void getPiecePossibleMoves(Piece* piece, std::vector<sf::Vector2i>* moves);

	void renderChecks();
	bool isMoveInvalid(Piece* piece, sf::Vector2i move);

	// UI
	void initUI();
	std::map<std::string, sf::Texture*> textures;
	void initTextures();

	sf::Font font;
	sf::Text turnText;
	sf::Text checkmateText;
	sf::Text winnerText;
	sf::Text tooltipText;
	sf::RectangleShape overlay;
	void renderText();

	sf::View boardView;
	int tileSize;
	float tileSizef;
	std::array<std::array<sf::RectangleShape*, 8>, 8> tiles;
	sf::Vector2f boardSize;
	sf::Vector2f margin;
	sf::RectangleShape pickedPieceCursor;

	// Mouse
	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosBoard;
	sf::Vector2i mousePosTile;
	sf::RectangleShape cursor;
	void updateMousePos();
	void updateInput();
	bool mousePressed;

	void renderPossibleMoves();

	void cleanup();
public:
	Game();
	~Game();

	// Getters
	const bool getWindowIsOpen() const;

	// Methods
	void run();
	void restart();
	void pollEvents();
	void update();
	void render();
};

// utils
sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);