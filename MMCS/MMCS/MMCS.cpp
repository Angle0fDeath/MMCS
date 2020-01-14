#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <iterator>
#include <string>
#include <time.h>
#include <stdlib.h>

const uint8_t PLAYERS = 4;
const unsigned long MAXRUNS = 10000000;
int myrandom(int i) { return std::rand() % i; }
static std::map<uint8_t, std::string> combinationsNames;

enum ERROR {
	ERROR_OK		= 0x00,
	ERROR_WON		= 0x10,
	ERROR_LOST		= 0x11,
	ERROR_UNKOWN	= 0xA0
};

enum COMBINATIONS {
	ROYAL_FLUSH		= 0xDF,
	STRAIGHT_FLUSH	= 0x5F,
	FOUR_OF_A_KIND	= 0x3E,
	FULL_HOUSE		= 0x38,
	FLUSH			= 0x35,
	STRAIGHT		= 0x2A,
	THREE_OF_A_KIND	= 0x28,
	TWO_PAIR		= 0x18,
	PAIR			= 0x10,
	HIGH_CARD		= 0x00
};

enum SHAPES {
	CLUB	= 1,
	SPADE	= 2,
	HEART 	= 4,
	DIAMOND	= 8
};

struct card {
	SHAPES shape;
	uint8_t number;
};

struct double_value {
	unsigned long presence, wins;
};

class player {
private:
	uint8_t ID, combination;
	std::vector<card> cards;
public:
	player(uint8_t outID);
	~player();
	uint8_t GetCombination();
	ERROR SetCards(card * outPointer);
	ERROR ResetCards();
	ERROR FindCombinations();
};

player::player(uint8_t outID) {
	ID = outID;
	combination = COMBINATIONS::HIGH_CARD;
	cards = std::vector<card>(5);
}

player::~player() {

}

uint8_t player::GetCombination() {
	return combination;
}

ERROR player::SetCards(card * outPointer) {
	for (uint8_t i = 0; i < 5; i++) {
		cards.at(i) = *(outPointer + i);
	}
	return ERROR::ERROR_OK;
}

ERROR player::ResetCards() {
	for (uint8_t i = 0; i < 5; i++) {
		cards.at(i) = { (SHAPES)0,0 };
	}
	return ERROR::ERROR_OK;
}

ERROR player::FindCombinations() {
	uint8_t shape = 0;
	std::map<uint8_t, uint8_t> cardNumbers;
	uint8_t flush = 0, straight = 1;
	for (uint8_t i = 0; i < 5; i++) {
		shape |= cards.at(i).shape;
		if (cardNumbers.find(cards.at(i).number) == cardNumbers.end()) {
			cardNumbers[cards[i].number] = 1;
		}
		else {
			cardNumbers[cards[i].number] = cardNumbers[cards[i].number] + 1;
		}
	}
	if (cardNumbers.size() == 2) {
		std::map<uint8_t, uint8_t>::iterator itr;
		for (itr = cardNumbers.begin(); itr != cardNumbers.end(); ++itr) {
			if (itr->second == 4) {
				combination = COMBINATIONS::FOUR_OF_A_KIND;
				return ERROR::ERROR_OK;
			}
		}
		combination = COMBINATIONS::FULL_HOUSE;
		return ERROR::ERROR_OK;
	}
	if (cardNumbers.size() == 3) {
		std::map<uint8_t, uint8_t>::iterator itr;
		for (itr = cardNumbers.begin(); itr != cardNumbers.end(); ++itr) {
			if (itr->second == 3) {
				combination = COMBINATIONS::THREE_OF_A_KIND;
				return ERROR::ERROR_OK;
			}
		}
		combination = COMBINATIONS::TWO_PAIR;
		return ERROR::ERROR_OK;
	}
	if (cardNumbers.size() == 4) {
		combination = COMBINATIONS::PAIR;
		return ERROR::ERROR_OK;
	}
	std::map<uint8_t, uint8_t>::iterator itr = cardNumbers.begin(), itr2 = cardNumbers.begin();
	for (++itr ; itr != cardNumbers.end(); ++itr) {
		if (itr->first - 1 != itr2->first) {
			straight = 0;
			break;
		}
		itr2 = itr;
	}	
	if (shape == 1 || shape == 2 || shape == 4 || shape == 8)
		flush = 1;
	if (flush == 1 && straight == 1 && cardNumbers.begin()->first == 10) {
		combination = COMBINATIONS::ROYAL_FLUSH;
		return ERROR::ERROR_OK;
	}
	if (flush == 1 && straight == 1) {
		combination = COMBINATIONS::STRAIGHT_FLUSH;
		return ERROR::ERROR_OK;
	}
	if (flush == 1) {
		combination = COMBINATIONS::FLUSH;
		return ERROR::ERROR_OK;
	}
	if (straight == 1) {
		combination = COMBINATIONS::STRAIGHT;
		return ERROR::ERROR_OK;
	}
		
	combination = COMBINATIONS::HIGH_CARD;
	return ERROR::ERROR_OK;
}

