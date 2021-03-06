#pragma once
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <utility>
#include <set>
#include <stack>
#include <iostream>
#include <fstream>

class ibitfstream : public std::ifstream {
	unsigned char buffer, bitcount;
public:
	ibitfstream() : bitcount(0) {}
	ibitfstream(const char* fname) : std::ifstream(fname, std::ios_base::binary), bitcount(0) {}
	int getbit() {
		if (bitcount == 8) bitcount = 0;
		if (bitcount == 0) buffer = std::ifstream::get();  // get();
		int res;
		res = (buffer & (1 << (7 - bitcount))) ? 1 : 0;
		++bitcount;
		return res;
	}
	bool eof() {
		if (bitcount > 0 && bitcount < 8) return false;
		if (peek() != EOF) return false;
		return true;
	}
};
class obitfstream : public std::ofstream {
	unsigned char buffer, bitcount;
public:
	obitfstream() : buffer(0), bitcount(0) {}
	obitfstream(const char* fname) : std::ofstream(fname, std::ios_base::binary), bitcount(0), buffer(0) {}
	void putBit(bool x) {
		unsigned char ch = x;
		buffer = buffer | (ch << (7 - bitcount)); // buffer |= ( ch << (7-bitcount));
		++bitcount;
		if (bitcount == 8) {
			put(buffer); // std::ofstream::put();
			bitcount = 0; buffer = 0;
		}
	}
	void close() {
		if (bitcount > 0) put(buffer);
		bitcount = 0; buffer = 0;
		std::ofstream::close();
	}
};

class Node {

private:
	Node()
		:symbol(-1)
		,frequency(0)
		,left(nullptr)
		,right(nullptr)
	{}
public:
	int symbol;
	unsigned long long frequency;
	const Node* left;
	Node* Left()
	{
		return const_cast<Node*>(left);
	}
	const Node* right;
	Node* Right()
	{
		return const_cast<Node*>(right);
	}

	Node(char _symbol, unsigned long long _frequency)
		:symbol(_symbol)
		,frequency(_frequency)
		,left(nullptr)
		,right(nullptr)
	{};
	Node(Node const& a, Node const& b)
		:symbol(-1)
		, frequency(a.frequency + b.frequency)
		, left(&a)
		, right(&b)
	{};
	Node(Node const& a)
		:symbol(a.symbol)
		, frequency(a.frequency)
		, left(a.left)
		, right(a.right)
	{};
	~Node()
	{
		if(left != nullptr)
			delete left;
		if(right != nullptr)
			delete right;
	}
	bool operator<(Node const& toCompare) const
	{
		if (this->frequency < toCompare.frequency)
			return true;
		else
			return (this->frequency == toCompare.frequency
				&& this->symbol < toCompare.symbol);
	}
	bool operator>(Node const& toCompare) const
	{
		if (this->frequency > toCompare.frequency)
			return true;
		else
			return (this->frequency == toCompare.frequency
				&& this->symbol > toCompare.symbol);
	}

	Node& operator=(Node const& SourceValue)
	{
		symbol = SourceValue.symbol;
		frequency = SourceValue.frequency;
		left = SourceValue.left;
		right = SourceValue.right;
		return *this;
	}

	Node CreateRoot(Node& Top)
	{
		Node a;
		a.left = &Top;
		return a;
	}
};


static class Coder
{
protected:
	static Node getTree(std::map<char, unsigned long long> const& FrequencyTable)
	{
		std::vector<Node*> OrderingTree;
		OrderingTree.reserve(FrequencyTable.size());
		std::for_each(
			FrequencyTable.begin(),
			FrequencyTable.end(),
			[&OrderingTree](std::pair<char, unsigned long long> ToInsert)
			{
				OrderingTree.push_back(new Node(ToInsert.first, ToInsert.second));
			}
		);
		std::sort(OrderingTree.begin()
			, OrderingTree.end()
			, [](Node* a, Node* b)
			{ return (*a) < (*b); });
		while (OrderingTree.size() > 1)
		{
			auto& first = **(OrderingTree.begin());
			auto& second = **(++OrderingTree.begin());
			OrderingTree.push_back(new Node(first, second));
			OrderingTree.erase(OrderingTree.begin(), ++++OrderingTree.begin());
			std::sort(OrderingTree.begin()
				, OrderingTree.end()
				, [](Node* a, Node* b)
				{ return (*a) < (*b); });
		}
		return **(OrderingTree.begin());
	}
	static std::map<char, std::vector<bool>> FindAllChars(Node& Root)
	{
		std::map<char, std::vector<bool>> result;

		std::stack<std::pair<Node*, std::vector<bool>>> toGo;
		toGo.push(std::make_pair(&Root, std::vector<bool>()));
		while (!toGo.empty())
		{
			auto i = toGo.top();
			toGo.pop();
			if (i.first->left == nullptr && i.first->right == nullptr)
			{
				result.insert(std::make_pair(i.first->symbol, i.second));
			}
			else
			{
				if (i.first->left != nullptr)
				{
					std::vector<bool> tmp = i.second;
					tmp.push_back(false);
					toGo.push(std::make_pair(i.first->Left(), tmp));
				}
				if (i.first->right != nullptr)
				{
					std::vector<bool> tmp = i.second;
					tmp.push_back(true);
					toGo.push(std::make_pair(i.first->Right(), tmp));
				}
			}
		}
		return result;
	}
	static std::map<char, std::vector<bool>> getCypher(std::map<char, unsigned long long> FrequencyTable)
	{
		auto TreeRoot = getTree(FrequencyTable);
		return FindAllChars(TreeRoot);
	}
};

