#pragma once
#include "stdafx.h"

#include "BravoEngine.h"
#include "BravoObject.h"



class BravoHUD : public BravoObject
{
public:
	BravoHUD(std::shared_ptr<class BravoEngine> _Engine, const BravoHandle& _Handle) :
		BravoObject(_Engine, _Handle)
	{}

};