// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module source_engine.vmf:entitydata;

export import pragma.filesystem;

export namespace source_engine::vmf {
	struct DataFileBlock {
		~DataFileBlock()
		{
			std::map<std::string, std::vector<DataFileBlock *> *>::iterator i;
			for(i = blocks.begin(); i != blocks.end(); i++) {
				for(int j = 0; j < i->second->size(); j++)
					delete(*i->second)[j];
				delete i->second;
			}
		}
		std::map<std::string, std::vector<std::string>> keyvalues;
		std::map<std::string, std::vector<DataFileBlock *> *> blocks;
		std::string KeyValue(std::string key, int i = 0)
		{
			std::map<std::string, std::vector<std::string>>::iterator it = keyvalues.find(key);
			if(it == keyvalues.end() || i >= it->second.size())
				return "";
			return it->second[i];
		}
	};

	class DataFile {
	  private:
		DataFile() = delete;
	  public:
		static DataFileBlock *ReadBlock(ufile::IFile &f, uint64_t readUntil = std::numeric_limits<uint64_t>::max());
		static DataFileBlock *Read(const char *f);
	};
};
