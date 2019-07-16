// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz

// Multithreaded Pseudo Code
// read in structure file,
// after second read calculation can start on a seperate thread
// how to pass relevant lists to threads?
// write can happen multithreaded as OS will keep this thread safe.

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
// #include "sem.h"

using namespace std;

// semaphore threadSetup;

template <class T>
struct Node {
	T data;
	Node *next;
};

template <class T>
class List {
    private:
	 Node<T> *front, *current;

    public:
	 List();
	 ~List();
	 void AddtoFront(T newthing);

	 bool FirstItem(T &item);
	 bool NextItem(T &item);

     bool FirstItem( Node<T> *thisPtr, T &item );
     bool thisItem( Node<T> *thisPtr, T &item );
     bool nextPointer( Node<T> *thisPtr, Node<T> *next );

};

template <class T>
List<T>::List() {
	front = NULL;
	current = NULL;
}

template <class T>
List<T>::~List() {
	if (front != NULL) {
		delete front;
	}
}

template <class T>
void List<T>::AddtoFront(T newthing) {
	Node<T> *temp;
	temp = new Node<T>;
	temp->data = newthing;
	temp->next = front;
	front = temp;
}

template <class T>
bool List<T>::FirstItem(T &item) {
	if (front == NULL) {
		return false;
	}
	item = front->data;
	current = front;
	return true;
}

template <class T>
bool List<T>::NextItem(T &item) {
	current = current->next;
	if (current == NULL) {
		return false;
	}
	item = current->data;
	return true;
}

template <class T>
bool List<T>::thisItem(Node<T> * thisPtr, T &item){
    if (thisPtr == NULL) return false;
    item = thisPtr -> data;
    return true;
}

template <class T>
bool List<T>::nextPointer(Node<T> * thisPtr, Node<T> *next){
    if(thisPtr == NULL) return false;
    next = thisPtr -> next;
    return true;
}

template <class T>
class Sample {
   private:
	string samplename;
	class List<T> alleolelist[2];

   public:
	Sample(){};
	~Sample(){};
	void MakeTitle(string s);
	void CopyName(string *copyname);
	void AddToAllele(int i, T newvalue);
	bool firstOnQueue(int i, T &value);
	bool NextOnQueue(int i, T &value);

    bool firstOnQueue(int i, Node<T> * returnThis, T &value);
	bool NextOnQueue(int i, Node<T> * afterThis, Node<T> * returnThis, T &value);

};

template <class T>
void Sample<T>::MakeTitle(string s) {
	samplename = s;
}

template <class T>
void Sample<T>::CopyName(string *copyname) {
	*copyname = samplename;
}

template <class T>
void Sample<T>::AddToAllele(int i, T newvalue) {
	alleolelist[i].AddtoFront(newvalue);
}

template <class T>
bool Sample<T>::firstOnQueue(int i, T &value) {
	bool ok = alleolelist[i].FirstItem(value);
	return ok;
}

template <class T>
bool Sample<T>::NextOnQueue(int i, T &value) {
	bool ok = alleolelist[i].NextItem(value);
	return ok;
}

template <class T>
bool Sample<T>::firstOnQueue(int i, Node<T> *returnThis, T &value){
    return alleolelist[i].FirstItem(returnThis, value);
}

template <class T>
bool Sample<T>::NextOnQueue(int i, Node<T> *afterThis, Node<T> *returnThis, T &value){
    bool ok = alleolelist[i].nextPointer(afterThis, returnThis);
    if (ok) {
        ok = alleolelist[i].thisItem(returnThis, value);
    }
    return ok;
}

struct ReadInSamples {
    Sample<int> *ReadInThisSample;
    istringstream ReadInLine[2];
    char *delim;
};

struct snipItStruct{
    int *lowindex, *highindex;
    Sample<int> *workOnMe[2];
    string output;
};

