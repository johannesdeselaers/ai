#include "player.hpp"
#include <cstdlib>
#include <math.h>
#include <limits>

using namespace std;
using namespace chrono;

namespace checkers
{
	Player::Player()
	{
		//Assign values to board positions.
		for (int i = 0; i < 32; i++)
		{
			if (i < 5) positionPoints[i] = 4;
			else if (i < 9) positionPoints[i] = 3;
			else if (i < 11) positionPoints[i] = 2;
			else if (i < 13) positionPoints[i] = 4;
			else if (i < 14) positionPoints[i] = 2;
			else if (i < 15) positionPoints[i] = 1;
			else if (i < 17) positionPoints[i] = 3;
			else if (i < 18) positionPoints[i] = 1;
			else if (i < 19) positionPoints[i] = 2;
			else if (i < 21) positionPoints[i] = 4;
			else if (i < 23) positionPoints[i] = 2;
			else if (i < 27) positionPoints[i] = 3;
			else positionPoints[i] = 4;
		}
	}

	GameState Player::play(const GameState &pState, const Deadline &pDue)
	{
		//Must move in less than one second.
		high_resolution_clock::time_point start = high_resolution_clock::now();
		deadline = start + seconds(1);
		double time_left_before = 1.0;

		// empty hasmaps to avoid going out of memory
		nextStates.clear();
		gameValues.clear();

		//cerr << "Processing " << pState.toMessage() << endl;
		string StateKey = pState.toMessage();
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

		//Iterative deepening
		for (int d = 1; d < 20; d++)
		{
			//Initialize value and move.
			double value = -1 * numeric_limits<double>::infinity();
			unsigned int new_move = 0;

			//Initialize alpha and beta values to minus and plus infinity respectively.
			double alpha = -1 * numeric_limits<double>::infinity();
			double beta = numeric_limits<double>::infinity();

			//Time now
			high_resolution_clock::time_point t1 = high_resolution_clock::now();

			unsigned int m = 0;
			for (m = 0; m < nextStates[StateKey].size(); m++)
			{
				//Select move if its a win.
				if ((color == 1 && pState.isRedWin()) || (color == -1 && pState.isWhiteWin()))
					return nextStates[StateKey][m];

				float child_value = Player::MiniMaxAB(nextStates[StateKey][m], d, alpha, beta, false);
				if (child_value > value)
				{
					value = child_value;
					new_move = m;
				}
			}

			if(m ==nextStates[StateKey].size()) move = new_move;

			//Return move if there is not enough time for the next iteration.
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
			// double time_left = time_left_before - time_span.count();
			// if (6 * (time_left_before - time_left) > time_left)
			// {
			// 	duration<double> time_to_deadline = duration_cast<duration<double>>(deadline - high_resolution_clock::now());
			// 	cout << "TIME LIMIT REACHED" << endl;
			// 	cout << "time left: " << time_to_deadline.count() << endl;
			// 	cout << "time left: " << time_left << "at depth: " << d << endl;
			// 	return nextStates[StateKey][move];
			// }
			time_left_before -= time_span.count();

			//Sort children based on their value (highest first).
		}
		// duration<double> time_to_deadline = duration_cast<duration<double>>(deadline - high_resolution_clock::now());
		// cout << "Returning with: " << time_to_deadline.count() << endl;

		return nextStates[StateKey][move];
	}

	double Player::MiniMaxAB(const GameState &pState, int depth, double alpha, double beta, bool maxPlayer)
	{
		//Node StateKey
		string StateKey = pState.toMessage();

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
			if (maxPlayer) value = -1 * numeric_limits<double>::infinity();
			else value = numeric_limits<double>::infinity();

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
					value = max(value, child_value);
					alpha = max(value, alpha);

					//Beta cut-off
					if (beta <= alpha) break;
				}
				else
				{
					//Update value and beta.
					value = min(value, child_value);
					beta = min(value, beta);

					//Alpha cut-off
					if (beta <= alpha) break;
				}

				duration<double> time_to_deadline = duration_cast<duration<double>>(deadline - high_resolution_clock::now());
				// cout << "time left: " << time_to_deadline.count() << endl;
				if (time_to_deadline.count() < 0.5) break;
			}

			return value;

		}
	}

	void Player::StaticGameValue(const GameState &pState, const string StateKey)
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
