#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <robotarm.h>
#include <unistd.h>
#include <robotarm.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <list>
#include <chrono>

robotarm myRobot("192.168.100.10",80,"thisTraceFile.log", "thisCommLog.log");// DLR robot APIo
int WIDTH = 105;
int HEIGHT = 65;
using namespace std;
using namespace std::chrono;
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

int stringAsInt(string number)
{
    int re = 0;
    for (unsigned int i = 0; i<number.length() ; i++)
    {
        re += number[i] - 48;
        re *= 10;
    }
    return re / 10;
}

int roundDoubleNDigits(double num, char digit)
{
    return round( pow(10,digit) * num ) / pow(10,digit);
}

string* split(string in, const char devider)
{
    int counter = 0;
    for (unsigned int i = 0; i < in.length(); i++)
        if (in[i] == devider)
            counter++;

    string* data = new string[counter + 1];

    string temp = "";
    int j = 0;
    for (unsigned int i = 0; i < in.length(); i++)
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


JOINTMOTION xyCord(double x, double y)
{
    // magic constants 37.756 = 250 on y axis
    // myRobot.rposc(0, false).Coord.z
    JOINTMOTION re = { 0 };
    re.Pos.Coord.x = 795-y; // 6.5 (730 795)
    re.Pos.Coord.y = 70-x; // 105 (-35 to 70)
    re.Pos.Coord.z = -378.2;
    re.Pos.R.x = -180;
    re.Pos.R.y = 0;
    re.Pos.R.z = 0;
    re.MotionSpeedPerc = 1  ;
    return re;
}

void waitPos()
{
    RPOSC r = myRobot.rposc(0);

    double ox = r.Coord.x;
    double oy = r.Coord.y;
    cout << ox << "," << oy << endl;
    while(true)
    {
        r = myRobot.rposc(0);
        if(ox-r.Coord.x == 0 && oy-r.Coord.y == 0)
        {
            cout << ox << "," << oy << endl;
            break;
        }

        ox = r.Coord.x;
        oy = r.Coord.y;

    }

}

void move(JOINTMOTION pos, int flag, int deltaT)
{
    auto t1 = system_clock::now();

    myRobot.movj(pos);
    if(flag)
        waitPos();

    if (deltaT >= 200)
    {
        deltaT -= 200;
        auto t2 = system_clock::now();
        duration<double> dt = (t2-t1);
        // cout << duration<double>(dt).count();
        if(dt.count()*1000 > deltaT)
            return;
        while(dt.count()*1000 <= deltaT)
        {
            t2 = system_clock::now();
            dt = t2-t1;
        }
    }

}

int main()
{
    //Connect to robot
    myRobot.Connect(true);  //true - llong conn, false - onetime
    myRobot.svon(true);

    move(xyCord(50,50),true, 0);
    move(xyCord(50,300),true, 0);
    move(xyCord(50,50),true, 0);
    move(xyCord(50,300),true, 0);
    move(xyCord(50,50),true, 0);
    move(xyCord(50,300),true, 0);

    string line = "";
    ifstream myfile;


    char result[ 23 ];
    readlink( "/proc/self/exe", result, 23 );
    string path = result;
    path += "osuRob/new.txt";

    myfile.open(path);


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

    int** dataArr = new int*[10000];
    for(int i = 0; i < 10000; i++) {
        dataArr[i] = new int[3];
    }
    int deltaT;
    string* pdata;
    double x = 0, y = 0;
    int timing = 0, oldTiming = 19390;
    string type;
    i = 0;
    while(!fileData.empty())
    {
        pdata = fileData.front();
        x = roundDoubleNDigits(stringAsInt(pdata[0]), 2)*(WIDTH) / 512.0;
        y = roundDoubleNDigits(stringAsInt(pdata[1]), 2)*HEIGHT / 512.0;
        timing = stringAsInt(pdata[2]);
        type = binaryToString(toBinary(stringAsInt(pdata[3])));
        deltaT = timing - oldTiming;
        oldTiming = timing;
        dataArr[i][0] = x;
        dataArr[i][1] = y;
        dataArr[i][2] = deltaT;
        i++;

        //cout << deltaT << endl;
        move(xyCord(x,y),true, deltaT);
        fileData.pop_front();
    }

    myRobot.svon(false);
    myRobot.Disconnect();
    cin.ignore();
}

