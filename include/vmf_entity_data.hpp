/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __VMF_ENTITY_DATA_HPP__
#define __VMF_ENTITY_DATA_HPP__

#include <map>
#include <vector>
#include <fsys/filesystem.h>

namespace vmf
{
	struct DataFileBlock
	{
		~DataFileBlock()
		{
			std::map<std::string,std::vector<DataFileBlock*>*>::iterator i;
			for(i=blocks.begin();i!=blocks.end();i++)
			{
				for(int j=0;j<i->second->size();j++)
					delete (*i->second)[j];
				delete i->second;
			}
		}
		std::map<std::string,std::vector<std::string>> keyvalues;
		std::map<std::string,std::vector<DataFileBlock*>*> blocks;
		std::string KeyValue(std::string key,int i=0)
		{
			std::map<std::string,std::vector<std::string>>::iterator it = keyvalues.find(key);
			if(it == keyvalues.end() || i >= it->second.size())
				return "";
			return it->second[i];
		}
	};

	class DataFile
	{
	private:
		DataFile()=delete;
	public:
		static DataFileBlock *ReadBlock(VFilePtr f,uint64_t readUntil=std::numeric_limits<uint64_t>::max());
		static DataFileBlock *Read(const char *f);
	};
};

#endif
