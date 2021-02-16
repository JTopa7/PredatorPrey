#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>

const int X_SIZE = 20;
const int Y_SIZE = 20;
const float SQUARE_PIXELS = 20.0;

class Index_out_of_bounds {};

enum Tile_type { EMPTY, ANT, DOODLEBUG, NUMBER_OF_TILES };
enum Direction { UP, DOWN, LEFT, RIGHT, NUMBER_OF_DIRECTIONS };
class World;

class Tile
{
public:
	Tile() : x(0), y(0), shape() {  }
	Tile(int x = 0, int y = 0, float radius = SQUARE_PIXELS, size_t point_count = 30) : x(x), y(y), shape(radius, point_count) { }
	virtual void display(sf::RenderWindow& window) = 0;
	static void tile_swap(Tile*& pTile1, Tile*& pTile2);
	virtual Tile_type who() = 0;
	virtual void turn(World& w) {};
protected:
	int x;
	int y;
	sf::CircleShape shape;
	void fix_shape_position();
};

class Organism : public Tile
{
public:
	Organism(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 30)
	{
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
	void display(sf::RenderWindow& window) = 0;
private:
};

class Ant : public Organism
{
public:
	Ant(int x = 0, int y = 0) : Organism(x, y)
	{
		shape.setFillColor(sf::Color::Green);
	}
	void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual Tile_type who() { return ANT; }
	virtual void turn(World& w);
private:
	int turnCount = 1;
};

class Doodlebug : public Organism
{
public:
	Doodlebug(int x = 0, int y = 0) : Organism(x, y)
	{
		shape.setFillColor(sf::Color::Red);
	}
	void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual Tile_type who() { return DOODLEBUG; }
	virtual void turn(World& w);
	
private:
	int turnsWithoutEating = 0;
	int turnCount = 1;
};

class Empty : public Tile
{
public:
	//Empty() : Tile(0, 0) { cout << "Calling default Empty constructor." << endl; }
	Empty(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 4)
	{
		shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256, 255));
		//tell the shape where it lives START HERE
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
	void display(sf::RenderWindow& window)
	{
		//window.draw(shape);
	}
	virtual Tile_type who() { return EMPTY; }

};

class World
{
public:
	World(int x = X_SIZE, int y = Y_SIZE, int ants = 100, int doodlebugs = 5);
	~World();
	//write the big 5
	void display(sf::RenderWindow& window);
	void run_simulation();
	void turn();
	std::vector<Tile*>& operator[](int index);
	const std::vector<Tile*>& operator[](int index) const;

private:
	std::vector<std::vector<Tile*>> grid;
};


int main()
{
	srand(time(0));
	World world;
	world.run_simulation();
	return 0;
}



World::World(int x, int y, int ants, int doodlebugs)
{
	std::vector<Tile*> column(y);
	for (int i = 0; i < x; i++)
	{
		grid.push_back(column);
	}

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			if (ants > 0)
			{
				grid[i][j] = new Ant(i, j);
				ants--;
			}
			else if (doodlebugs > 0)
			{
				grid[i][j] = new Doodlebug(i, j);
				doodlebugs--;
			}
			else
			{
				grid[i][j] = new Empty(i, j);
			}
		}
	}

	int x1, x2;
	int y1, y2;
	for (int i = 0; i < 100000; i++)
	{
		x1 = rand() % x;
		x2 = rand() % x;
		y1 = rand() % y;
		y2 = rand() % y;
		Tile::tile_swap(grid[x1][y1], grid[x2][y2]);
	}
}

World::~World()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			delete grid[i][j];
		}
	}
}

void World::display(sf::RenderWindow& window)
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j]->display(window);
		}
	}
}

void World::turn()
{
	std::vector<Tile*> doodlebugs;
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j]->who() == DOODLEBUG)
			{
				doodlebugs.push_back(grid[i][j]);
			}
		}
	}

	for (auto doodlebug : doodlebugs)
	{
		doodlebug->turn(*this);
	}

	std::vector<Tile*> ants;
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j]->who() == ANT)
			{
				ants.push_back(grid[i][j]);
			}
		}
	}

	for (auto ant : ants)
	{
		ant->turn(*this);
	}
}

void World::run_simulation()
{
	sf::RenderWindow window(sf::VideoMode(400, 400), "World of blobs!");
	int count = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				turn();
			}
			
			//cout << event.type << endl;
		}
		//sf::sleep(sf::milliseconds(33));
		window.clear();
		/*count++;
		if (count == 200)
		{
			turn();
			count = 0;
		}*/
		display(window);
		//shape.move(sf::Vector2f(rand()%3 -1, rand()%3 - 1));
		//shape.rotate(1);
		//window.draw(shape);		
		window.display();

	}

}

void Tile::tile_swap(Tile*& pTile1, Tile*& pTile2)
{
	std::swap(pTile1->x, pTile2->x);
	std::swap(pTile1->y, pTile2->y);
	std::swap(pTile1, pTile2);
	pTile1->fix_shape_position();
	pTile2->fix_shape_position();
}

void Tile::fix_shape_position()
{
	shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
}

