#pragma once

template <typename T>
class MVector
{
public:
	using TDataType = T;

	void push_back(const std::string& dataName, const TDataType& data);
	void push_back(const std::string& dataName, const TDataType& data, int pointerFlag);
	template<typename Pred>
	void push_back(const std::string& dataName, const TDataType& data, Pred pred);

	void reserve(size_t capacity);
	void clear();

	bool empty();
	bool empty() const;

	bool find(const std::string& dataName);
	bool find(const std::string& dataName) const;

	size_t size();
	size_t size() const;

	auto& operator[](size_t i);
	auto& operator[](const std::string& dataName);
	auto& operator[](size_t i) const;

	auto begin();
	auto begin() const;
	auto end();
	auto end() const;

private:
	std::vector<TDataType> m_dataSet;
	std::unordered_map<std::string, TDataType> m_dataMap;
};

template<typename T>
inline void MVector<T>::push_back(const std::string& dataName, const TDataType& data)
{
	m_dataSet.insert(0, data);
	std::sort(m_dataSet.begin(), m_dataSet.end(), [](const TDataType& pass1, const TDataType& pass2) { return pass1 < pass2; });
	m_dataMap[dataName] = data;
}

template<typename T>
inline void MVector<T>::push_back(const std::string& dataName, const TDataType& data, int pointerFlag)
{
	m_dataSet.insert(std::lower_bound(m_dataSet.begin(), m_dataSet.end(), data, [](const TDataType& pass1, const TDataType& pass2) { return pass1 < pass2; }), data);
	m_dataMap[dataName] = data;
}

template<typename T>
template<typename Pred>
inline void MVector<T>::push_back(const std::string& dataName, const TDataType& data, Pred pred)
{
	m_dataSet.insert(std::lower_bound(m_dataSet.begin(), m_dataSet.end(), data, pred), data);
}

template<typename T>
inline void MVector<T>::reserve(size_t capacity)
{
	m_dataSet.reserve(capacity);
}

template<typename T>
inline void MVector<T>::clear()
{
	m_dataSet.clear();
	m_dataMap.clear();
}

template<typename T>
inline bool MVector<T>::empty()
{
	return m_dataSet.empty() || m_dataMap.empty();
}

template<typename T>
inline bool MVector<T>::empty() const
{
	return m_dataSet.empty() || m_dataMap.empty();
}

template<typename T>
inline bool MVector<T>::find(const std::string& dataName)
{
	return m_dataMap.find(dataName) != m_dataMap.end();
}

template<typename T>
inline bool MVector<T>::find(const std::string& dataName) const
{
	return m_dataMap.find(dataName) != m_dataMap.end();
}

template<typename T>
inline size_t MVector<T>::size()
{
	return static_cast<const MVector&>(*this).size();
}

template<typename T>
inline size_t MVector<T>::size() const
{
	return m_dataSet.size();
}

template<typename T>
inline auto& MVector<T>::operator[](size_t i)
{
	return const_cast<TDataType&>(static_cast<const MVector&>(*this)[i]);
}

template<typename T>
inline auto& MVector<T>::operator[](const std::string& dataName)
{
	assert(!dataName.empty() && m_dataMap.find(dataName) != m_dataMap.end());
	return m_dataMap[dataName];
}

template<typename T>
inline auto& MVector<T>::operator[](size_t i) const
{
	assert(i < m_dataSet.size());
	return m_dataSet[i];
}

template<typename T>
inline auto MVector<T>::begin()
{
	m_dataSet.begin();
}

template<typename T>
inline auto MVector<T>::begin() const
{
	m_dataSet.begin();
}

template<typename T>
inline auto MVector<T>::end()
{
	m_dataSet.end();
}

template<typename T>
inline auto MVector<T>::end() const
{
	m_dataSet.end();
}