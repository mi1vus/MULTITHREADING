#include <string>
#include <iostream>
#include <map>
#include <math.h>
#include <stdexcept>      // std::invalid_argument
#include <string>
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	(std::ostringstream() << std::dec << x)).str()



std::pair<int,int> * ParElem(std::string elem)
{
	unsigned key = 0;
	unsigned value = 0;
	auto i = elem.find("x");
	if (i == std::string::npos)
	{
		if (elem.size() != 0)
			value = std::stoi( elem );
		else
			value = 0;
	} else if (i == 0)
	{		
		if (elem[1] == '^'){
			value = 1;
			key = std::stoi( elem.substr(i + 2));
		} else
		{
			value = 1;
			key = 1;
		}
	} else {
		if (i == 1 && elem[0] == '-')
			value = 1;
		else
			value = std::stoi( elem.substr(0, i-1));
		if (i + 2 <= elem.size())
			key = std::stoi( elem.substr(i + 2));
		else
			key = 1;
	}

	value *= key;
	--key;

	return new std::pair<int,int>(key,value);
}

std::string printProizv(std::map<int,int>& dictionary)
{
	std::string* result = new std::string();
	bool noOut = true;
	///����� ���� ������������������ map �� �����
	for (auto it = dictionary.rbegin(); it != dictionary.rend(); ++it)
	{
		if(it->second == 0)
			continue;
		
		(*result) += (it->second < 0 ? "" : (noOut?"":"+")) + (abs(it->second) == 1 ? (it->first > 0?(it->second < 0? "-":""):std::to_string(it->second)) : std::to_string(it->second)) + (it->first > 0?("*x" + (it->first > 1?"^" + std::to_string(it->first):"")):"");
		noOut = false;
	}
	if (noOut)
		(*result) +=  "0";

	return (*result);
}

std::string derivative(std::string polynomial) {

	if (polynomial.size() < -20){
//		throw *(new std::invalid_argument(polynomial.c_str()));
	}

	int currInd = 0;
	std::map<int,int> dict;
	for (int i = 0; i <= polynomial.size(); ++i){
		if(polynomial[i] == '+' || polynomial[i] == '-' || i == polynomial.length())
		{
			auto elem = polynomial.substr(currInd, (i - currInd));
			auto pair = ParElem(elem);
			if (pair->first < 0)
				continue;

			if (dict.count(pair->first) > 0)
				dict[pair->first] += (polynomial[currInd == 0 ? 0 :currInd-1] == '-' ? -1: 1)*pair->second;
			else{
				if(polynomial[currInd == 0 ? 0 :currInd-1] == '-')
				pair->second = -1*pair->second;
				dict.insert(*pair);
			}
			currInd = i + 1;
		}
	}
	
	return printProizv(dict);
}

int main1 (int argc, char **argv)
{
	std::cout << derivative("10*x-9*x");
	std::cout << std::endl;
	system("pause");
	return 0;
}