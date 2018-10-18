#pragma once

#include <string>
#include <vector>


namespace Desert::System
{

	class File
	{
		std::vector<unsigned char> data;

	public:
		File();
		File(std::string filename);
		~File();

		bool Load(std::string filename);

		const std::vector<unsigned char>& GetBytes() const;

		void SetData(const std::vector<unsigned char>& newData);

		bool Save(std::string filename);
	};

}