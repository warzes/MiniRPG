#include "stdafx.h"
#include "GenerateUUID.h"

namespace
{
	std::random_device rd;
	std::mt19937_64 e2(rd());
	std::uniform_int_distribution<uint64_t> dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));
}

uint64_t GenerateUUID()
{
	return dist(e2);
}