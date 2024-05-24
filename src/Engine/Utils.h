#pragma once

// TODO:
inline std::string ReadTextFile(std::string_view filepath)
{
	if (!std::filesystem::exists(filepath.data()))
	{
		std::ostringstream message;
		message << "file " << filepath.data() << " does not exist.";
		throw std::filesystem::filesystem_error(message.str(), std::make_error_code(std::errc::no_such_file_or_directory));
	}
	std::ifstream file(filepath.data());
	return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

inline std::string ToLower(std::string str)
{
	for (char& c : str)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	return str;
}

inline std::string FormatDebugPathInfo(const std::filesystem::path& path)
{
	std::ostringstream oss;
	oss << "    Provided path: " << path << '\n' //
		<< "    Absolute path: " << std::filesystem::absolute(path);
	return oss.str();
}