// Patrick Donghil
// Raul Barquilla Jr.
// Ian Christian Lao
// Roanne Pearl Misolas
// BSCS-3B

#include <iostream>
#include <time.h>
#include <string>

#define n 3

using namespace std;

//this class is used for creating objects that keeps the position of the blank tile for each state
//OOP terminologies: class, methods, attributes
class Vector2
{
public:
	int i, j;
	void setIndex(int x, int y)
	{
		i = x;
		j = y;
	}
};

//the main data structure for storing a state of a puzzle like:
struct eightPuzzle
{
	int board[n][n];	   //tile arrangements
	Vector2 blankTile;	   //position (index) of the blank tile. Vector2 class is implemented above
	int level;			   //depth of a node (level) which is used in IDS algorithm
	string move;		   //holds the previous tile movement which resulted into current state
	int manhattanDistance; //used in AStar
	eightPuzzle *parent;   //pointer to parent node - used to backtrack previous states
};

//data structure needed for creating a linked-list of states
struct list
{
	eightPuzzle *state;
	list *next;
};

//global goal state given in the machine problem
int goalState[][n] = {{1, 2, 3}, {8, 0, 4}, {7, 6, 5}};

//function declarations
eightPuzzle *newState(int state[][n]);
eightPuzzle *newInitialState(int arr[][n]);
eightPuzzle *move(eightPuzzle *state, string direction);
bool movable(eightPuzzle *state, string direction);
bool isEqual(eightPuzzle *state1, eightPuzzle *state2);
bool isGoal(eightPuzzle *state1);
void print(eightPuzzle *state);
int printStates(eightPuzzle *state);
int getManhattanDistance(eightPuzzle *state);

//IDS and AStar search function declaration
void IDS(eightPuzzle *initialState);
void AStar(eightPuzzle *initialState);

//making use of the data structure list, This is a class used for handling linked-list of states i.e:
//creating queue or stacks,
//manipulating values of a state
class List
{
public:
	//initializing an empty list of states
	list *lst = NULL;
	//accessing a node (state) in the BEGINNING of the list & popping it afterwards
	eightPuzzle *front()
	{
		eightPuzzle *tmp = NULL;
		if (lst != NULL)
		{
			tmp = lst->state;
			list *temp = lst;
			lst = lst->next;
			delete temp;
		}
		return tmp;
	}
	//accessing a node (state) in the END of the list & popping it afterwards
	eightPuzzle *end()
	{
		if (lst->next == NULL)
		{
			front();
			return lst->state;
		}
		eightPuzzle *tmp = NULL;
		if (lst != NULL)
		{
			list *temp = lst;
			while (temp->next != NULL)
			{
				temp = temp->next;
			}
			tmp = temp->state;
			delete temp;
			temp = NULL;
		}
		return tmp;
	}
	void insertToFront(eightPuzzle *s)
	{
		list *tmp = new (list);
		tmp->state = s;
		tmp->next = NULL;
		if (lst != NULL)
		{
			tmp->next = lst;
		}
		lst = tmp;
	}
	void insertToEnd(eightPuzzle *s)
	{
		if (lst == NULL)
		{
			insertToFront(s);
			return;
		}
		list *temp = lst;
		list *tmp = new (list);
		temp->state = s;
		temp->next = NULL;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = tmp;
	}
	//this returns false or true if the given state is already in the list or not
	//this helps preventing insertion of the same node twice into the list
	bool notInList(eightPuzzle *state)
	{
		list *tmplist = lst;
		while (tmplist != NULL)
		{
			if (isEqual(state, tmplist->state))
			{
				return false;
			}
			tmplist = tmplist->next;
		}
		return true;
	}

	eightPuzzle *chooseBestState()
	{ //chooses the state on the entire list with the lowest heuristic value
		list *tmplist = lst;
		list *previous;
		list *survivor; //holds the state with the lowest heuristic value
		eightPuzzle *bestState = NULL;
		int min;

		if (lst->next == NULL)
		{ //CASE 1: has only 1 node in the list, just return that. that's already the bestState since there's no other state to compare with in the first place
			bestState = lst->state;
			getManhattanDistance(bestState);
			delete lst;
			lst = NULL;
			return bestState;
		}
		//finding the state with lowest hueristic value by looping through the entire list
		min = getManhattanDistance(tmplist->state); //gets the heuristic value of the first state and sets that value as the minimum
		survivor = tmplist;							//initially, the best state is set to the first state in the list
		while (tmplist->next != NULL)
		{
			int dist = getManhattanDistance(tmplist->next->state);
			if (dist < min)
			{
				previous = tmplist;
				survivor = tmplist->next;
				min = dist;
			}
			tmplist = tmplist->next; //iterate
		}
		bestState = survivor->state;
		if (lst != NULL)
		{
			if (survivor == lst)
			{ //CASE 1: bestState is found at the start of the list
				return front();
			}
			else if (survivor->next == NULL)
			{						   //CASE 2: bestState is at the last
				previous->next = NULL; //link the previous state to NULL means removing the last state out of the list
			}
			else
			{									 //CASE 3: bestState is found in the middle so unlink the middle and reconnect the list
				previous->next = survivor->next; //link the previous node and the node next to bestState.
			}
		}
		delete survivor;
		return bestState;
	}
	void view()
	{ //view the entire list of states
		if (lst != NULL)
		{
			list *tmp = lst;
			while (tmp != NULL)
			{
				cout << getManhattanDistance(tmp->state) << " ";
				tmp = tmp->next;
			}
		}
		else
			cout << "empty list";
	}
};

