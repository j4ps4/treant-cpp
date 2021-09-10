#include <fstream>
#include <charconv>


template<size_t NX>
std::vector<std::tuple<int,Attacker<NX>*>> parse_batch_file(const std::string& batch_file,
	const std::string& attack_file, int budget)
{
	static const auto chars = {',', '\n'};
	std::vector<std::tuple<int,Attacker<NX>*>> result;
	try
	{
		std::string line;
		std::ifstream is(batch_file);
		while (!is.eof())
		{
			std::getline(is, line);
			if (line.empty())
				continue;
	        auto pos = line.find(':');
			int id;
			std::from_chars(line.c_str(), line.c_str()+pos, id);
			auto rest = line.substr(pos+1);
			auto searchBeg = rest.data();
			const auto dataEnd = searchBeg + line.size();
			std::set<size_t> id_set;
			while (searchBeg < dataEnd)
			{
				auto comma = std::find_first_of(searchBeg, dataEnd, chars.begin(), chars.end());
				size_t fid;
				std::from_chars(searchBeg, comma, fid);
				searchBeg = comma+1;
				id_set.insert(fid);
			}

			auto res = load_attack_rules(attack_file, {}, id_set);
			if (res.has_error())
				throw std::runtime_error(res.error());
			auto& rulz = res.value();
			auto atkr = new Attacker<NX>(std::move(rulz), budget);
			result.push_back(std::make_tuple(id, atkr));
		}
		return result;
	}
	catch (std::exception& e)
	{
		Util::die("can't read batch file: {}", e.what());
	}
}