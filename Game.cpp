#include "Game.h"

#include <cmath>



Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initUI();
	this->initBoard();
}

Game::~Game()
{
	this->cleanup();
	delete this->window;
}

void Game::run()
{
	while (this->getWindowIsOpen())
	{
		// Update
		this->update();

		// Render
		this->render();

	}
}

void Game::cleanup() {
	for (auto row : board) {
		for (auto p : row) {
			if (!p) continue;
			delete p;
		}
	}

	pickedPiece = nullptr;
	whiteKing = nullptr;
	blackKing = nullptr;

	delete possibleMoves;
	possibleMoves = nullptr;
}

void Game::restart()
{
	this->cleanup();
	this->initVariables();
	this->initBoard();

	turnText.setString("Whites' turn");
}

/*
	Getters
*/

const bool Game::getWindowIsOpen() const
{
	return this->window->isOpen();
}

/*
	Initializers
*/

void Game::initVariables()
{
	this->tileSize = 150;
	this->tileSizef = float(tileSize);
	this->boardSize = sf::Vector2f(8 * tileSizef, 8 * tileSizef);
	this->margin = sf::Vector2f(50.f, 50.f);

	this->pickedPiece = nullptr;
	this->possibleMoves = new std::vector<sf::Vector2i>;
	this->turn = PieceColor::WHITE;

	this->isWhiteCheck = false;
	this->isBlackCheck = false;
	this->isCheckmate = false;
}

void Game::initWindow()
{
	// developed on a 2880x1800 screen
	this->videoMode.height = (tileSize * 8 + margin.y * 2) * (static_cast<float>(sf::VideoMode::getDesktopMode().width) / 2880.f);
	this->videoMode.width = (tileSize * 8 + margin.x * 2) * (static_cast<float>(sf::VideoMode::getDesktopMode().height) / 1800.f);

	this->window = new sf::RenderWindow(this->videoMode, "Chess");
	window->setFramerateLimit(10);
}


void Game::initUI()
{
	this->initTextures();

	if (!font.loadFromFile("Fonts/Pixel NES.otf"))
		std::cout << "Failed to load font\n";

	boardView.setSize(tileSizef * 8.f + margin.x * 2, tileSizef * 8.f + margin.y * 2);
	boardView.setCenter(boardView.getSize().x / 2.f - margin.x, boardView.getSize().y / 2.f -margin.y);

	turnText.setFont(font);
	turnText.setCharacterSize(36);
	turnText.setFillColor(sf::Color::White);
	turnText.setString("Whites' turn");
	turnText.setOrigin(turnText.getGlobalBounds().width / 2.f, turnText.getGlobalBounds().height / 2.f);
	turnText.setPosition(boardSize.x / 2.f, -margin.y / 2.f - turnText.getGlobalBounds().height / 2.f);

	// Overlay
	this->overlay.setSize(boardSize);
	this->overlay.setFillColor(sf::Color(0, 0, 0, 150));

	// Checkmate screen
	this->checkmateText.setFont(this->font);
	this->checkmateText.setCharacterSize(96);
	this->checkmateText.setFillColor(sf::Color::White);
	this->checkmateText.setString("CHECKMATE!");
	this->checkmateText.setOrigin(this->checkmateText.getGlobalBounds().width / 2.f, this->checkmateText.getGlobalBounds().height);
	this->checkmateText.setPosition(boardSize.x / 2.f, boardSize.y / 2.f - 100.f);
	
	this->winnerText.setFont(this->font);
	this->winnerText.setCharacterSize(64);
	this->winnerText.setFillColor(sf::Color::White);
	this->winnerText.setString("whites win");
	this->winnerText.setOrigin(this->winnerText.getGlobalBounds().width / 2.f, this->winnerText.getGlobalBounds().height);
	this->winnerText.setPosition(boardSize.x / 2.f, boardSize.y / 2.f - 10.f);

	this->tooltipText.setFont(this->font);
	this->tooltipText.setCharacterSize(48);
	this->tooltipText.setFillColor(sf::Color::White);
	this->tooltipText.setString("[esc] to quit\t[r] to restart");
	this->tooltipText.setOrigin(this->tooltipText.getGlobalBounds().width / 2.f, 0.f);
	this->tooltipText.setPosition(boardSize.x / 2.f, boardSize.y / 2.f + 40.f);

	// cursor
	cursor.setSize(sf::Vector2f(tileSizef, tileSizef));
	cursor.setFillColor(sf::Color::Transparent);
	cursor.setOutlineColor(sf::Color(103, 232, 15, 200));
	cursor.setOutlineThickness(-5.f);

	// picked piece cursor
	pickedPieceCursor.setSize(sf::Vector2f(tileSizef, tileSizef));
	pickedPieceCursor.setFillColor(sf::Color(103, 232, 230, 100));
	// pickedPieceCursor.setFillColor(sf::Color::Transparent);
	pickedPieceCursor.setOutlineColor(sf::Color(103, 232, 230, 200));
	pickedPieceCursor.setOutlineThickness(-5.f);

	bool prevWhite = false;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			tiles[i][j] = new sf::RectangleShape();
			tiles[i][j]->setSize(sf::Vector2f(tileSizef, tileSizef));
			tiles[i][j]->setFillColor(prevWhite ? sf::Color(88, 50, 11) : sf::Color(231, 198, 165));
			tiles[i][j]->setPosition(tileSizef * j, tileSizef * i);
			prevWhite = !prevWhite;
		}
		prevWhite = !prevWhite;
	}
}

