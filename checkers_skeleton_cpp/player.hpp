#ifndef _CHECKERS_PLAYER_HPP_
#define _CHECKERS_PLAYER_HPP_

#include "constants.hpp"
#include "deadline.hpp"
#include "move.hpp"
#include "gamestate.hpp"
#include <vector>
#include <unordered_map>

namespace checkers
{

	class Player
	{
	public:
		///perform a move
		///\param pState the current state of the board
		///\param pDue time before which we must have returned
		///\return the next state the board is in after our move
		GameState play(const GameState &pState, const Deadline &pDue);

		//Player's color (1 for red, -1 for white).
		int color;

		//Scoring parameters
		const double B0 = 0.0; //Constant
		const double B1 = 1.0; //Pawn pieces
		const double B2 = 2.0; //King pieces
		const double B3 = 0.02; //Moves until draw
		const double B4 = 0.1; //Available moves

		//Some hash maps to speed things up a bit.
		std::unordered_map<std::string, std::vector<GameState>> nextStates;
		std::unordered_map<std::string, double> gameValues;

		//Scoring function
		void StaticGameValue(const GameState &pState, const std::string StateKey);

		//Points awarded for material (zero-sum).
		void materialValue(const GameState &pState, int materialPoints[]);

		//MiniMax algorithm with Alpha Beta pruning.
		double MiniMaxAB(const GameState &pState, int depth, double alpha, double beta, bool maxPlayer);
	};

	/*namespace checkers*/
}

#endif