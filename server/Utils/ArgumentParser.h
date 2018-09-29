#pragma once

#include <vector>

class ArgumentParser {
	using Strings = std::vector<std::string>;
	template <typename T>
	using ConvertFunction = T(*)(const std::string&);
public:
	ArgumentParser(int argc, char** argv) : args(argc) {
		for (int i = 0; i < argc; i++) {
			this->args.push_back(argv[i]);
		}
	}

	template <typename ResultType>
	ResultType getArgumentValue(const Strings &names, const ResultType &default) {
		return getArgumentValue(names, default, ArgumentParser::convertFunction<ResultType>);
	}

	template <typename ResultType>
	ResultType getArgumentValue(const Strings &names, const ResultType &default, ConvertFunction<ResultType> convertFunction) {
		for (auto &name : names) {
			auto nameIt = std::find(this->args.begin(), this->args.end(), name);
			if (nameIt == this->args.end()) continue;

			auto valueIt = nameIt + 1;
			if (valueIt == this->args.end()) continue;

			return convertFunction(*valueIt);
		}
		return default;
	}

	template<typename Integral, typename = std::enable_if_t<std::is_integral<Integral>::value>>
	static Integral convertFunction(const std::string &arg) {
		return static_cast<Integral>(std::stoll(arg));
	}

	template<typename String, typename = std::enable_if_t<std::is_same<String,std::string>::value>>
	static std::string convertFunction(const std::string &arg) {
		return arg;
	}

private:
	Strings args;
};
