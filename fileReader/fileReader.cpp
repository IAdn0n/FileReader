#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;

const int FILES_SIZE = 40;
unsigned long long wordsFound = 0;
int filesRead = 1;

mutex printingMutex;
mutex fileCounterMutex;

//function for printing
void print(string& target, string& fileName, int lineNumber, int columnNumber) {
    cout << "Found \"" << target <<"\" at File: \"" << fileName << "\" Line #" << lineNumber << ", " << columnNumber << ".\n";
}

//function to check end of the word constraints
bool endOfWord(char c) {
    if (c == ' ' || c == '?' || c == '.' || c == ',' || c == '!' || c == '-' || c == '\"')
        return true;
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == ':' || c == ';';

}

//find the specific word in string of words
void findWord(string& line, string& fileName, string& target, int lineNumber) {
    int startIndex = 0, columnNumber = 1;
    for (int i = 0; i < line.length(); i++) {

        //if we can make a word;
        if (endOfWord(line[i]) || i == line.length() - 1) {
            if (i == line.length() - 1) i++;
            string word = line.substr(startIndex, i - startIndex);
            startIndex = i + 1;

            //if we find the word
            if (word._Equal(target)) {
                //lock cout; 

                printingMutex.lock();
                print(target, fileName, lineNumber, columnNumber);
                printingMutex.unlock();
               
                wordsFound++;
                wordsFound %= 922337203685477580;
            }
            
            columnNumber++;
        }
    }
}

void readFromFile(string& fileName, string& target) {
    ifstream file(fileName);

    //couldnt open a file 
    if (!file.is_open()) {
        cout << "Unable to open file";
        return;
    }
    string line;
    int lineNumber = 1;
    while (getline(file, line)) {
        findWord(line, fileName, target, lineNumber);
        lineNumber++;
    }

    file.close();
}

void workingThread(string target) {
    while (true) {
        //we lock it so no two threads read from the same file
        fileCounterMutex.lock();

        //if all files have been read
        if (filesRead > FILES_SIZE) {
            fileCounterMutex.unlock();
            break;
        }
        string fileName = "../file" + to_string(filesRead) + ".txt";
        filesRead++;

        fileCounterMutex.unlock();
        //unlock

        readFromFile(fileName, target);
    }
}

int main() {
    string target;
    int threadSize;
    cout << "Enter String to Find: ";
    cin >> target;
    cout << "Enter Number of Threads: ";
    cin >> threadSize;

    //input constraints
    threadSize = min(FILES_SIZE, threadSize);
    threadSize = max(1, threadSize);

    thread* thds = new thread[threadSize];
   
    //start timer
    const clock_t begin_time = clock();

    for (int i = 0; i < threadSize; i++) 
        thds[i] = thread(workingThread, target);

    //join all threads;
    for (int i = 0; i < threadSize; i++)
        thds[i].join();

    delete [] thds;

    float time = float(clock() - begin_time) / CLOCKS_PER_SEC;
    cout << "Found Total of " << wordsFound << " Words\n";
    cout << "Time: " << time << " Seconds\n";
    return 0;
}
