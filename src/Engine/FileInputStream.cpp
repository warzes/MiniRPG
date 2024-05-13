#include "stdafx.h"
#include "FileInputStream.h"
//-----------------------------------------------------------------------------
FileInputStream::~FileInputStream()
{
	std::fclose(m_file);
}
//-----------------------------------------------------------------------------
bool FileInputStream::Open(const std::filesystem::path& filename)
{
#if PLATFORM_WINDOWS
	errno_t err = _wfopen_s(&m_file, filename.c_str(), L"rb");
	if (err != 0) return false;
#else
	m_file.reset(std::fopen(filename.c_str(), "rb"));
#endif
	return m_file != nullptr;
}
//-----------------------------------------------------------------------------
std::int64_t FileInputStream::Read(void* data, std::int64_t size)
{
	if (!m_file)
		return -1;
	return static_cast<std::int64_t>(std::fread(data, 1, static_cast<std::size_t>(size), m_file));
}
//-----------------------------------------------------------------------------
std::int64_t FileInputStream::Seek(std::int64_t position)
{
	if (!m_file)
		return -1;
	if (std::fseek(m_file, static_cast<long>(position), SEEK_SET))
		return -1;

	return Tell();
}
//-----------------------------------------------------------------------------
std::int64_t FileInputStream::Tell()
{
	if (!m_file)
		return -1;
	return std::ftell(m_file);
}
//-----------------------------------------------------------------------------
std::int64_t FileInputStream::GetSize()
{
	if (!m_file)
		return -1;
	const std::int64_t position = Tell();
	std::fseek(m_file, 0, SEEK_END);
	const std::int64_t size = Tell();

	if (Seek(position) == -1)
		return -1;

	return size;
}
//-----------------------------------------------------------------------------