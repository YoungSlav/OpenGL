#pragma once
#include "stdafx.h"

class FluidMath
{
public:
	void SetRadius(float _radius)
	{
		Radius = _radius;
		Poly6ScalingFactor					= 4.0f  / (glm::pi<float>() * glm::pow(Radius, 8));
		SpikyPow3ScalingFactor				= 10.0f / (glm::pi<float>() * glm::pow(Radius, 5));
		SpikyPow2ScalingFactor				= 6.0f  / (glm::pi<float>() * glm::pow(Radius, 4));
		SpikyPow3DerivativeScalingFactor	= 30.0f / (glm::pi<float>() * glm::pow(Radius, 5));
		SpikyPow2DerivativeScalingFactor	= 12.0f / (glm::pi<float>() * glm::pow(Radius, 4));
	}

private:
	float Radius = 0.0f;
	float Poly6ScalingFactor = 0.0f;
	float SpikyPow3ScalingFactor = 0.0f;
	float SpikyPow2ScalingFactor = 0.0f;
	float SpikyPow3DerivativeScalingFactor = 0.0f;
	float SpikyPow2DerivativeScalingFactor = 0.0f;

public:

	float SmoothingKernelPoly6(float dst) const
	{
		if (dst < Radius)
		{
			float v = Radius * Radius - dst * dst;
			return v * v * v * Poly6ScalingFactor;
		}
		return 0;
	}

	float SpikyKernelPow3(float dst) const
	{
		if (dst < Radius)
		{
			float v = Radius - dst;
			return v * v * v * SpikyPow3ScalingFactor;
		}
		return 0;
	}

	float SpikyKernelPow2(float dst) const
	{
		if (dst < Radius)
		{
			float v = Radius - dst;
			return v * v * SpikyPow2ScalingFactor;
		}
		return 0;
	}

	float DerivativeSpikyPow3(float dst) const
	{
		if (dst <= Radius)
		{
			float v = Radius - dst;
			return v * v * SpikyPow3DerivativeScalingFactor;
		}
		return 0;
	}

	float DerivativeSpikyPow2(float dst) const
	{
		if (dst <= Radius)
		{
			float v = Radius - dst;
			return -v * SpikyPow2DerivativeScalingFactor;
		}
		return 0;
	}
};