#include "BravoTimeManager.h"

BravoTimerHandle BravoTimeManager::SetTimer(float Rate, bool bLoop, std::function<void()> func)
{
	if ( Rate <= 0.0f )
	{
		Log::LogMessage(ELog::Warning, "Timer with negative rate: {}. Ignoring.", Rate);
		return 0;
	}
	HandleCounter++;
	std::shared_ptr<BravoTimeManager::Timer> newTimer = std::shared_ptr<BravoTimeManager::Timer>(new BravoTimeManager::Timer(Rate, bLoop, func));
	ActiveTimers.insert({HandleCounter, newTimer});
	return HandleCounter;
}

void BravoTimeManager::DeleteTimer(const BravoTimerHandle& Handle)
{
	ActiveTimers.erase(Handle);
}

float BravoTimeManager::GetRate(const BravoTimerHandle& Handle)
{
	auto it = ActiveTimers.find(Handle);
	if ( it != ActiveTimers.end() )
	{
		return it->second->Rate;
	}
	return -1.0f;
}

float BravoTimeManager::GetElapsedTime(const BravoTimerHandle& Handle)
{
	auto it = ActiveTimers.find(Handle);
	if ( it != ActiveTimers.end() )
	{
		return it->second->Elapsed;
	}
	return -1.0f;
}

float BravoTimeManager::GetRemainingTime(const BravoTimerHandle& Handle)
{
	auto it = ActiveTimers.find(Handle);
	if ( it != ActiveTimers.end() )
	{
		return it->second->Rate - it->second->Elapsed;
	}
	return -1.0f;
}

void BravoTimeManager::Tick(float DeltaTime)
{
	for (auto it = ActiveTimers.begin(); it != ActiveTimers.end(); )
	{
		it->second->Elapsed += DeltaTime;

		if ( it->second->Elapsed >= it->second->Rate )
		{
			it->second->func();
			it->second->Elapsed = 0.0f;
			if ( !it->second->bLoop )
			{
				DeleteTimer(it->first);
				continue;
			}
		}
		it++;
	}
}