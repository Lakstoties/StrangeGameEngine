#pragma once


//Character Data
struct CharacterData
{
	//Stats Normal
	unsigned int agileNormal;
	unsigned int brainNormal;
	unsigned int charmNormal;
	unsigned int detectNormal;
	unsigned int endureNormal;


	//Stats Current
	int agileCurrent;
	int brainCurrent;
	int charmCurrent;
	int detectCurrent;
	int endureCurrent;

	//Cash
	unsigned int cashOnSelf;
	unsigned int cashInStash;

	//Game Debt
	unsigned int gameDebt;

	//Inventory in Trench Coat
	unsigned int cocaineOnSelf;
	unsigned int heroinOnSelf;
	unsigned int acidOnSelf;
	unsigned int weedOnSelf;
	unsigned int speedOnSelf;
	unsigned int ludesOnSelf;

	//Trench Coat
	unsigned int trenchCoatStorage;

	//Stash Inventory
	unsigned int cocaineInStash;
	unsigned int heroineInStash;
	unsigned int acidInStash;
	unsigned int weedInStash;
	unsigned int speedInStash;
	unsigned int ludesInStash;


};