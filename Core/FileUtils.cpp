#include "Engine/Core/FileUtils.hpp"

#include <stdio.h>

int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename)
{
	outBuffer.clear();

	FILE* file = nullptr;
	errno = 0;

	// Binary read
	int openErr = fopen_s(&file, filename.c_str(), "rb");
	if (openErr != 0 || file == nullptr)
	{
		return -1;
	}

	// Seek to end to get size
	if (fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		return -2;
	}

	long fileSizeLong = ftell(file);
	if (fileSizeLong < 0)
	{
		fclose(file);
		return -3;
	}

	// Return to beginning
	if (fseek(file, 0, SEEK_SET) != 0)
	{
		fclose(file);
		return -4;
	}

	size_t fileSize = static_cast<size_t>(fileSizeLong);

	// Handle empty file
	if (fileSize == 0)
	{
		fclose(file);
		return 0;
	}

	outBuffer.resize(fileSize);

	// Read the whole file
	size_t bytesRead = fread(outBuffer.data(), 1, fileSize, file);

	// Always close before returning
	fclose(file);
	file = nullptr;

	if (bytesRead != fileSize)
	{
		// Shrink to what we actually got
		outBuffer.resize(bytesRead);
		return -5;
	}

	return static_cast<int>(bytesRead);
}

int FileReadToString(std::string& outString, const std::string& filename)
{
	outString.clear();

	std::vector<unsigned char> buffer;
	int bytesRead = FileReadToBuffer(buffer, filename);
	if (bytesRead < 0)
	{
		return bytesRead;
	}

	buffer.push_back('\0');

	outString = std::string(reinterpret_cast<char const*>(buffer.data()));

	return bytesRead;
}

