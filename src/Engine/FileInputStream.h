#pragma once

#include "InputStream.h"

class FileInputStream final : public InputStream
{
public:
	FileInputStream() = default;
	~FileInputStream() final;
	FileInputStream(const FileInputStream&) = delete;
	FileInputStream& operator=(const FileInputStream&) = delete;
	FileInputStream(FileInputStream&&) noexcept = default;
	FileInputStream& operator=(FileInputStream&&) noexcept = default;

	[[nodiscard]] bool Open(const std::filesystem::path& filename);
	[[nodiscard]] std::int64_t Read(void* data, std::int64_t size) final;
	[[nodiscard]] std::int64_t Seek(std::int64_t position) final;
	[[nodiscard]] std::int64_t Tell() final;
	std::int64_t GetSize() final;

private:
	std::FILE* m_file = nullptr; // stdio file stream
};