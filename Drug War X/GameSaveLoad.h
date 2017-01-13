#pragma once



struct GameStateData
{

};



int SaveGame(GameStateData gameData, char* filename);

int LoadGame(GameStateData &gameData, char* filename);