void Game::initTextures()
{
	this->textures["pieces"] = new sf::Texture();
	if (!this->textures["pieces"]->loadFromFile("Textures/pieces.png"))
		std::cout << "Failed to load pieces texture";
}

void Game::initBoard()
{
	int boardTemplate[8][8] = {
		{-5,-4,-3,-2,-1,-3,-4,-5},
		{-6,-6,-6,-6,-6,-6,-6,-6},
		{ 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0},
		{ 6, 6, 6, 6, 6, 6, 6, 6},
		{ 5, 4, 3, 2, 1, 3, 4, 5},
	};

	int k = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			int n = boardTemplate[i][j];
			if (!n) {
				board[i][j] = nullptr;
				continue;
			}

			PieceColor color = n < 0 ? PieceColor::BLACK : PieceColor::WHITE;
			switch (abs(n))
			{
			case 1:
			{
				King* king = new King(color, textures["pieces"], tileSizef);

				if (color == PieceColor::WHITE) whiteKing = king;
				else blackKing = king;

				board[i][j] = king;
				break;
			}
			case 2:
				board[i][j] = new Queen(color, textures["pieces"], tileSizef);
				break;
			case 3:
				board[i][j] = new Bishop(color, textures["pieces"], tileSizef);
				break;
			case 4:
				board[i][j] = new Knight(color, textures["pieces"], tileSizef);
				break;
			case 5:
				board[i][j] = new Rook(color, textures["pieces"], tileSizef);
				break;
			case 6:
				board[i][j] = new Pawn(color, textures["pieces"], tileSizef);;
				break;
			}

			board[i][j]->moveToTile(sf::Vector2i(j, i));
		}
	}
}

/*
	Methods
*/

void Game::pollEvents()
{
	// Event Polling
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type) {
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::Resized:
			boardView = getLetterboxView(boardView, ev.size.width, ev.size.height);
			break;
		case sf::Event::KeyPressed:
			switch (this->ev.key.code) {
			case sf::Keyboard::Escape:
				this->window->close();
				break;
			case sf::Keyboard::R:
				this->restart();
				break;
			}
		}
	}

}

void Game::handleTurnChange() {
	turn = oppositeColor(turn);
	if(turn == PieceColor::WHITE)
		turnText.setString("Whites' turn");
	else
		turnText.setString("Blacks' turn");
}

