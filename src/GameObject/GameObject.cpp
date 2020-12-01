#include "../src/GameObject/GameObject.h"

Environment* Environment::instance = nullptr;

Environment* Environment::GetInstance()
{
	if (instance == nullptr)
		instance = new Environment;
	return instance;
}
