#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <unordered_set>

enum PieceType {
	KING = 0,
	QUEEN,
	BISHOP,
	KNIGHT,
	ROOK,
	PAWN,
};

enum PieceColor {
	WHITE = 0,
	BLACK
};

class Piece : public sf::Sprite
{
public:
		PieceType type;
		PieceColor color;
		float size;

		sf::Vector2i tile;
		sf::Vector2i getTile();
		void setTile(sf::Vector2i t);

		virtual void moveToTile(sf::Vector2i) = 0;
		virtual void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*) = 0;
		Piece(PieceColor color, float size);

		bool isMoveGood(sf::Vector2i move, std::array<std::array<Piece*, 8>, 8>& board);
		bool isEnemyOnTile(sf::Vector2i tile, std::array<std::array<Piece*, 8>, 8>& board);

		// Separate these methods, because Queen is a combination of all of them
		void getKingMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves);
		void getBishopMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves);
		void getRookMoves(std::array<std::array<Piece*, 8>, 8>& board, std::vector<sf::Vector2i>* moves);
};

// KING
class King : public Piece
{
private:
public:
	King(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

// QUEEN
class Queen : public Piece
{
private:
public:
	Queen(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

// BISHOP
class Bishop : public Piece
{
private:
public:
	Bishop(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

// KNIGHT
class Knight : public Piece
{
private:
public:
	Knight(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

// ROOK
class Rook : public Piece
{
private:
public:
	Rook(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

// PAWN
class Pawn : public Piece
{
private:
	bool hasMoved;
	bool initialMove;
public:
	Pawn(PieceColor color, sf::Texture* texture, float size);
	void getMoves(std::array<std::array<Piece*, 8>, 8>&, std::vector<sf::Vector2i>*);
	void moveToTile(sf::Vector2i);
};

PieceColor oppositeColor(PieceColor color);