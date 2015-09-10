#include "player.hpp"
#include <cstdlib>
#include <math.h>
#include <limits>
#include <chrono>


namespace checkers
{

	GameState Player::play(const GameState &pState, const Deadline &pDue)
	{
		//std::cerr << "Processing " << pState.toMessage() << std::endl;
		std::string StateKey = pState.toMessage();
		if (!nextStates.count(StateKey)) pState.findPossibleMoves(nextStates[StateKey]);

		//if (lNextStates.size() == 0) return GameState(pState, Move());
		if (nextStates[StateKey].size() == 0) return GameState(pState, Move());

		/*
		* Here you should write your clever algorithms to get the best next move, ie the best
		* next state. This skeleton returns a random move instead.
		*/

		//Determine player's color.
		uint8_t nextPlayer = pState.getNextPlayer();
		if (nextPlayer&CELL_RED) color = 1;
		else if (nextPlayer&CELL_WHITE) color = -1;

		//Initialize move choice and move value.
		unsigned int move;

		//Must move in less than one second.
		double time_left_before = 1.0;

		//Iterative deepening
		for (int d = 1; d < 9; d++)
		{
			//Initialize value and move.
			double value = -1 * std::numeric_limits<double>::infinity();
			move = 0;

			//Initialize alpha and beta values to minus and plus infinity respectively.
			double alpha = -1 * std::numeric_limits<double>::infinity();
			double beta = std::numeric_limits<double>::infinity();

			//Time now
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

			for (unsigned int m = 0; m < nextStates[StateKey].size(); m++)
			{
				//Select move if its a win.
				if ((color == 1 && pState.isRedWin()) || (color == -1 && pState.isWhiteWin()))
					return nextStates[StateKey][m];

				float child_value = Player::MiniMaxAB(nextStates[StateKey][m], d, alpha, beta, false);
				if (child_value > value)
				{
					value = child_value;
					move = m;
				}
			}

			//Return move if there is not enough time for the next iteration.
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			double time_left = time_left_before - time_span.count();
			if (6 * (time_left_before - time_left) > time_left) return nextStates[StateKey][move];
			time_left_before -= time_span.count();

			//Sort children based on their value (highest first).
		}
		return nextStates[StateKey][move];
	}

	double Player::MiniMaxAB(const GameState &pState, int depth, double alpha, double beta, bool maxPlayer)
	{
		//Node StateKey
		std::string StateKey = pState.toMessage();

		if (!depth)
		{
			if (!gameValues.count(StateKey)) Player::StaticGameValue(pState, StateKey);
			return gameValues[StateKey];
		}
		else if (pState.isDraw()) return 0.0;
		else if (pState.isRedWin())
		{
			if (color == 1) return 10000.0;
			else return -10000.0;
		}
		else if (pState.isWhiteWin())
		{
			if (color == -1) return 10000.0;
			else return -10000.0;
		}
		else
		{
			//Get GameState children (possible next moves).
			if (!nextStates.count(StateKey))
				pState.findPossibleMoves(nextStates[StateKey]);

			//Initialize value to minus/plus infinity.
			double value;
			if (maxPlayer) value = -1 * std::numeric_limits<double>::infinity();
			else value = std::numeric_limits<double>::infinity();

			for (unsigned int i = 0; i < nextStates[StateKey].size(); i++)
			{
				//Increase depth by one when whe there is only one possible move (explored beyond forzed moves).
				double next_depth;
				if (nextStates[StateKey].size() == 1) next_depth = (depth);
				else next_depth = (depth - 1);

				//Get child value
				double child_value = Player::MiniMaxAB(nextStates[StateKey][i], next_depth, alpha, beta, !maxPlayer);

				if (maxPlayer)
				{
					//Update value and alpha.
					value = std::max(value, child_value);
					alpha = std::max(value, alpha);

					//Beta cut-off
					if (beta <= alpha) break;
				}
				else
				{
					//Update value and beta.
					value = std::min(value, child_value);
					beta = std::min(value, beta);

					//Alpha cut-off
					if (beta <= alpha) break;
				}

			}

			return value;
		}
	}

	void Player::StaticGameValue(const GameState &pState, const std::string StateKey)
	{
		//Points awarded for regular pieces (zero-zum).
		//Points for regular pieces stored at index 0.
		//Points for king pieces stored at index 1.
		int materialPoints[2] = { 0, 0 };
		Player::materialValue(pState, materialPoints);

		//Moves left until draw.
		int movesLeft = (int)pState.getMovesUntilDraw();

		//Available moves.
		if (!nextStates.count(StateKey)) pState.findPossibleMoves(nextStates[StateKey]);
		int availableMoves = nextStates[StateKey].size();

		//Heuristic (linear polynomial).
		gameValues[StateKey] = B1 * color * materialPoints[0] + B2 * color * materialPoints[1] + B3 * availableMoves + (B4 / movesLeft);
	}

	void Player::materialValue(const GameState &pState, int materialPoints[])
	{

		//There are 31 cells in the board.
		for (int i = 0; i < 32; i++)
		{

			//Points awarded depending on the piece position on the board.
			int points = positionPoints[i];

			//If the cell is occupied by white, increment points.
			if (pState.at(i)&CELL_RED)
			{
				if (pState.at(i)&CELL_KING) materialPoints[1] += points;
				else materialPoints[0] += points;
			}
			//If the piece belongs to the oponent, substract points as appropiate.
			else if (pState.at(i)&CELL_WHITE)
			{
				if (pState.at(i)&CELL_KING) materialPoints[1] -= points;
				else materialPoints[0] -= points;
			}
		}

	}

	/*namespace checkers*/
}