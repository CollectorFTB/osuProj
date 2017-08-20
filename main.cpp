#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

class HitObject
{
	private:
		double x, y = 0;
		string type = "";
		int time = 0;
	/*
	public HitObject(int x, int y, string type, int time) :x(x), y(	y), type(type), time(time)
	{
		int c = 3;
	}*/
};

char* binaryToString(int num)
{
	char* numData = new char[8];
	for (int j = 0, i = num; j<7 ; i/=10, j++)
	{
		numData[6 - j] = (i % 2) + 48;
	}
	numData[7] = '\0';
	return numData;
}


int toBinary(int num)
{
	if (!num)
		return 0;
	else
		return (num % 2 + 10 * toBinary(num / 2));
}

int stringAsInt(const string& number)
{
	int re = 0;
	for (int i = 0; i<number.length() ; i++)
	{
		re += number[i] - 48;
		re *= 10;
	}
	return re / 10;
}

string* split(const string& in, const char& devider)
{
	int counter = 0;
	for (int i = 0; i < in.length(); i++)
		if (in[i] == devider)
			counter++;

	string* data = new string[counter + 1];

	string temp = "";
	int j = 0;
	for (int i = 0; i < in.length(); i++)
		if (in[i] == devider)
		{
			data[j++] = temp;
			temp = "";
		}
		else
		{
			temp += in[i];
		}
	data[j] = temp;
	return data;
}
/*
ms / 1000 = s
*/
int main()
{
	string line = "";
	ifstream myfile;
	myfile.open("Songs/FAF/FAF.osu");

	string* objectData;
	list<string*> fileData = list<string*>();


	bool flag = false;
	int i = 0;
	while(getline(myfile, line))
	{
		objectData = split(line, ',');
		if (objectData[0] == "[HitObjects]")
		{
			flag = true;
			continue;
		}
		if (flag)
			fileData.push_back(objectData);
	}
	
	while(!fileData.empty())
	{
		string* pdata = fileData.back();
		double x = round(100 * stringAsInt(pdata[0]) / 32.0) / 100;
		double y = round(100 * stringAsInt(pdata[1]) / 32.0) / 100;
		int timing = stringAsInt(pdata[2]);
		string type = binaryToString(toBinary(stringAsInt(pdata[3])));
		cout << '(' << x << ',' << y << ')' << "\ttype = " << type << endl;
		fileData.pop_back();
	}

	cin.ignore();
	return 0;
}