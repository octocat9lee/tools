#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef void (*MallocFunc)(const int, const int);

void MallocMemoryInMap(const int size, const int blockSize)
{
    cout << "MallocMemoryInMap" << endl;
    std::map<int, char*> memoryMap;
    int blockSizeKb = blockSize * 1024;
    for(int i = 0; i < size / blockSize; ++i)
    {
        char *p = new char[blockSizeKb];
        char v = rand() % 128;
        memset(p, v, blockSizeKb);
        memoryMap.insert(make_pair(i, p));
    }
    sleep(5);
    std::map<int, char*>::iterator iter = memoryMap.begin();
    for(; iter != memoryMap.end(); ++iter)
    {
        delete [] iter->second;
    }
    memoryMap.clear();
    cout << "Free Memory" << endl;
}

void MallocMemoryInMapDelNull(const int size, const int blockSize)
{
    cout << "MallocMemoryInMapDelNull" << endl;
    std::map<int, char*> memoryMap;
    int blockSizeKb = blockSize * 1024;
    for(int i = 0; i < size / blockSize; ++i)
    {
        char *p = new char[blockSizeKb];
        char v = rand() % 128;
        memset(p, v, blockSizeKb);
        //memoryMap.insert(make_pair(i, (char*)NULL));
        memoryMap.insert(make_pair(i, p));
    }
    sleep(5);
    memoryMap.clear();
    std::map<int, char*>::iterator iter = memoryMap.begin();
    for(; iter != memoryMap.end(); ++iter)
    {
        delete [] iter->second;
    }
    cout << "Free Memory" << endl;
}

void MallocMemoryInVec(const int size, const int blockSize)
{
    cout << "MallocMemoryInVec" << endl;
    std::vector<char*> memoryVec;
    memoryVec.reserve(size / blockSize);
    int blockSizeKb = blockSize * 1024;
    for(int i = 0; i < size / blockSize; ++i)
    {
        char *p = new char[blockSizeKb];
        char v = rand() % 128;
        memset(p, v, blockSizeKb);
        memoryVec.push_back(p);
    }
    sleep(5);
    std::vector<char*>::iterator iter = memoryVec.begin();
    for(; iter != memoryVec.end(); ++iter)
    {
        delete [] (*iter);
    }
    memoryVec.clear();
    cout << "Free Memory" << endl;
}

void MallocMemoryInArray(const int size, const int blockSize)
{
    cout << "MallocMemoryInArray" << endl;
    char** memoryArray = new char*[size / blockSize];
    int blockSizeKb = blockSize * 1024;
    for(int i = 0; i < size / blockSize; ++i)
    {
        char *p = new char[blockSizeKb];
        char v = rand() % 128;
        memset(p, v, blockSizeKb);
        memoryArray[i] = p;
    }
    sleep(5);
    for(int i = 0; i < size / blockSize; ++i)
    {
        delete [] memoryArray[i];
    }
    delete [] memoryArray;
    cout << "Free Memory" << endl;
}

int main()
{
    int containerType = 0;
    int initSize = 0;
    int randSize = 0;
    int onceSize = 0;
    cout << "Please Input Container Type (0:Map 1:Vector 2:Array 3:MapDelNull) "
        << "Initialize Size[MB], Rand Size[MB] and Once Size [KB]" << endl;
    cin >> containerType >> initSize >> randSize >> onceSize;
    initSize *= 1024;
    srand((unsigned)time(NULL));

    MallocFunc mallocFuncArray[4] = {MallocMemoryInMap,
        MallocMemoryInVec, MallocMemoryInArray, MallocMemoryInMapDelNull};

    while(true)
    {
        cout << "Malloc Size " << initSize / 1024 << " MB" << endl;
        mallocFuncArray[containerType](initSize, onceSize);
        initSize = 1024 * (1 + (rand() % randSize));
        sleep(10);
    }
    return 0;
}
