#pragma once
#include "stdafx.h"
#include <functional>
#include "BravoObject.h"


typedef uint32 BravoTimerHandle;

class BravoTimeManager : public BravoObject, public IBravoTickable
{
private:
	struct Timer
	{
		Timer(float _Rate, bool _bLoop, std::function<void()> _func) :
			Rate(_Rate),
			bLoop(_bLoop),
			func(_func),
			Elapsed(0.0)
		{}

		float Rate = -1.0f;
		bool bLoop = false;
		std::function<void()> func;

		float Elapsed = 0.0f;
	};

public:
	template <typename... Args>
	BravoTimeManager(Args&&... args) :
		BravoObject(std::forward<Args>(args)...),
		IBravoTickable()
	{}

	BravoTimerHandle SetTimer(float Rate, bool bLoop, std::function<void()> func);
	void DeleteTimer(const BravoTimerHandle& Handle);
	
	float GetElapsedTime(const BravoTimerHandle& Handle);
	float GetRate(const BravoTimerHandle& Handle);
	float GetRemainingTime(const BravoTimerHandle& Handle);


protected:
	virtual void Tick(float DeltaTime) override;

private:
	BravoTimerHandle HandleCounter = 0;
	std::map<BravoTimerHandle, std::shared_ptr<Timer>> ActiveTimers;
};