void *ReadInSampleFunc(void *sampleReadIn) {
	ReadInSamples *readMeIn = (ReadInSamples *)sampleReadIn;            // cast sampleReadIn from a void * back to a ReadInSamples
    string token;
	for (int i = 0; i < 2; ++i) {
		getline( readMeIn -> ReadInLine[i], token, *(readMeIn -> delim));  // get first word of line, sample title
		if (i % 2 == 0) {
			readMeIn -> ReadInThisSample -> MakeTitle(token);               // if its the first line adf this to the sample title, if not ignore it

			// string temp1;                        // Debugging to see that names ae loading correctly
			// AllSamples[idiv2].CopyName(&temp1);
			// std::cout << temp1 << endl;
		}
		while ( getline( readMeIn -> ReadInLine[i], token, *(readMeIn -> delim) ) ) {    // Tokenise the rest of the data
			// cout << token << endl; // for debugging
			if (token[0] == '-' || isdigit(token[0])) {                         // make sure its a valid number
				int mynumber = stoi(token);                                     // string to integer it
				readMeIn->ReadInThisSample->AddToAllele(i, mynumber);           // to add to the correct array of allele queues

        //         // int temp;
        //         // AllSamples[idiv2]->firstOnQueue((i%2), temp);   // Debugging code
        //         // std::cout << temp << endl;

            }
		}
	}
    delete readMeIn;
    return NULL;
}

void sortSnips(int value[2][2]) {
	int temp;
	if (value[0][0] > value[0][1]) {
		temp = value[0][0];
		value[0][0] = value[0][1];
		value[0][1] = temp;
	}
	if (value[1][0] > value[1][1]) {
		temp = value[1][0];
		value[1][0] = value[1][1];
		value[1][1] = temp;
	}
}

//cases

int CheckTotal(int value[2][2], int low, int high) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (value[i][j] < low || value[i][j] > high) {
				return 0;
			}
		}
	}
	return 1;
}

//matches 01 01 or 01 10 or 11 11

int CheckMisMatch(int value[2][2]) {
	if (value[0][0] == value[1][0] &&
		value[0][1] == value[1][1]) {
		return 0;
	} else {
		return 1;
	}
}

//hom hom the same but different 11 22

int HomPair(int value1[2]) {
	if (value1[0] == value1[1]) {
		return 1;
	}
	return 0;
}

int CheckHomMisMatch(int value[2][2]) {
	if (HomPair(value[0]) && HomPair(value[1]) && (value[0][0] != value[1][0])) {
		return 1;
	} else {
		return 0;
	}
}

//het hom overlapping one is hom one of the hets is the same value as the hom 11 12

int CheckHetHomOverlapMismatch(int value[2][2]) {
	if ((HomPair(value[0]) &&
		 (value[0][0] == value[1][0] || value[0][0] == value[1][1])) ||
		(HomPair(value[1]) &&
		 (value[1][0] == value[0][0] || value[1][0] == value[0][1]))) {
		return 1;
	}
	return 0;
}

//het hom non overlapping 11 23

int CheckHetHomNonOverlapMismatch(int value[2][2]) {
	if ((HomPair(value[0]) &&
		 (value[0][0] != value[1][0] || value[0][0] != value[1][1])) ||
		(HomPair(value[1]) &&
		 (value[1][0] != value[0][0] || value[1][0] != value[0][1]))) {
		return 1;
	}
	return 0;
}

//het het 0112  or 0123

int CheckHetHetMisMatch(int value[2][2]) {
	if (value[0][0] != value[0][1] ||
		value[1][0] != value[1][1]) {
		return 1;
	} else {
		return 0;
	}
}

