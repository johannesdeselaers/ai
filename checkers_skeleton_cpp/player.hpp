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
		const double B1 = 1.0; //Pawn pieces
		const double B2 = 2.0; //King pieces
		const double B3 = 1.0; //Available moves
		const double B4 = 0.0; //Moves until draw

		//Points awarded depending on the piece position on the board.
		//http://ai-depot.com/articles/minimax-explained/3/
		std::unordered_map<int, int> positionPoints;

		//Some hash maps to speed things up a bit.
		std::unordered_map<std::string, std::vector<GameState>> nextStates;
		std::unordered_map<std::string, double> gameValues;

		//Constructor
		Player()
		{
			//Assign values to board positions.
			for (int i = 0; i < 32; i++)
			{
				if (i < 5) positionPoints[i] = 4;
				else if (i < 9) positionPoints[i] = 3;
				else if (i < 9) positionPoints[i] = 3;
				else if (i < 11) positionPoints[i] = 2;
				else if (i < 13) positionPoints[i] = 4;
				else if (i < 14) positionPoints[i] = 2;
				else if (i < 15) positionPoints[i] = 1;
				else if (i < 17) positionPoints[i] = 1;
				else if (i < 18) positionPoints[i] = 3;
				else if (i < 19) positionPoints[i] = 2;
				else if (i < 21) positionPoints[i] = 4;
				else if (i < 23) positionPoints[i] = 2;
				else if (i < 27) positionPoints[i] = 3;
				else positionPoints[i] = 4;
			}
		}

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