static class Encoder: public Coder
{
private:
	static std::map<char, unsigned long long> getFrequencyTable(std::string const& text)
	{
		std::ifstream in(text, std::ios_base::binary);
		std::map<char, unsigned long long> result;
		while(!in.eof())
		{
			char sign = in.get();
			auto itr = result.find(sign);
			if (itr == result.end())
				result.insert(std::make_pair(sign, 1));
			else
				++(itr->second);
			}
		in.close();
		return result;
	}
	static std::map<char, unsigned long long> getFrequencyTable(std::ifstream& input)
	{
		std::map<char, unsigned long long> result;
		while (!input.eof()) {
			char sign = input.get();
			auto itr = result.find(sign);
			if (itr == result.end())
				result.insert(std::make_pair(sign, 1));
			else
				++(itr->second);	
		}
		return result;
	}
	
	static void writeLongLong(std::ofstream& out, unsigned long long toWrite)
	{
		char* str = reinterpret_cast<char*>(&toWrite);
		for (int i = 0; i < 8; ++i)
			out << str[i];
	}
	static void writeInt(std::ofstream& out, int toWrite)
	{
		char* str = reinterpret_cast<char*>(&toWrite);
		for (int i = 0; i < 4; ++i)
			out << str[i];
	}

	static void WriteEncoded(
		std::string& inputFile
		, std::string& outputFile
		, std::map<char, unsigned long long> FrequencyTable
		, std::map<char, std::vector<bool>> Cypher)
	{
		obitfstream out(outputFile.c_str());
		std::ifstream in(inputFile, std::ios_base::binary);
		writeInt(out, FrequencyTable.size());
		std::for_each(
			FrequencyTable.begin()
			, FrequencyTable.end()
			, [&out, &FrequencyTable](std::pair<char, unsigned long long> toWrite)
			{
				out << toWrite.first;
				char* vect = reinterpret_cast<char*>(&toWrite.second);
				for (int i = 0; i < 8; ++i)
					out << vect[i];
			}
		);
		long long q = 0;
		std::for_each(
			FrequencyTable.begin(),
			FrequencyTable.end(),
			[&q](std::pair<char, unsigned long long> toWrit) {q += toWrit.second; }
		);

		int qwe = 0;
		while (!in.eof())
		{		
			++qwe;
			char symbol = in.get();
			auto writeArray = Cypher.find(symbol)->second;
			for (int i = 0; i < writeArray.size() ;++i)
				out.putBit(writeArray[i]);
		}
		out.close();
		in.close();
	}


public:
	static void Huffman(std::string textFileInput, std::string textFileOutput)
	{
		auto FrequencyTable = Encoder::getFrequencyTable(textFileInput);
		auto Cypher = Coder::getCypher(FrequencyTable);
		WriteEncoded(textFileInput, textFileOutput, FrequencyTable, Cypher);
	}
};

static class Decoder : private Coder
{
private:
	static char ReadChar(std::ifstream& source)
	{
		return source.get();
	}
	static unsigned int ReadInt(std::ifstream& source)
	{
		char buf[4];
		for (int i = 0; i < 4; ++i)
			buf[i] = source.get();
		return  *(reinterpret_cast<int*>(&buf));
	}
	static unsigned long long ReadLongLong(std::ifstream& source)
	{
		char buf[8];
		for (int i = 0; i < 8; ++i)
			buf[i] = source.get();
		return  *(reinterpret_cast<long long*>(&buf));
	}
	static void WriteDecoded(
		std::string& inputFile
		, std::string& outputFile)
	{
		ibitfstream in(inputFile.c_str());
		std::ofstream out(outputFile, std::ios_base::binary);

		int SymbolsCount = ReadInt(in);

		std::map<char, unsigned long long> FrequencyTable;
		for (int i = 0; i < SymbolsCount; ++i)
		{
			std::pair<char, unsigned long long> readField;
			readField.first = ReadChar(in);
			readField.second = ReadLongLong(in);
			FrequencyTable.insert(readField);
		}		
		auto Cypher = Coder::getCypher(FrequencyTable);

		std::map<std::vector<bool>, char> ReversedCypher;
		std::for_each(
			Cypher.begin()
			, Cypher.end()
			, [&ReversedCypher](std::pair<char, std::vector<bool>> ToInsert)
			{
				ReversedCypher.insert(std::make_pair(ToInsert.second, ToInsert.first));
			});

		std::vector<bool> buffer;
		long long summarySymbols = -1;
		std::for_each(
			FrequencyTable.begin()
			,FrequencyTable.end()
			, [&summarySymbols](std::pair<char, long long> toSumming)->void
			{summarySymbols += toSumming.second; });

		while (summarySymbols)
		{
			buffer.push_back(in.getbit());
			if (ReversedCypher.find(buffer) != ReversedCypher.end())
			{
				out << ReversedCypher[buffer];
				buffer.clear();
				--summarySymbols;
			}
		}




		out.close();
		in.close();
	}
public:
	static void Huffman(std::string textFileInput, std::string textFileOutput)
	{
		WriteDecoded(textFileInput, textFileOutput);
	}
};