void * snipAndString(void * cutMeUp){
    snipItStruct *sampleSnip = (snipItStruct *)cutMeUp;

	int TotalSnps, mismatchedSnps, mismatchBothHoms, mismatchHetHomOverlap, mismatchHetHomNonOverlap, mismatchHetHet;
	float mismatchedSnpsPerc, mismatchBothHomsPerc, mismatchHetHomOverlapPerc, mismatchHetHomNonOverlapPerc, mismatchHetHetPerc;

	TotalSnps = 0;
	mismatchedSnps = 0;
	mismatchBothHoms = 0;
	mismatchHetHomOverlap = 0;
	mismatchHetHomNonOverlap = 0;
	mismatchHetHet = 0;

	int value[2][2];  // Load an array with the sample values for comparison
	bool ok1, ok2, ok3, ok4;


    Node<int> *current[4], *next[4];

    // cout << thread[0] << thread[1] << thread[2] << thread[3] << endl;

	ok1 = sampleSnip -> workOnMe[0] -> firstOnQueue(0, next[0], value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
	ok2 = sampleSnip -> workOnMe[0] -> firstOnQueue(1, next[1], value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
	ok3 = sampleSnip -> workOnMe[1] -> firstOnQueue(0, next[2], value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
	ok4 = sampleSnip -> workOnMe[1] -> firstOnQueue(1, next[3], value[1][1]);  // j is the second sample name to compare, 1 is the 2nd allele from that sample name

	while (ok1 && ok2 && ok3 && ok4) {
		sortSnips(value);

		if (CheckTotal(value, *sampleSnip -> lowindex, *sampleSnip -> highindex)) {
			TotalSnps++;
			if (CheckMisMatch(value)) {
				mismatchedSnps++;
				if (CheckHomMisMatch(value)) {
					mismatchBothHoms++;
				} else if (CheckHetHomOverlapMismatch(value)) {
					mismatchHetHomOverlap++;
				} else if (CheckHetHomNonOverlapMismatch(value)) {
					mismatchHetHomNonOverlap++;
				} else if (CheckHetHetMisMatch(value)) {
					mismatchHetHet++;
				}
			}
		}

        for( int z = 0; z < 4; ++z){
            current[z] = next[z];
        }

		// std::cout << value[0][0] << value[0][1] << value[1][0] << value[1][1] << endl; // for debugging
		ok1 = sampleSnip -> workOnMe[0] -> NextOnQueue(0, current[0], next[0], value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
		ok2 = sampleSnip -> workOnMe[0] -> NextOnQueue(1, current[1], next[1], value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
		ok3 = sampleSnip -> workOnMe[1] -> NextOnQueue(0, current[2], next[2],  value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
		ok4 = sampleSnip -> workOnMe[1] -> NextOnQueue(1, current[3], next[3],  value[1][1]);
	}

	mismatchedSnpsPerc = static_cast<float>(mismatchedSnps) / static_cast<float>(TotalSnps) * 100.0;
	mismatchBothHomsPerc = static_cast<float>(mismatchBothHoms) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHomOverlapPerc = static_cast<float>(mismatchHetHomOverlap) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHomNonOverlapPerc = static_cast<float>(mismatchHetHomNonOverlap) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHetPerc = static_cast<float>(mismatchHetHet) / static_cast<float>(TotalSnps) * 100.0;

    // stringstream os;
	// os << std::setprecision(2) << std::fixed;

	string sample1, sample2, SnpsPerc, BothHomsPerc, HomLapPerc, HetHomNonLapPerc, HetHetPerc;

	sampleSnip -> workOnMe[0] -> CopyName(&sample1);
	sampleSnip -> workOnMe[1] -> CopyName(&sample2);

	stringstream temp22;

	temp22 << std::setprecision(2) << std::fixed << mismatchedSnpsPerc << "%";
	SnpsPerc = temp22.str();
	temp22.str("");

	temp22 << std::setprecision(2) << std::fixed << mismatchBothHomsPerc << "%";
	BothHomsPerc = temp22.str();
	temp22.str("");

	temp22 << std::setprecision(2) << std::fixed << mismatchHetHomOverlapPerc << "%";
	HomLapPerc = temp22.str();
	temp22.str("");

	temp22 << std::setprecision(2) << std::fixed << mismatchHetHomNonOverlapPerc << "%";
	HetHomNonLapPerc = temp22.str();
	temp22.str("");

	temp22 << std::setprecision(2) << std::fixed << mismatchHetHetPerc << "%";
	HetHetPerc = temp22.str();
	temp22.str("");

	temp22 << sample1 << "," << sample2 << "," << TotalSnps
				<< "," << mismatchedSnps << "," << SnpsPerc << "," << mismatchBothHoms << ","
				<< BothHomsPerc << "," << mismatchHetHomOverlap << "," << HomLapPerc << ","
				<< mismatchHetHomNonOverlap << "," << HetHomNonLapPerc << ","
				<< mismatchHetHet << "," << HetHetPerc << endl;

    sampleSnip -> output = temp22.str();
    // sampleSnip -> output = "test";
    return NULL;
}

// -------------------------- v Main Starts Here v --------------------- //

int main(int argc, char **argv) {  //get arguments from command line, i.e., yourexec filename
	int i, j, SampleCount, ChromosoneCount;
	vector<Sample<int> *> AllSamples;
    // semaphore_create(&threadSetup, 1);

	cout << "Welcome to SnipIt" << endl
		 << endl;
	cout << "Loading File" << endl           // Move this to when the file actually opens
		 << endl;

// ***************************************************** Change this to command line arguments or optionally enter if not here, if delim is a tab or space then right t or s

	int lowindex, highindex;
	char delim;

	cout << "Enter the lowest allele index number used (ex 0 1 2 3 with a -9 for missing data then enter 0)" << endl;
	cin >> lowindex;
	highindex = lowindex + 3;
	cout << "So the highest allele index number is " << highindex << endl;

	cout << "Enter the character used for delimiting the data" << endl
		 << "(it won't show on the screen if it's a tab or space but trust me I know what you mean)" << endl;
	cin.ignore();
	delim = cin.get();

	cout << "Thank you, if the output file is all 0's or " << endl
		 << "the program errors try a different delim character" << endl;

// ******************************************************* End of command line argument change

	ifstream input_file;
	if (argc < 2) {
		std::cout << "needs a filename as argument  " << endl;
		exit(0);
	}
	input_file.open(argv[1]);
	if (!input_file.good()) {
		std::cout << "cannot read file " << argv[1] << endl;
		exit(0);
	}

	i = 0;
	ChromosoneCount = 0;
	string row;
    pthread_t tempTid;
    vector<pthread_t> tid;
    vector<ReadInSamples *> loadme;
	while (!input_file.eof()) {
		while (getline(input_file, row)) {

            loadme.push_back(new ReadInSamples);

            AllSamples.push_back(new Sample<int>);
            loadme[i] -> ReadInThisSample = AllSamples[i];
            loadme[i] -> delim = &delim;

			loadme[i] -> ReadInLine[0].str(row);
            getline(input_file, row);
            loadme[i] -> ReadInLine[1].str(row);

            pthread_create( &tempTid, NULL, ReadInSampleFunc, loadme[i] );
            tid.push_back(tempTid);

			i++;
		}
	}

	input_file.close();

	SampleCount = i;
	// int totalTest = (SampleCount * (SampleCount - 1)) / 2;

    // ----------      join here

    for( i=0; i < tid.size(); ++i){
        pthread_join( tid[i], NULL);                // Wait for any loading threads still waiting to complete before continuing
    }
    cout << "file read in" << endl;
    // Write compare and write string stream here

	tid.resize(0);
    vector<snipItStruct *> twoSamples;
    i = 0;
	int k = 0;
    while (i < SampleCount - 1) {
		for (j = i + 1; j < SampleCount; ++j) {

            twoSamples.push_back(new snipItStruct);
            twoSamples[k] -> lowindex = &lowindex;
            twoSamples[k] -> highindex = &highindex;
            twoSamples[k] -> workOnMe[0] = AllSamples[i];
            twoSamples[k] -> workOnMe[1] = AllSamples[j];

            pthread_create( &tempTid, NULL, snipAndString, twoSamples[k] );
            tid.push_back(tempTid);
            ++k;
            // pthread_join(tempTid, NULL);
        }
        // cout << "Do I make it here" << endl;
        ++i;
    }

    // write stirng stream to file here
    argv[1][strlen(argv[1])-4] = '\0';
	ofstream output_file(strcat(argv[1], "Snipped.csv"));

	if (output_file.is_open()) {
		output_file << "sample1,sample2,total SNPs,mismatched SNPs,mismatch %,Hom Hom (ex: 00 11),Hom Homs %,";
		output_file << "Het Hom Lap (ex: 00 01),Het Hom Lap %,Het Hom non Lap (ex 00 12),Het Hom non Lap %,Het Het (ex: 01 12 or 01 23),Het Het %\n";

		std::cout << "File is loaded, output file is being written to: " << endl
				  << argv[1] << endl
				  << endl;
    // Join here before writing to file

        for( i=0; i < tid.size(); ++i){
            pthread_join( tid[i], NULL);                // Wait for any loading threads still waiting to complete before continuing
            output_file << twoSamples[i] -> output;
        }

	output_file.close();

        // for ( i=0; i < k; ++i){
        //     delete twoSamples[i];
        // }

	}
	// for (vector<Sample *>::iterator it = AllSamples.begin(); it != AllSamples.end(); it++) {
	// 	delete (*it);
	// }

	std::cout << endl
			  << endl
			  << "Processing Complete, you may now use " << argv[1] << endl
			  << endl
			  << "Thanks for using SnipIt feel free to donate " << endl
			  << "or request other features or programs at www.SierraAlpha.co.nz" << endl
			  << endl;
}

//cases
//matches 01 01 or 01 10 or 11 11
//hom hom the same but different 11 22
//het hom overlapping one is hom one of the hets is the same value as the hom 11 12
//het hom non overlapping 11 23
//het het 0112  or 0123

// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz