#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <time.h>
#include <robotarm.h>
#include <unistd.h>




using namespace std;

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

int roundDoubleNDigits(const double& num, const char& digit)
{
	return round( pow(10,digit) * num ) / pow(10,digit);
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


void playMove(robotarm& myRobot, LINEARMOTION& note/*, bool waitAtEnd*/, double playTime) {
	myRobot.movj(note);
	/*
	if (waitAtEnd) {
		waitUntilPosReached(0.3, myRobot);
	}
	*/
	usleep(playTime);
	/*
	if (playTime == 0) {
		waitUntilPosReached(0.3, myRobot);
	}
	
	waitUntilPosReached(0.3, myRobot);
	*/
}

/*
int waitUntilPosReached(double minThreashdold, robotarm & myRobot) {
	//waits until distance between robot positions in times t1, t2 is below minThreashold
	RPOSC currPos = { 0 };
	RPOSC prevPos = { 0 };
	cntWaits++;
	prevPos = myRobot.rposc(0, 0);
	currPos = myRobot.rposc(0, 0);
	//Wait until robot starts moving
	while (dist3D(prevPos.Coord.x, prevPos.Coord.y, prevPos.Coord.z, currPos.Coord.x, currPos.Coord.y, currPos.Coord.z)<0.001) {
		//cout << "*delta distance waiting for move: " << dist3D(prevPos.Coord.x,prevPos.Coord.y,prevPos.Coord.z,currPos.Coord.x, currPos.Coord.y,currPos.Coord.z) << endl;
		prevPos = currPos;
		currPos = myRobot.rposc(0, 0);
	}

	//Wait until robot stops moving
	while (dist3D(prevPos.Coord.x, prevPos.Coord.y, prevPos.Coord.z, currPos.Coord.x, currPos.Coord.y, currPos.Coord.z)>0.001) {
		//cout << "*delta distance waiting for stop: " << dist3D(prevPos.Coord.x,prevPos.Coord.y,prevPos.Coord.z,currPos.Coord.x, currPos.Coord.y,currPos.Coord.z) << endl;
		prevPos = currPos;
		currPos = myRobot.rposc(0, 0);
	}
	//cout << "wait number " << cntWaits << endl;
	return 0;
}

*/

/*
double dist3D(double x1, double y1, double z1, double x2, double y2, double z2) {
	//Estimate 3D distance between two points
	double distx = pow(x1 - x2, 2);
	double disty = pow(x1 - x2, 2);
	double distz = pow(x1 - x2, 2);
	double sqrtDist3d = sqrt(distx + disty + distz);
	return sqrtDist3d;
}
*/

int main()
{	

	robotarm myRobot("192.168.100.10", 80, "thisTraceFile.log", "thisCommLog.log");// DLR robot APIo
	//Connect to robot
	int connectReturnCode = myRobot.Connect(true);  //true - llong conn, false - onetime
	myRobot.svon(true);

	JOINMOTION start = { 0 };
	start.Pos.Coord.x = 452.430;
	start.Pos.Coord.y = -2.233;
	start.Pos.Coord.z = -0.828;
	start.Pos.R.x = -179.5431;
	start.Pos.R.y = -0.8233;
	start.Pos.R.z = -2.0924;
	start.MotionSpeedPerc = 50;

	// move to start position
	myRobot.movj(start);
	// pause
	usleep(1000);

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

	int[][] dataArr = new int[10000][3];
	int deltaT = 0;
	string* pdata;
	double x = 0, y = 0;
	int timing = 0, oldTiming = 0;
	string type;
	int i = 0;
	while(!fileData.empty())
	{
		pdata = fileData.front();
		x = roundDoubleNDigits(stringAsInt(pdata[0]) / 16.0, 2);
		y = roundDoubleNDigits(stringAsInt(pdata[1]) / 16.0, 2);
		timing = stringAsInt(pdata[2]);
		type = binaryToString(toBinary(stringAsInt(pdata[3])));
		deltaT = timing - oldTiming;
		oldTiming = timing;
		if (type != 2)
		{
			dataArr[i][0] = x;
			dataArr[i][1] = y;
			dataArr[i][2] = timing;
			i++;
		}
		else if (type == 2)
		{
			// use find to find break points at | and add them aspoints to go to, fucking slider velocity
		}
		//cout << '(' << x << ',' << y << ')' << "deltaT" << deltaT << endl;
		
		fileData.pop_front();
	}

	cin.ignore();
	return 0;
}

/*

JOINTMOTION start = { 0 };
start.Pos.Coord.x = 452.430;
start.Pos.Coord.y = -2.233;
start.Pos.Coord.z = -0.828;
start.Pos.R.x = -179.5431;
start.Pos.R.y = -0.8233;
start.Pos.R.z = -2.0924;
start.MotionSpeedPerc = 30;

*/


JOINMOTION xyCord(double x, double y) 
{
	// myRobot.rposc(0, false).Coord.z
	JOINMOTION re = { 0 };
	re.Pos.Coord.x = 452.430+x;
	re.Pos.Coord.y = -2.233+y;
	re.Pos.Coord.z = -0.828;
	re.Pos.R.x = -179.5431;
	re.Pos.R.y = -0.8233;
	re.Pos.R.z = -2.0924;
	re.MotionSpeedPerc = 50;
	return re;
}