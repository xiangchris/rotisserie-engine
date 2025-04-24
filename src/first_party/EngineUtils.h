#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

class EngineUtils
{
public:
	// Read a file to json document. Overwrites existing document
	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
	{
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		const size_t buffer_size = 65536;
		char* buffer = new char[buffer_size];
		rapidjson::FileReadStream stream(file_pointer, buffer, buffer_size);
		out_document.ParseStream(stream);
		std::fclose(file_pointer);

		if (!file_pointer) {
			std::cerr << "error failed to open file [" << path << "]" << std::endl;
			exit(0);
		}

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);
		}
		delete[] buffer;
	}

	// Write a json document to file. Overwrites existing files
	static void WriteJsonFile(const std::string& path, rapidjson::Document& doc)
	{
		// Create a file stream to write to a file
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "w");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif

		if (!file_pointer) {
			std::cerr << "error failed to open file [" << path << "]" << std::endl;
			exit(0);
		}

		const size_t buffer_size = 65536;
		char* buffer = new char[buffer_size];
		rapidjson::FileWriteStream stream(file_pointer, buffer, buffer_size);

		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
		doc.Accept(writer);

		std::fclose(file_pointer);
	}

	// Get value from json
	template <typename T>
	static void GetConfigValue(const rapidjson::Value& doc, const char* key, T& result)
	{
		// Find member in document
		rapidjson::Value::ConstMemberIterator itr = doc.FindMember(key);
		if (itr != doc.MemberEnd())
		{
			if constexpr (std::is_same_v<T, std::string>)
				result = itr->value.GetString();
			else if constexpr (std::is_same_v<T, float>)
				result = itr->value.GetFloat();
			else if constexpr (std::is_same_v<T, int>)
				result = itr->value.GetInt();
			else if constexpr (std::is_same_v<T, bool>)
				result = itr->value.GetBool();
			// Add more types if needed
		}
	}

	static void GetComponentOverride(luabridge::LuaRef& ref, const std::string& key, const rapidjson::Value& val)
	{
		if (val.IsString())
			ref[key] = val.GetString();
		else if (val.IsFloat())
			ref[key] = val.GetFloat();
		else if (val.IsInt())
			ref[key] = val.GetInt();
		else if (val.IsBool())
			ref[key] = val.GetBool();
	}

	// Report an error
	static void ReportError(const std::string& actor_name, const luabridge::LuaException& e)
	{
		std::string error_msg = e.what();

		// Normalize file paths across platforms
		std::replace(error_msg.begin(), error_msg.end(), '\\', '/');

		// Display with color codes
		std::cout << "\033[31m" << actor_name << " : " << error_msg << "\033[0m" << std::endl;
	}
};

#endif
