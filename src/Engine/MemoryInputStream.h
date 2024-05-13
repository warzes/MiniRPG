#pragma once

#include "InputStream.h"

class MemoryInputStream final : public InputStream
{
public:
	void Open(const void* data, std::size_t sizeInBytes);
	[[nodiscard]] std::int64_t Read(void* data, std::int64_t size) override;
	[[nodiscard]] std::int64_t Seek(std::int64_t position) override;
	[[nodiscard]] std::int64_t Tell() override;
	std::int64_t GetSize() override;

private:
	const std::byte* m_data{};   // Pointer to the data in memory
	std::int64_t     m_size{};   // Total size of the data
	std::int64_t     m_offset{}; // Current reading position
};