bool Game::isTileKing(sf::Vector2i tile) {
	return !!board[tile.y][tile.x] && board[tile.y][tile.x]->type == PieceType::KING;
}

bool Game::isMoveInvalid(Piece* piece, sf::Vector2i move) {
	if (piece == nullptr) return true;

	auto prevTile = piece->getTile();
	auto movesTile = board[move.y][move.x];

	board[prevTile.y][prevTile.x] = nullptr;
	piece->setTile(move);
	board[move.y][move.x] = piece;
	updateIsCheck();

	bool res = false;
	if (
		(piece->color == PieceColor::WHITE && isWhiteCheck) ||
		(piece->color == PieceColor::BLACK && isBlackCheck)
		) res = true;

	piece->setTile(prevTile);
	board[prevTile.y][prevTile.x] = piece;
	board[move.y][move.x] = movesTile;
	updateIsCheck();

	return res;
}

void Game::getPiecePossibleMoves(Piece* piece, std::vector<sf::Vector2i>* moves) {
	if (pickedPiece == nullptr) return;

	moves->clear();

	piece->getMoves(board, moves);

	moves->erase(
		std::remove_if(
			moves->begin(), moves->end(),
			[this, piece](sf::Vector2i& move) {return this->isMoveInvalid(piece, move); }
		),
		moves->end()
	);
}

void Game::setPossibleMoves() {
	if (pickedPiece == nullptr) return;

	getPiecePossibleMoves(pickedPiece, possibleMoves);
}

void Game::updateInput()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (!mousePressed) {
			mousePressed = true;
			Piece* newPiece = board[mousePosTile.y][mousePosTile.x];

			if (pickedPiece == nullptr) {
				if (!newPiece || newPiece->color != turn) return;
				pickedPiece = newPiece;
				pickedPieceCursor.setPosition(mousePosTile.x * tileSizef, mousePosTile.y * tileSizef);
				setPossibleMoves();
			}
			else if (newPiece == pickedPiece) 
				pickedPiece = nullptr;
			else {
				for (auto move : *possibleMoves) {
					if (isTileKing(move)) continue;
					if (move == mousePosTile) {
						board[pickedPiece->getTile().y][pickedPiece->getTile().x] = nullptr;
						pickedPiece->moveToTile(move);
						board[move.y][move.x] = pickedPiece;
						handleTurnChange();
						updateIsCheck();
						updateIsCheckmate();
						break;
					}
				}
				pickedPiece = nullptr;
			}
		}
	}
	else mousePressed = false;
}

void Game::updateMousePos()
{
	mousePosWindow = sf::Mouse::getPosition(*window);

	mousePosBoard = window->mapPixelToCoords(mousePosWindow, boardView);
	if (mousePosBoard.x < 0) mousePosBoard.x = 0;
	if (mousePosBoard.y < 0) mousePosBoard.y = 0;
	if (mousePosBoard.x > boardSize.x) mousePosBoard.x = boardSize.x - tileSizef;
	if (mousePosBoard.y > boardSize.y) mousePosBoard.y = boardSize.y - tileSizef;

	mousePosTile = sf::Vector2i(int(mousePosBoard.x) / tileSize, int(mousePosBoard.y) / tileSize);

	cursor.setPosition(mousePosTile.x * tileSizef, mousePosTile.y * tileSizef);
}

void Game::updateIsCheck() {
	isWhiteCheck = false;
	isBlackCheck = false;

	std::vector<sf::Vector2i>* moves = new std::vector<sf::Vector2i>;
	for (auto row : board) {
		for (auto p : row) {
			if (!p) continue;

			p->getMoves(board, moves);
			for (auto m : *moves) {
				if (isTileKing(m)) {
					if (p->color == PieceColor::WHITE) isBlackCheck = true;
					else isWhiteCheck = true;
				}
			}
		}
	}

	delete moves;
}