int main()
{
	int initState[n][n] = {0};

	cout << "+-----------------------------------+" << endl
		 << "| 8 Puzzle Problem Using IDS and A* |" << endl
		 << "+-----------------------------------+" << endl
		 << endl;
	cout << "Enter an initial state: (0-9)" << endl;
	cout << "Example: 1 3 4 8 6 2 7 0 5" << endl
		 << endl;

	//initialize initial state to null
	eightPuzzle *init = NULL;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> initState[i][j];
		}
	}
	init = newInitialState(initState);

	cout << endl;

	cout << "Initial State:" << endl;
	print(init);
	cout << endl;

	cout << "Goal State:" << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "+---+---+---+" << endl;
		for (int j = 0; j < n; j++)
		{
			cout << "| " << goalState[i][j] << ' ';
		}
		cout << "|" << endl;
	}
	cout << "+---+---+---+" << endl
		 << endl;

	double cpuTimeUsed;
	//variables for measuring the running time
	clock_t start, end;
	cout << "+--------------------------------+" << endl
		 << "|  A* Search (Heuristic Search)  |" << endl
		 << "+--------------------------------+" << endl;
	start = clock();
	AStar(init);
	end = clock();
	cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "Running Time: " << cpuTimeUsed << endl
		 << endl;

	cout << "+---------------------------------------------+" << endl
		 << "|  Iterative Deepening Search (Blind Search)  |" << endl
		 << "+---------------------------------------------+" << endl;
	start = clock();
	IDS(init); //IDS algorithm
	end = clock();
	cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "Running time: " << cpuTimeUsed << endl;

	return 0;
}

//A* search function
void AStar(eightPuzzle *initialState)
{
	List openList;
	List closedList;
	openList.insertToFront(initialState);
	int counter = 0;
	while (openList.lst != NULL)
	{
		//get the best state which has the lowest heuristic value f(x) + g(x)
		eightPuzzle *bestState = openList.chooseBestState();
		closedList.insertToFront(bestState); //moves to closed list

		if (isGoal(bestState))
		{
			cout << "Solution Path: ";
			int cost = printStates(bestState);
			cout << endl;
			cout << "No. of Nodes Expanded: " << counter << endl;
			cout << "Solution Cost: " << cost - 1 << endl;
			return;
		}
		counter++;
		if (movable(bestState, "UP"))
		{
			eightPuzzle *tmp = move(bestState, "UP");
			if (closedList.notInList(tmp))
			{
				openList.insertToFront(tmp);
			}
		}
		if (movable(bestState, "RIGHT"))
		{
			eightPuzzle *tmp = move(bestState, "RIGHT");
			if (closedList.notInList(tmp))
			{
				openList.insertToFront(tmp);
			}
		}
		if (movable(bestState, "DOWN"))
		{
			eightPuzzle *tmp = move(bestState, "DOWN");
			if (closedList.notInList(tmp))
			{
				openList.insertToFront(tmp);
			}
		}
		if (movable(bestState, "LEFT"))
		{
			eightPuzzle *tmp = move(bestState, "LEFT");
			if (closedList.notInList(tmp))
			{
				openList.insertToFront(tmp);
			}
		}
	}
}

//Iterative Deepening Search
void IDS(eightPuzzle *initialState)
{
	int i = 0, counter = 0;
	while (true)
	{
		List closed;
		List stack;
		stack.insertToFront(initialState);
		while (stack.lst != NULL)
		{
			eightPuzzle *first = stack.front();

			if (first->level > i)
				continue;

			closed.insertToFront(first);

			if (isGoal(first))
			{
				cout << "Solution Path: ";
				int cost = printStates(first);
				cout << endl;
				cout << "No. of Nodes Expanded: " << counter << endl;
				cout << "Solution Cost: " << cost - 1 << endl;
				return;
			}
			counter++;
			if (movable(first, "UP"))
			{
				eightPuzzle *tmp = move(first, "UP");
				if (closed.notInList(tmp))
				{
					stack.insertToFront(tmp);
				}
			}
			if (movable(first, "RIGHT"))
			{
				eightPuzzle *tmp = move(first, "RIGHT");
				if (closed.notInList(tmp))
				{
					stack.insertToFront(tmp);
				}
			}
			if (movable(first, "DOWN"))
			{
				eightPuzzle *tmp = move(first, "DOWN");
				if (closed.notInList(tmp))
				{
					stack.insertToFront(tmp);
				}
			}
			if (movable(first, "LEFT"))
			{
				eightPuzzle *tmp = move(first, "LEFT");
				if (closed.notInList(tmp))
				{
					stack.insertToFront(tmp);
				}
			}
		}
		i++;
	}
}

