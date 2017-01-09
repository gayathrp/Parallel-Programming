#include "MyPlace.h"
#include "MASS_base.h"
#include <sstream>     // ostringstream
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <list>

using namespace std;

//Used to toggle output for Wave2d
//const bool printOutput = false;
const bool printOutput = true;

extern "C" Place* instantiate( void *argument ) {
    return new MyPlace( argument );
}

extern "C" void destroy( Place *object ) {
    delete object;
}


/**
 * Initializes a Land object.
// */
void *MyPlace::init( void *argument ) {

    return NULL;
}

double finalMin = 0.0;

bool has_suffix(const string& s, const string& suffix)
{
    return (s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

double findMin(const string& file){
    string line;
    ifstream myfile (file);
    int lineNumber = 0;
    double min[10];
    int indexMin = 0;
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            lineNumber++;
            if (lineNumber > 7 && lineNumber < 16) {
                string delimiter = ",";
                size_t pos = 0;
                int count = 0;
                while ((pos = line.find(delimiter)) != string::npos && count > 1 ||count < 3) {
                    if (count == 2) {
                        min[indexMin] = stod(line.substr(0, pos));
                        indexMin++;
                    }
                    line.erase(0, pos + delimiter.length());
                    count++;
                }
            }
        }
//    Find Min from array
        double mn = min[0];  
        int x;
        for(x = 1 ; x < 8 ; x++) { 
            if(min[x] == 0) continue;
            if(mn > min[x])  { 
                mn = min[x]; 
            } 
        }  
        myfile.close();
        return mn;
    }
    return 0;
}

double listFiles(const string& addDir) {
    list<string> mylist;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (addDir.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if(has_suffix(ent->d_name, ".csv")) {
                mylist.push_back(ent->d_name);
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
    for (auto v : mylist) {
        cout << v << endl;
        double min = findMin(addDir+"/"+v);
        cout << min << endl;
        if(finalMin == 0.0)
            finalMin = min;
        else if(min < finalMin)
            finalMin = min;
    }
    return finalMin;
}

void *MyPlace::findMinTemperature(void *argument){
    double min = listFiles("/CSSDIV/students/css534/gayathrp/MASS-Cpp/ubuntu/climate/NCDCDataSets");
    cout << "Final Min " << min << endl;
}
