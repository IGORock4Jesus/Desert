#include "system_file.h"
#include <fstream>


namespace Desert::System
{
	File::File()
	{
	}

	File::File(std::string filename)
	{
		Load(filename);
	}


	File::~File()
	{
	}

	bool File::Load(std::string filename)
	{
		data = {};

		std::ifstream file{ filename, std::ios::end };
		if (file.bad() || !file.good())
			return false;

		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<unsigned char> buffer(size);
		file.read((char*)buffer.data(), size);

		data = move(buffer);

		return true;
	}

	const std::vector<unsigned char>& File::GetBytes() const
	{
		return data;
	}

	void File::SetData(const std::vector<unsigned char>& newData)
	{
		data = newData;
	}

	bool File::Save(std::string filename)
	{
		std::ofstream file{ filename };
		file.write((char*)data.data(), data.size());
		return false;
	}

}