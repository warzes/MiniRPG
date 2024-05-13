#include "stdafx.h"
#include "MemoryInputStream.h"
//-----------------------------------------------------------------------------
void MemoryInputStream::Open(const void* data, std::size_t sizeInBytes)
{
	m_data = static_cast<const std::byte*>(data);
	m_size = static_cast<std::int64_t>(sizeInBytes);
	m_offset = 0;
}
//-----------------------------------------------------------------------------
std::int64_t MemoryInputStream::Read(void* data, std::int64_t size)
{
	if (!m_data)
		return -1;

	const std::int64_t endPosition = m_offset + size;
	const std::int64_t count = endPosition <= m_size ? size : m_size - m_offset;

	if (count > 0)
	{
		std::memcpy(data, m_data + m_offset, static_cast<std::size_t>(count));
		m_offset += count;
	}

	return count;
}
//-----------------------------------------------------------------------------
std::int64_t MemoryInputStream::Seek(std::int64_t position)
{
	if (!m_data)
		return -1;

	m_offset = position < m_size ? position : m_size;
	return m_offset;
}
//-----------------------------------------------------------------------------
std::int64_t MemoryInputStream::Tell()
{
	if (!m_data)
		return -1;

	return m_offset;
}
//-----------------------------------------------------------------------------
std::int64_t MemoryInputStream::GetSize()
{
	if (!m_data)
		return -1;

	return m_size;
}
//-----------------------------------------------------------------------------