void Game::updateIsCheckmate() {
	if (!isWhiteCheck && !isBlackCheck) return;

	if (isWhiteCheck && !anyPossibleMoves(PieceColor::WHITE)) {
		isCheckmate = true;
		this->winnerText.setString("blacks win");
	}
	else if (isBlackCheck && !anyPossibleMoves(PieceColor::BLACK)) {
		isCheckmate = true;
		this->winnerText.setString("whites win");
	}
}

bool Game::anyPossibleMoves(PieceColor color) {

	std::vector<sf::Vector2i>* moves = new std::vector<sf::Vector2i>;
	for (auto row : board) {
		for (auto p : row) {
			if (!p || p->color != color) continue;

			getPiecePossibleMoves(p, moves);
			if (!!moves->size()) 
				return true;
		}
	}

	delete moves;

	return false;
}

void Game::update()
{
	this->pollEvents();
	if (isCheckmate) return;

	this->updateMousePos();
	this->updateInput();
}

/// RENDER

void Game::render()
{
	//window->clear(sf::Color(25, 25, 25, 1)); // Clear previous frame
	window->clear(sf::Color::Black); // Clear previous frame

	window->setView(boardView);

	// Draw tiles
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			window->draw(*tiles[i][j]);
		}
	}

	renderPossibleMoves();

	renderChecks();

	if (pickedPiece != nullptr) {
		window->draw(pickedPieceCursor);
	}

	// render pieces
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == nullptr) continue;
			window->draw(*board[i][j]);
		}
	}

	window->draw(cursor);

	// window->setView(window->getDefaultView());

	renderText();

	window->display();
}

void Game::renderText() {
	window->draw(turnText);

	if (isCheckmate) {
		window->draw(overlay);
		window->draw(checkmateText);
		window->draw(winnerText);
		window->draw(tooltipText);
	}
}

void Game::renderPossibleMoves() {
	if (pickedPiece == nullptr) return;

	for (auto move : *possibleMoves) {
		sf::RectangleShape tile = sf::RectangleShape();
		tile.setSize(sf::Vector2f(tileSizef, tileSizef));
		tile.setFillColor(
			isTileKing(move)
			? sf::Color(230, 100, 103, 200) 
			: sf::Color(103, 232, 230, 100)
		);
		tile.setPosition(sf::Vector2f(tileSizef * move.x, tileSizef * move.y));
		window->draw(tile);
	}
}

void Game::renderChecks() {
	sf::RectangleShape tile = sf::RectangleShape();
	tile.setSize(sf::Vector2f(tileSizef, tileSizef));
	tile.setFillColor(sf::Color(230, 100, 103, 200));

	if (!!whiteKing && isWhiteCheck) {
		tile.setPosition(whiteKing->getPosition());
		window->draw(tile);
	}
	
	if (!!blackKing && isBlackCheck) {
		tile.setPosition(blackKing->getPosition());
		window->draw(tile);
	}
}

/*
	UTILS
*/

sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {

	// Compares the aspect ratio of the window to the aspect ratio of the view,
	// and sets the view's viewport accordingly in order to archieve a letterbox effect.
	// A new view (with a new viewport set) is returned.

	float windowRatio = windowWidth / (float)windowHeight;
	float viewRatio = view.getSize().x / (float)view.getSize().y;
	float sizeX = 1;
	float sizeY = 1;
	float posX = 0;
	float posY = 0;

	bool horizontalSpacing = true;
	if (windowRatio < viewRatio)
		horizontalSpacing = false;

	// If horizontalSpacing is true, the black bars will appear on the left and right side.
	// Otherwise, the black bars will appear on the top and bottom.

	if (horizontalSpacing) {
		sizeX = viewRatio / windowRatio;
		posX = (1 - sizeX) / 2.f;
	}

	else {
		sizeY = windowRatio / viewRatio;
		posY = (1 - sizeY) / 2.f;
	}

	view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

	return view;
}