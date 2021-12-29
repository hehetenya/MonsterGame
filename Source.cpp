#include <iostream>
#include <string>
#include <array>
#include <string_view>
#include <cstdlib> // for rand() and srand()
#include <ctime>



int getRandomNumber(int min, int max)
{
	static constexpr double fraction{ 1.0 / (RAND_MAX + 1.0) }; // static used for efficiency, so we only calculate this value once
	// evenly distribute the random number across our range
	return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

class Creature
{
protected:
	std::string m_name{};
	char m_symbol{};
	int m_health{};
	int m_damage{};
	int m_gold{};

public:
	Creature(std::string_view name, char symbol, int health, int damage, int gold) : 
		m_name{ name }, m_symbol{ symbol }, m_health{ health }, m_damage{ damage }, m_gold{gold}
	{}

	std::string& getName() { return m_name; }
	char getSymbol() { return m_symbol; }
	int getHealth() { return m_health; }
	int getDamage() { return m_damage; }
	int getGold() { return m_gold; }

	void reduceHealth(int a) { m_health -= a; }
	void increaseHealth(int a) { m_health += a; }
	void increaseStrength() { ++m_damage; }
	bool isDead() { return m_health <= 0; }
	void addGold(int a) { m_gold += a; }

};

class Player: public Creature
{
	int m_level{1};
public:
	Player(std::string_view name)
		: Creature{ name, '@', 10, 1, 0 }
	{
	}
	int getLevel() { return m_level; }
	void levelUp() { ++m_level; ++m_damage; }
	bool hasWon() { return m_level >= 20; }
};

class Monster : public Creature
{
	
public:
	enum class Type
	{
		dragon,
		orc,
		slime,
		max_types
	};

private:
	static const Creature& getDefaultCreature(Type type)
	{
		static const std::array<Creature, static_cast<std::size_t>(Type::max_types)> monsterData{
		  { { "dragon", 'D', 20, 4, 100 },
			{ "orc", 'o', 4, 2, 25 },
			{ "slime", 's', 1, 1, 10 } }
		};

		return monsterData.at(static_cast<std::size_t>(type));
	}

public:
	Monster(Type type)
		: Creature{getDefaultCreature(type)}
	{}

	static Monster getRandomMonster()
	{
		int num{ getRandomNumber(0, static_cast<int>(Type::max_types) - 1) };
		return Monster{ static_cast<Type>(num) };
	}
};

void attackPlayer(Player&, Monster&);
void fightMonster(Player&, Monster&);

void drinkPotion(Player& p, Monster& m)
{
	std::cout << "You have found the mistery potion!\n";
	std::cout << "(D)rink og (I)gnore: ";
	char choice{};
	std::cin >> choice;
	if (choice == 'd')
	{
		int x{ getRandomNumber(1,3) };
		switch (x)
		{
		case 1:
			std::cout << "You have drank a health potion!\n";
			p.increaseHealth(2);
			std::cout << "Your health was increased by 2 points.\n";
			break;
		case 2:
			std::cout << "You have drank a strength potion!\n";
			p.increaseStrength();
			std::cout << "Your strength was increased by 2 points.\n";
			break;
		case 3:
			std::cout << "Oh no! You have drank a poison!\n";
			p.reduceHealth(1);
			std::cout << "Your health was reduced by 1 point.\n";
			break;
		}
	}
}

void attackMonster(Player& p, Monster& m)
{
	std::cout << "You hit the " << m.getName() << " for " << p.getDamage() << " damage.\n";
	m.reduceHealth(p.getDamage());
	if (m.isDead())
	{
		std::cout << "You killed the " << m.getName() << ".\n";
		p.levelUp();
		if (p.hasWon())
		{
			std::cout << "You reached level 20 and won!\n";
			return;
		}
		std::cout << "You are now level " << p.getLevel() << ".\n";
		std::cout << "You found " << m.getGold() << " gold.\n";
		p.addGold(m.getGold());
		if (getRandomNumber(1, 3) < 2)
		{
			drinkPotion(p, m);
		}
		return;
	}
	attackPlayer(p, m);
}

void attackPlayer(Player& p, Monster& m)
{
	std::cout << "The " << m.getName() << " hit you for " << m.getDamage() << " damage.\n";
	p.reduceHealth(m.getDamage());
	if (p.isDead())
	{
		std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
		std::cout << "Too bad you can't take it with you!\n";
		return;
	}
	fightMonster(p, m);
}

void fightMonster(Player& p, Monster& m)
{
	/*static Monster m{ Monster::getRandomMonster() };
	std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ")\n";*/

	std::cout << "(R)un or (F)ight: ";
	char choice{};
	std::cin >> choice;
	if (choice == 'f')
		attackMonster(p, m);
	else if (getRandomNumber(1, 2) < 2)
		attackPlayer(p, m);
	else
		std::cout << "You successfully fled.\n";

}

int main()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // set initial seed value to system clock
	std::rand(); // get rid of first result

	std::cout << "Enter your name: ";
	std::string name{};
	std::cin >> name;
	Player p{ name };
	std::cout << "Welcome, " << p.getName() << '\n';

	while (!p.isDead())
	{
		Monster m{ Monster::getRandomMonster() };
		std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ")\n";

		fightMonster(p, m);
	}

	return 0;
}