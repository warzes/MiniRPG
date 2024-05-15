#pragma once

// TODO:
std::string ReadTextFile(std::string_view filepath)
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
