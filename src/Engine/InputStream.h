#pragma once

class InputStream
{
public:
	virtual ~InputStream() = default;

	// Read data from the stream
	/// After reading, the stream's reading position must be advanced by the amount of bytes read.
	[[nodiscard]] virtual std::int64_t Read(void* data, std::int64_t size) = 0;

	// Change the current reading position
	[[nodiscard]] virtual std::int64_t Seek(std::int64_t position) = 0;

	// Get the current reading position in the stream
	[[nodiscard]] virtual std::int64_t Tell() = 0;

	// Return the size of the stream
	virtual std::int64_t GetSize() = 0;
};