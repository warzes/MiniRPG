#pragma once

class IGraphicsObject
{
public:
	IGraphicsObject() = default;
	IGraphicsObject(const std::string& objectName, int executionOrder);
	virtual ~IGraphicsObject() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

	const std::string& GetName() const { return m_name; }

private:
	std::string m_name;
};