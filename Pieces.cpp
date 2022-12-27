#include "Pieces.h"

const float spriteSize = 426;

PieceColor oppositeColor(PieceColor color) {
	return color == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE;
}

/*
	PIECE - base class
*/

Piece::Piece(PieceColor color, float size) : color{ color }, size{ size } {
	this->setScale(sf::Vector2f(size / spriteSize, size / spriteSize));
}

bool Piece::isMoveGood(sf::Vector2i move, std::array<std::array<Piece*, 8>, 8>& board) {
	if (move.x < 0 || move.x > 7 || move.y < 0 || move.y > 7) return false;

	if (board[move.y][move.x] != nullptr && board[move.y][move.x]->color == this->color) return false;

	return true;
}

bool Piece::isEnemyOnTile(sf::Vector2i tile, std::array<std::array<Piece*, 8>, 8>& board) {
	if (!isMoveGood(tile, board)) return false;

	return board[tile.y][tile.x] != nullptr && board[tile.y][tile.x]->color == oppositeColor(this->color);
}

sf::Vector2i Piece::getTile() {
	return this->tile;
}

void Piece::setTile(sf::Vector2i t) {
	this->tile = t;
}

/*
	KING
*/

King::King(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::KING;

	this->setTextureRect(
		sf::IntRect(
			0,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
}

void Piece::getKingMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	int movesArr[3] = { -1, 0, 1 };

	sf::Vector2i m{};
	for (auto x : movesArr) {
		for (auto y : movesArr) {
			if (!x && !y) continue; // current position

			m = sf::Vector2i(tile.x + x, tile.y + y);
			if (isMoveGood(m, board)) {
				moves->push_back(m);
			}
		}
	}
}

void King::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	getKingMoves(board, moves);
}

void King::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;
}

/*
	QUEEN
*/

Queen::Queen(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::QUEEN;

	this->setTextureRect(
		sf::IntRect(
			spriteSize,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
}

void Queen::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	getKingMoves(board, moves);
	getBishopMoves(board, moves);
	getRookMoves(board, moves);

	// filter out duplicate moves
	auto end = moves->end();
	for (auto it = moves->begin(); it != end; ++it) {
		end = std::remove(it + 1, end, *it);
	}

	moves->erase(end, moves->end());
}

void Queen::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;
}

/*
	BISHOP
*/

Bishop::Bishop(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::BISHOP;

	this->setTextureRect(
		sf::IntRect(
			spriteSize * 2,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
}

void Piece::getBishopMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	std::string movesArr[4] = {
		"-1:-1", // UP LEFT
		"1:-1", // UP RIGHT
		"1:1", // DOWN RIGHT
		"-1:1" // DOWN LEFT
	};

	int i{};
	sf::Vector2i m{};
	for (auto s : movesArr) {
		int x = std::stoi(s.substr(0, s.find(':')));
		int y = std::stoi(s.substr(s.find(':') + 1));

		i = 1;
		m = sf::Vector2i(tile.x + i * x, tile.y + i * y);
		while (isMoveGood(m, board)) {
			moves->push_back(m);
			if (isEnemyOnTile(m, board)) break;
			++i;
			m = sf::Vector2i(tile.x + i * x, tile.y + i * y);
		}
	}
}

void Bishop::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	getBishopMoves(board, moves);
}

void Bishop::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;
}

/*
	KNIGHT
*/

Knight::Knight(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::KNIGHT;

	this->setTextureRect(
		sf::IntRect(
			spriteSize * 3,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
}

void Knight::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	std::string movesArr[8] = {
			"-2:-1",	"1:-2",
		"-1:-2",			"2:-1",
		//			 K	
		"-2:1",				"2:1",
			"-1:2",		"1:2",
	};

	for (auto s : movesArr) {
		int x = std::stoi(s.substr(0, s.find(':')));
		int y = std::stoi(s.substr(s.find(':') + 1));

		sf::Vector2i m = sf::Vector2i(tile.x + x, tile.y + y);
		if ((isMoveGood(sf::Vector2i(m), board))) {
			moves->push_back(m);
		}
	}

}

void Knight::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;
}

/*
	ROOK
*/

Rook::Rook(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::ROOK;

	this->setTextureRect(
		sf::IntRect(
			spriteSize * 4,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
}

void Piece::getRookMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	std::string movesArr[4] = {
		"-1:0", // LEFT
		"0:-1", // UP
		"1:0", // RIGHT
		"0:1" // DOWN
	};

	int i{};
	sf::Vector2i m{};
	for (auto s : movesArr) {
		int x = std::stoi(s.substr(0, s.find(':')));
		int y = std::stoi(s.substr(s.find(':') + 1));

		i = 1;
		m = sf::Vector2i(tile.x + i * x, tile.y + i * y);
		while (isMoveGood(m, board)) {
			moves->push_back(m);
			if (isEnemyOnTile(m, board)) break;
			++i;
			m = sf::Vector2i(tile.x + i * x, tile.y + i * y);
		}
	}
}

void Rook::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	getRookMoves(board, moves);
}

void Rook::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;
}

/*
	PAWN
*/

Pawn::Pawn(PieceColor color, sf::Texture* texture, float size) : Piece(color, size) {
	this->type = PieceType::PAWN;

	this->setTextureRect(
		sf::IntRect(
			spriteSize * 5,
			color == PieceColor::WHITE ? 0 : spriteSize,
			spriteSize,
			spriteSize
		)
	);
	this->setTexture(*texture);
	this->hasMoved = false;
	this->initialMove = true;
}

void Pawn::getMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves) {
	moves->clear();

	int dir = color == PieceColor::WHITE ? -1 : 1;

	// go forawrd
	sf::Vector2i m1 = sf::Vector2i(tile.x, tile.y + 1 * dir);
	if (isMoveGood(m1, board) && board[m1.y][m1.x] == nullptr) {
		moves->push_back(m1);
		sf::Vector2i m2 = sf::Vector2i(tile.x, tile.y + 2 * dir);
		if (!hasMoved && isMoveGood(m2, board) && board[m2.y][m2.x] == nullptr) {
			moves->push_back(m2);
		}
	}

	// diagonal left
	sf::Vector2i dl = sf::Vector2i(tile.x - 1, tile.y + 1 * dir);
	if (isEnemyOnTile(dl,board)) {
		moves->push_back(dl);
	}
	// diagonal right
	sf::Vector2i dr = sf::Vector2i(tile.x + 1, tile.y + 1 * dir);
	if (isEnemyOnTile(dr,board)) {
		moves->push_back(dr);
	}
}

void Pawn::moveToTile(sf::Vector2i move) {
	this->setPosition(sf::Vector2f(size * move.x, size * move.y));
	this->tile = move;

	if (initialMove) initialMove = false;
	else if (!hasMoved) hasMoved = true;
}