//creating a new state that accepts array which contains tile arrangement
eightPuzzle *newState(int state[][n])
{
	eightPuzzle *tmp = new (eightPuzzle);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (state[i][j] == 0) //finds the blank tile
				tmp->blankTile.setIndex(i, j);
			tmp->board[i][j] = state[i][j];
			tmp->manhattanDistance = -1; //by default sets to -1 to determine if it is not yet calculated
		}
	}
	return tmp;
}

//used for creating a new initial state
eightPuzzle *newInitialState(int arr[][n])
{
	eightPuzzle *state = newState(arr);
	state->level = 0; //initial state has g(x) = 0
	state->move = "Start";
	state->parent = NULL;
	return state; //return the created state
}

//used in checking if the state is already in the visited list - notInList()    function
bool isEqual(eightPuzzle *state1, eightPuzzle *state2)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (state1->board[i][j] != state2->board[i][j])
				return false;
		}
	}
	return true;
}

//checks if the goal is found by comparing each tiles
bool isGoal(eightPuzzle *state1)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (state1->board[i][j] != goalState[i][j])
				return false;
		}
	}
	return true;
}

void print(eightPuzzle *state)
{
	for (int i = 0; i < n; i++)
	{
		cout << "+---+---+---+" << endl;
		for (int j = 0; j < n; j++)
		{
			cout << "| " << state->board[i][j] << ' ';
		}
		cout << "|";
		cout << endl;
	}
	cout << "+---+---+---+" << endl;
}

int printStates(eightPuzzle *state)
{
	if (state != NULL)
	{
		int i;
		i = printStates(state->parent) + 1;
		if (state->move == "Start")
			return i;
		cout << " " << state->move;
		return i;
	}
	else
	{
		return 0;
	}
}

//moves the blank tile in a certain direction
eightPuzzle *move(eightPuzzle *state, string direction)
{
	eightPuzzle *tmp = newState(state->board);
	tmp->parent = state;
	tmp->level = state->level + 1;

	//actions i.e move left, right, "UP" and down
	if (direction == "UP")
	{
		tmp->move = "UP";
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i - 1][tmp->blankTile.j];
		tmp->blankTile.i--;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == "RIGHT")
	{
		tmp->move = "RIGHT";
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i][tmp->blankTile.j + 1];
		tmp->blankTile.j++;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == "DOWN")
	{
		tmp->move = "DOWN";
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i + 1][tmp->blankTile.j];
		tmp->blankTile.i++;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == "LEFT")
	{
		tmp->move = "LEFT";
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i][tmp->blankTile.j - 1];
		tmp->blankTile.j--;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else
		return NULL;
	return tmp;
}

//checks if it is a valid move for the blank tile
bool movable(eightPuzzle *state, string direction)
{
	if (direction == "UP")
	{
		if (state->blankTile.i > 0)
			return true;
	}
	else if (direction == "RIGHT")
	{
		if (state->blankTile.j < n - 1)
			return true;
	}
	else if (direction == "DOWN")
	{
		if (state->blankTile.i < n - 1)
			return true;
	}
	else if (direction == "LEFT")
	{
		if (state->blankTile.j > 0)
			return true;
	}
	return false;
}

//return the absolute value of an integer
int abs(int x)
{
	if (x < 0)
	{
		x -= (x * 2);
	}
	return x;
}

//used in getManhattanDistance function for AStar
int distBetween2Tiles(eightPuzzle *state, Vector2 correctTile)
{
	for (int y = 0; y < n; y++)
	{
		for (int x = 0; x < n; x++)
		{
			int i = correctTile.i;
			int j = correctTile.j;
			if (state->board[y][x] == goalState[i][j])
			{
				int localDist = abs(i - y) + abs(j - x);
				return localDist;
			}
		}
	}
	return 0;
}

int getManhattanDistance(eightPuzzle *state)
{
	if (state->manhattanDistance != -1)
	{ //if already calculated previously, just return that value
		return state->manhattanDistance;
	}
	int dist = 0;		 //variable that will keep the total manhattan distance
	Vector2 correctTile; //holds the index (i,j) of a tile in the given state
	for (int i = 0; i < n; i++)
	{ //loop through all the tiles of the given state
		for (int j = 0; j < n; j++)
		{
			if (state->board[i][j] == goalState[i][j]) //skip if the tile is in the correct place
				continue;
			else
			{ //else calculate the distance between 2 tiles (goalState's tile & misplaced tile)
				correctTile.setIndex(i, j);
				dist += distBetween2Tiles(state, correctTile); //implemented as function to make use of return keyword to terminate nested loops completely when the distance is already calculated
			}
		}
	}
	//memoizing (recording) the value so next time getting the heuristic value for this state, just return the value recorded
	//this will skip bunch of loops next time
	//useful for getting the state with the lowest heuristic value in A* search algorithm
	state->manhattanDistance = dist + state->level;
	return dist + state->level;
}
