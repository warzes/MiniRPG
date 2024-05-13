#pragma once

class Time
{
public:
	constexpr Time() = default;
	template <typename Rep, typename Period>
	constexpr Time(const std::chrono::duration<Rep, Period>& duration);

	constexpr float AsSeconds() const;
	constexpr std::int32_t AsMilliseconds() const;
	constexpr std::int64_t AsMicroseconds() const;

	constexpr std::chrono::microseconds ToDuration() const;

	template <typename Rep, typename Period>
	constexpr operator std::chrono::duration<Rep, Period>() const;

	static const Time Zero;
private:
	friend constexpr Time seconds(float);
	friend constexpr Time milliseconds(std::int32_t);
	friend constexpr Time microseconds(std::int64_t);

	std::chrono::microseconds m_microseconds{}; // Time value stored as microseconds
};

constexpr Time Seconds(float amount);
constexpr Time Milliseconds(std::int32_t amount);
constexpr Time Microseconds(std::int64_t amount);

[[nodiscard]] constexpr bool operator==(Time left, Time right);
[[nodiscard]] constexpr bool operator!=(Time left, Time right);
[[nodiscard]] constexpr bool operator<(Time left, Time right);
[[nodiscard]] constexpr bool operator>(Time left, Time right);
[[nodiscard]] constexpr bool operator<=(Time left, Time right);
[[nodiscard]] constexpr bool operator>=(Time left, Time right);
[[nodiscard]] constexpr Time operator-(Time right);
[[nodiscard]] constexpr Time operator+(Time left, Time right);
constexpr Time& operator+=(Time& left, Time right);
[[nodiscard]] constexpr Time operator-(Time left, Time right);
constexpr Time& operator-=(Time& left, Time right);
[[nodiscard]] constexpr Time operator*(Time left, float right);
[[nodiscard]] constexpr Time operator*(Time left, std::int64_t right);
[[nodiscard]] constexpr Time operator*(float left, Time right);
[[nodiscard]] constexpr Time operator*(std::int64_t left, Time right);
constexpr Time& operator*=(Time& left, float right);
constexpr Time& operator*=(Time& left, std::int64_t right);
[[nodiscard]] constexpr Time operator/(Time left, float right);
[[nodiscard]] constexpr Time operator/(Time left, std::int64_t right);
constexpr Time& operator/=(Time& left, float right);
constexpr Time& operator/=(Time& left, std::int64_t right);
[[nodiscard]] constexpr float operator/(Time left, Time right);
[[nodiscard]] constexpr Time operator%(Time left, Time right);
constexpr Time& operator%=(Time& left, Time right);

template <typename Rep, typename Period>
constexpr Time::Time(const std::chrono::duration<Rep, Period>& duration) : m_microseconds(duration)
{
}

constexpr float Time::AsSeconds() const
{
	return std::chrono::duration<float>(m_microseconds).count();
}

constexpr std::int32_t Time::AsMilliseconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<std::int32_t, std::milli>>(m_microseconds).count();
}

constexpr std::int64_t Time::AsMicroseconds() const
{
	return m_microseconds.count();
}

constexpr std::chrono::microseconds Time::ToDuration() const
{
	return m_microseconds;
}

template <typename Rep, typename Period>
constexpr Time::operator std::chrono::duration<Rep, Period>() const
{
	return m_microseconds;
}

constexpr Time Seconds(float amount)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(amount));
}

constexpr Time Milliseconds(std::int32_t amount)
{
	return std::chrono::milliseconds(amount);
}

constexpr Time Microseconds(std::int64_t amount)
{
	return std::chrono::microseconds(amount);
}

constexpr bool operator==(Time left, Time right)
{
	return left.AsMicroseconds() == right.AsMicroseconds();
}

constexpr bool operator!=(Time left, Time right)
{
	return left.AsMicroseconds() != right.AsMicroseconds();
}

constexpr bool operator<(Time left, Time right)
{
	return left.AsMicroseconds() < right.AsMicroseconds();
}

constexpr bool operator>(Time left, Time right)
{
	return left.AsMicroseconds() > right.AsMicroseconds();
}

constexpr bool operator<=(Time left, Time right)
{
	return left.AsMicroseconds() <= right.AsMicroseconds();
}

constexpr bool operator>=(Time left, Time right)
{
	return left.AsMicroseconds() >= right.AsMicroseconds();
}

constexpr Time operator-(Time right)
{
	return Microseconds(-right.AsMicroseconds());
}

constexpr Time operator+(Time left, Time right)
{
	return Microseconds(left.AsMicroseconds() + right.AsMicroseconds());
}

constexpr Time& operator+=(Time& left, Time right)
{
	return left = left + right;
}

constexpr Time operator-(Time left, Time right)
{
	return Microseconds(left.AsMicroseconds() - right.AsMicroseconds());
}

constexpr Time& operator-=(Time& left, Time right)
{
	return left = left - right;
}

constexpr Time operator*(Time left, float right)
{
	return Seconds(left.AsSeconds() * right);
}

constexpr Time operator*(Time left, std::int64_t right)
{
	return Microseconds(left.AsMicroseconds() * right);
}

constexpr Time operator*(float left, Time right)
{
	return right * left;
}

constexpr Time operator*(std::int64_t left, Time right)
{
	return right * left;
}

constexpr Time& operator*=(Time& left, float right)
{
	return left = left * right;
}

constexpr Time& operator*=(Time& left, std::int64_t right)
{
	return left = left * right;
}

constexpr Time operator/(Time left, float right)
{
	assert(right != 0 && "Time::operator/ cannot divide by 0");
	return Seconds(left.AsSeconds() / right);
}

constexpr Time operator/(Time left, std::int64_t right)
{
	assert(right != 0 && "Time::operator/ cannot divide by 0");
	return Microseconds(left.AsMicroseconds() / right);
}

constexpr Time& operator/=(Time& left, float right)
{
	assert(right != 0 && "Time::operator/= cannot divide by 0");
	return left = left / right;
}

constexpr Time& operator/=(Time& left, std::int64_t right)
{
	assert(right != 0 && "Time::operator/= cannot divide by 0");
	return left = left / right;
}

constexpr float operator/(Time left, Time right)
{
	assert(right.AsMicroseconds() != 0 && "Time::operator/ cannot divide by 0");
	return left.AsSeconds() / right.AsSeconds();
}

constexpr Time operator%(Time left, Time right)
{
	assert(right.AsMicroseconds() != 0 && "Time::operator% cannot modulus by 0");
	return Microseconds(left.AsMicroseconds() % right.AsMicroseconds());
}

constexpr Time& operator%=(Time& left, Time right)
{
	assert(right.AsMicroseconds() != 0 && "Time::operator%= cannot modulus by 0");
	return left = left % right;
}

inline constexpr Time Time::Zero;