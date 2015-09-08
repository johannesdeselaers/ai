#include "player.hpp"
#include <cstdlib>
#include <math.h> 
#include <limits>


namespace checkers
{

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    //std::cerr << "Processing " << pState.toMessage() << std::endl;

    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);

    if (lNextStates.size() == 0) return GameState(pState, Move());

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
	for (int d = 7; d < 15; d++)
	{
		//Initialize value and move.
		double value = -1 * std::numeric_limits<double>::infinity();
		move = 0;

		//Initialize alpha and beta values to minus and plus infinity respectively.
		double alpha = -1 * std::numeric_limits<double>::infinity();
		double beta = std::numeric_limits<double>::infinity();

		//Time left
		double time_left_before = 1.0;

		for (unsigned int m=0; m < lNextStates.size(); m++)
		{
			float child_value = Player::MiniMaxAB(lNextStates[m], d, alpha, beta, true);
			if (child_value > value) move = m;
		}

		//Return move if there is not enough time for the next iteration.
		double time_left = 0.0;
		if ((time_left_before - time_left) > time_left) return lNextStates[move];

		//Sort children based on their value (highest first).
	}

	//return lNextStates[rand() % lNextStates.size()];
}

double Player::MiniMaxAB(const GameState &pState, int depth, double alpha, double beta, bool maxPlayer)
{
	if (!depth) return Player::StaticGameValue(pState);
	else
	{
		//Initialize value to minus/plus infinity.
		double value;
		if (maxPlayer) value = -1 * std::numeric_limits<double>::infinity();
		else value = std::numeric_limits<double>::infinity();

		//Get GameState children (possible next moves).
		std::vector<GameState> lNextStates;
		pState.findPossibleMoves(lNextStates);

		for (unsigned int i = 0; i < lNextStates.size(); i++)
		{
			//Get child value
			double child_value = Player::MiniMaxAB(lNextStates[i], (depth - 1), alpha, beta, !maxPlayer);

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

double Player::StaticGameValue(const GameState &pState)
{
	//Score should equal to 1 if the game state is a victory :)
	if (color == 1 && pState.isRedWin()) return 1.0;
	else if (color == -1 && pState.isWhiteWin()) return 1.0;

	//Check hash table to see if this value has already been computed before.

	//Points awarded for regular pieces (zero-zum).
	//Points for regular pieces stored at index 0.
	//Points for king pieces stored at index 1.
	int materialPoints[2];
	Player::materialValue(pState, materialPoints);

	//Moves left until draw.
	int movesLeft = (int)pState.getMovesUntilDraw();

	//Available moves.
	std::vector<GameState> lNextStates;
	pState.findPossibleMoves(lNextStates);
	int availableMoves = lNextStates.size();

	//Heuristic (linear polynomial).
	return B0 + B1 * color * materialPoints[0] + B2 * color * materialPoints[1] + B3 * movesLeft + B4 * availableMoves;
}

void Player::materialValue(const GameState &pState, int materialPoints[])
{

	//There are 31 cells in the board.
	for (int i = 0; i < 31; i++)
	{

		//If the cell is occupied by white, increment points.
		if (pState.at(i)&CELL_RED)
		{
			if (pState.at(i)&CELL_KING) materialPoints[1] += 1;
			else materialPoints[0] += 1;
		}
		//If the piece belongs to the oponent, substract points as appropiate.
		else if (pState.at(i)&CELL_WHITE)
		{
			if (pState.at(i)&CELL_KING) materialPoints[1] -= 1;
			else materialPoints[0] -= 1;
		}
	}
	
}

/*namespace checkers*/ }