void Doodlebug::turn(World& w)
{
	int tempX = 0;
	int tempY = 0;
	int placeholderX;
	int placeholderY;
	bool ateAnt = false;
	if (y > 0 && w[x][y - 1]->who() == ANT)
	{
		tempX = x;
		tempY = y - 1;
		delete w[x][y - 1];
		w[tempX][tempY] = new Empty(tempX, tempY);
		Tile::tile_swap(w[x][y], w[tempX][tempY]);
		ateAnt = true;
	}
	else if(y < Y_SIZE - 1 && w[x][y+1]->who() == ANT)
	{
		tempX = x;
		tempY = y + 1;
		delete w[x][y + 1];
		w[tempX][tempY] = new Empty(tempX, tempY);
		Tile::tile_swap(w[x][y], w[tempX][tempY]);
		ateAnt = true;
	}
	else if (x > 0 && w[x - 1][y]->who() == ANT){
		tempX = x - 1;
		tempY = y;
		delete w[x - 1][y];
		w[tempX][tempY] = new Empty(tempX, tempY);
		Tile::tile_swap(w[x][y], w[tempX][tempY]);
		ateAnt = true;
	}

	else if (x < X_SIZE - 1 && w[x + 1][y]->who() == ANT)
	{
		tempX = x + 1;
		tempY = y;
		delete w[x + 1][y];
		w[tempX][tempY] = new Empty(tempX, tempY);
		Tile::tile_swap(w[x][y], w[tempX][tempY]);
		ateAnt = true;
	}
	
	else {
		switch (rand() % NUMBER_OF_DIRECTIONS)
		{
		case UP:
			if (y > 0)
			{
				if (w[x][y - 1]->who() == EMPTY)
				{
					Tile::tile_swap(w[x][y], w[x][y - 1]);
				}
			}
			break;
		case DOWN:
			if (y < Y_SIZE - 1)
			{
				if (w[x][y + 1]->who() == EMPTY)
				{
					Tile::tile_swap(w[x][y], w[x][y + 1]);
				}
			}
			break;
		case LEFT:
			if (x > 0)
			{
				if (w[x - 1][y]->who() == EMPTY)
				{
					Tile::tile_swap(w[x][y], w[x - 1][y]);
				}
			}
			break;
		case RIGHT:
			if (x < X_SIZE - 1)
			{
				if (w[x + 1][y]->who() == EMPTY)
				{
					Tile::tile_swap(w[x][y], w[x + 1][y]);
				}
			}
			break;
		}
	}
	if (ateAnt == true) {
		turnsWithoutEating = 0;
	}
	else {
		turnsWithoutEating++;
	}
	if (turnsWithoutEating >= 3) {
		placeholderX = x;
		placeholderY = y;
		delete w[x][y];
		w[placeholderX][placeholderY] = new Empty(placeholderX, placeholderY);
	}
	
	if (turnCount % 8 == 0){
		if (y > 0 && w[x][y - 1]->who() == EMPTY) {
		placeholderX = x;
		placeholderY = y - 1;	
		delete w[x][y - 1];
		w[placeholderX][placeholderY] = new Doodlebug(placeholderX, placeholderY);
	}

	else if (y < Y_SIZE - 1 && w[x][y + 1]->who() == EMPTY) {
		placeholderX = x;	
		placeholderY = y + 1;
		delete w[x][y + 1];
		w[placeholderX][placeholderY] = new Doodlebug(placeholderX, placeholderY);
	}
	else if (x > 0 && w[x - 1][y]->who() == EMPTY) {
		placeholderX = x - 1;
		placeholderY = y;
		delete w[x - 1][y];
		w[placeholderX][placeholderY] = new Doodlebug(placeholderX, placeholderY);
	}
		else if (x < X_SIZE - 1 && w[x + 1][y]->who() == EMPTY) {
			placeholderX = x + 1;
			placeholderY = y;
			delete w[x + 1][y];			
			w[placeholderX][placeholderY] = new Doodlebug(placeholderX, placeholderY);
		}
	}
		
	turnCount++;
}

void Ant::turn(World& w)
{
	int tempX = 0;
	int tempY = 0;
	switch (rand() % NUMBER_OF_DIRECTIONS)
	{
	case UP:
		if (y > 0)
		{
			if (w[x][y - 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y - 1]);
			}
		}
		break;
	case DOWN:
		if (y < Y_SIZE - 1)
		{
			if (w[x][y + 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y + 1]);
			}
		}
		break;
	case LEFT:
		if (x > 0)
		{
			//go left
			if (w[x - 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x - 1][y]);
			}
		}
		break;
	case RIGHT:
		if (x < X_SIZE - 1)
		{
			if (w[x + 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x + 1][y]);
			}
		}
		break;
	}
	if (turnCount % 3 == 0)
	{		
		if (y > 0 && w[x][y - 1]->who() == EMPTY){
			tempX = x;
			tempY = y - 1;
			delete w[x][y - 1];
			w[tempX][tempY] = new Ant(tempX, tempY);				
		}
			
		else if (y < Y_SIZE - 1 && w[x][y + 1]->who() == EMPTY){
			tempX = x;
			tempY = y + 1;
			delete w[x][y + 1];
			w[tempX][tempY] = new Ant(tempX, tempY);
		}
		else if (x > 0 && w[x - 1][y]->who() == EMPTY){
			tempX = x - 1;
			tempY = y;
			delete w[x - 1][y];
			w[tempX][tempY] = new Ant(tempX, tempY);
		}
		else if (x < X_SIZE - 1 && w[x + 1][y]->who() == EMPTY){
			tempX = x + 1;
			tempY = y;
			delete w[x + 1][y];
			w[tempX][tempY] = new Ant(tempX, tempY);
		}	
	}
	turnCount++;
}

std::vector<Tile*>& World::operator[](int index)
{
	if (index >= grid.size())
	{
		throw Index_out_of_bounds();
	}
	return grid[index];
}
const std::vector<Tile*>& World::operator[](int index) const
{
	if (index >= grid.size())
	{
		throw Index_out_of_bounds();
	}
	return grid[index];
}