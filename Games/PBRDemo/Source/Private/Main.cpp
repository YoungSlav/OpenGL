#include "stdafx.h"


#include "BravoEngine.h"
#include "PBRGameInstance.h"
#include "BravoGameInstance.h"

int main()
{
    std::shared_ptr<BravoEngine> Engine(new BravoEngine("BravoEngine"));
	Engine->Initialize();
	
	std::shared_ptr<PBRGameInstance> GameInstance = Engine->NewObject<PBRGameInstance>("PBR Game Instance");

	Engine->GameLoop();

    return 0;
}