class game {
private:
	uint8_t ID;
	std::vector<card> deck;
	std::vector<player> players;
	std::map<uint8_t, double_value> percentage;
public:
	game();
	~game();
	ERROR Display(std::map<uint8_t, double_value> outPercentage);
	ERROR Play(unsigned long outNumber);
	ERROR CheckWinner();
};


game::game() {
	ID = 0;
	deck = std::vector<card>(52);
	for (uint8_t i = 2; i < 15; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			deck.at((i - 2) * 4 + j) = { (SHAPES)(1 << j) , i };
		}
	}
	for (uint8_t i = 1; i < PLAYERS + 1; i++) {
		player auxPlayer(i);
		players.push_back(auxPlayer);
	}
	percentage[COMBINATIONS::ROYAL_FLUSH] = { 0,0 };
	percentage[COMBINATIONS::STRAIGHT_FLUSH] = { 0,0 };
	percentage[COMBINATIONS::FOUR_OF_A_KIND] = { 0,0 };
	percentage[COMBINATIONS::FULL_HOUSE] = { 0,0 };
	percentage[COMBINATIONS::FLUSH] = { 0,0 };
	percentage[COMBINATIONS::STRAIGHT] = { 0,0 };
	percentage[COMBINATIONS::THREE_OF_A_KIND] = { 0,0 };
	percentage[COMBINATIONS::TWO_PAIR] = { 0,0 };
	percentage[COMBINATIONS::PAIR] = { 0,0 };
	percentage[COMBINATIONS::HIGH_CARD] = { 0,0 };
}

game::~game() {


}

ERROR game::Display(std::map<uint8_t, double_value> outPercentage) {
	std::map<uint8_t, double_value>::iterator itr;
	for (itr = outPercentage.begin(); itr != outPercentage.end(); ++itr) {
		printf("combinatia %s a aparut de %lu si a castigat de %lu\nprocent aparitie: %.2f%% si procent victorii: %.2f%%\n\n", combinationsNames[itr->first].c_str(), itr->second.presence, itr->second.wins, ((double)itr->second.presence / MAXRUNS) * 100, ((double)itr->second.wins/(double)itr->second.presence) * 100);
	}
	return ERROR::ERROR_OK;
}

ERROR game::Play(unsigned long outNumber) {
	unsigned long counter = 0;
	for (counter = 0; counter < outNumber; counter++) {
		std::random_shuffle(std::begin(deck), std::end(deck), myrandom);
		for (uint8_t i = 0; i < PLAYERS; i++) {
			players.at(i).ResetCards();
			players.at(i).SetCards(&deck[0 + i * 5]);
			players.at(i).FindCombinations();
		}
		if (this->CheckWinner() == ERROR::ERROR_WON) {
			percentage[players.at(0).GetCombination()] = { percentage[players.at(0).GetCombination()].presence + 1, percentage[players.at(0).GetCombination()].wins + 1 };
		}
		else {
			percentage[players[0].GetCombination()] = { percentage[players[0].GetCombination()].presence + 1, percentage[players[0].GetCombination()].wins};
		}
	}
	this->Display(percentage);
	return ERROR::ERROR_OK;
}

ERROR game::CheckWinner() {
	for (uint8_t i = 1; i < PLAYERS; i++) {
		if (players[i].GetCombination() > players[0].GetCombination()) {
			return ERROR::ERROR_LOST;
		}
	}
	return ERROR::ERROR_WON;
}

ERROR Initialize() {
	std::srand(time(NULL));
	combinationsNames[COMBINATIONS::ROYAL_FLUSH] = "ROYAL_FLUSH";
	combinationsNames[COMBINATIONS::STRAIGHT_FLUSH] = "STRAIGHT_FLUSH";
	combinationsNames[COMBINATIONS::FOUR_OF_A_KIND] = "FOUR_OF_A_KIND";
	combinationsNames[COMBINATIONS::FULL_HOUSE] = "FULL_HOUSE";
	combinationsNames[COMBINATIONS::FLUSH] = "FLUSH";
	combinationsNames[COMBINATIONS::STRAIGHT] = "STRAIGHT";
	combinationsNames[COMBINATIONS::THREE_OF_A_KIND] = "THREE_OF_A_KIND";
	combinationsNames[COMBINATIONS::TWO_PAIR] = "TWO_PAIR";
	combinationsNames[COMBINATIONS::PAIR] = "PAIR";
	combinationsNames[COMBINATIONS::HIGH_CARD] = "HIGH_CARD";
	return ERROR::ERROR_OK;
}

int main()
{
	Initialize();
	game playGame;
	playGame.Play(MAXRUNS);
	system("pause");
	return 0;
}
