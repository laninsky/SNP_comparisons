// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz

#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ---------------- The Node for the list ----------------
// The node needs to be available from main so that each thread
// can traverse the list independently of other threads

struct Node {
	int data;
	Node *next;
};

// ---------------- The list ----------------
// The list needed to have special methods for the thread independent
// traversal of the list to do the multi threaded comparisons

class List {
   private:
	Node *front, *current;

   public:
	List();
	~List();
	void AddtoFront(int newthing);  // Adds a new node with 'newthing' data to the begining of the list (faster to add it to the front)

	bool FirstItem(int &item);  // Set 'item' to the first Node 'data' and set the member 'current' to equal 'front'
	bool NextItem(int &item);   // Set 'item' to the Node 'data' that 'current' is pointing to and move member 'current' to the next Node

	bool FirstItem(Node **returnThis, int &item);  // Set 'returnThis' to the first Node ('front'), set 'item' to the first Node 'data'
	bool thisItem(Node *thisPtr, int &item);	   // Set 'item' to the Node 'data' that 'thisPtr' is pointing too
	bool nextPointer(Node *thisPtr, Node **next);  // Make 'next' pointer point to the Node after 'thisPtr'
};

List::List() {
	front = NULL;
	current = NULL;
}

List::~List() {
	while (front != NULL) {
		Node *temp = front;
		front = front->next;
		delete temp;
	}
	current = NULL;
}

void List::AddtoFront(int newthing) {
	Node *temp;
	temp = new Node;
	temp->data = newthing;
	temp->next = front;
	front = temp;
}

bool List::FirstItem(int &item) {
	if (front == NULL) {
		return false;
	}
	item = front->data;
	current = front;
	return true;
}

bool List::NextItem(int &item) {
	current = current->next;
	if (current == NULL) {
		return false;
	}
	item = current->data;
	return true;
}

bool List::FirstItem(Node **returnThis, int &item) {
	if (front == NULL) return false;
	*returnThis = front;
	item = front->data;
	return true;
}

bool List::thisItem(Node *thisPtr, int &item) {
	if (thisPtr == NULL) return false;
	item = thisPtr->data;
	return true;
}

bool List::nextPointer(Node *thisPtr, Node **next) {
	if (thisPtr == NULL) return false;
	*next = thisPtr->next;
	return true;
}

// ---------------- Sample class ----------------

class Sample {
   private:
	string samplename;			// Name of the sample
	class List alleolelist[2];  // A two index array of 'List' of all the ACGT value pairs per allele (locii? bioligist to confirm naming) for a sample

   public:
	Sample(){};
	~Sample(){};
	void MakeTitle(const string &s);		// Set the 'samplename' to the string 's'
	void CopyName(string *copyname);		// Copy the 'samplename' to the string passed into the function ('copyname')
	void AddToAllele(int i, int newvalue);  // Add 'newValue' to the 'alleolelist' specified by the int 'i'
	bool firstOnQueue(int i, int &value);   // Set 'value' to the Node 'data' that is at the List 'front' of the queue specified by 'i'
	bool NextOnQueue(int i, int &value);	// Set 'value' to the Node 'data' that is at the List 'current' specified by 'i'

	bool firstOnQueue(int i, Node **returnThis, int &value);				  // Overload, difference is that this changes 'returnThis' to point to the List 'front' Node (needed for multithreaded traversal of a List)
	bool NextOnQueue(int i, Node *afterThis, Node **returnThis, int &value);  // Overload, difference is that takes a Node *'afterThis' and changes 'returnThis' to point to the next Node. 'value' becomes the Node data that 'returnThis' points to. (needed for multithreaded traversal of a List)
};

void Sample::MakeTitle(const string &s) {
	samplename = s;
}

void Sample::CopyName(string *copyname) {
	*copyname = samplename;
}

void Sample::AddToAllele(int i, int newvalue) {
	alleolelist[i].AddtoFront(newvalue);
}

bool Sample::firstOnQueue(int i, int &value) {
	bool ok = alleolelist[i].FirstItem(value);
	return ok;
}

bool Sample::NextOnQueue(int i, int &value) {
	bool ok = alleolelist[i].NextItem(value);
	return ok;
}

bool Sample::firstOnQueue(int i, Node **returnThis, int &value) {
	return alleolelist[i].FirstItem(returnThis, value);
}

bool Sample::NextOnQueue(int i, Node *afterThis, Node **returnThis, int &value) {
	bool ok = alleolelist[i].nextPointer(afterThis, returnThis);
	if (ok) {
		ok = alleolelist[i].thisItem(*returnThis, value);
	}
	return ok;
}

// ---------------- Structure for passing data to read-in thread ----------------

struct ReadInSamples {
	Sample *ReadInThisSample;	 // A pointer to the place in the 'allSamples' vector in main
	istringstream ReadInLine[2];  // An array of two items corresponding to the two lines in the file for a given sample
	char *delim;				  // The delim character to tokenise the 'ReadInLine'
};

// ---------------- Read-in thread function ----------------

void *ReadInSampleFunc(void *sampleReadIn) {
	ReadInSamples *readMeIn = (ReadInSamples *)sampleReadIn;		  // cast sampleReadIn from a void * back to a ReadInSamples
	string token;													  // For breaking up the string
	for (int i = 0; i < 2; ++i) {									  // Go through the two lines of text
		getline(readMeIn->ReadInLine[i], token, *(readMeIn->delim));  // get first word of line, sample title
		if (i % 2 == 0) {											  // If it is the first line
			readMeIn->ReadInThisSample->MakeTitle(token);			  // then add this to the sample title, if not ignore it
		}
		while (getline(readMeIn->ReadInLine[i], token, *(readMeIn->delim))) {  // while there is still part of the line to tokenise then Tokenise the rest of the data
			if (token[0] == '-' || isdigit(token[0])) {						   // make sure its a valid number
				int mynumber = stoi(token);									   // string to integer it
				readMeIn->ReadInThisSample->AddToAllele(i, mynumber);		   // to add to the correct array of allele queues
			}
		}
	}
	delete readMeIn;  // memory management, dont need heap allocation for 'readMeIn' anymore
	return NULL;	  // need to return nothing
}

// ---------------- function to sort snips for comparison ----------------
// Only matters for the 01 10 match so it would become 01 01

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

// ---------------- Function to count total number of samples for percentage calculations ----------------

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

// ------- Cases ----------------------------------------------

// This is where the Magic happens, these are all the functions to compare the samples live

// cases
// matches 01 01 or 01 10 or 11 11
// hom hom the same but different 11 22
// het hom overlapping one is hom one of the hets is the same value as the hom 11 12
// het hom non overlapping 11 23
// het het 0112  or 0123

//matches 01 01 or 01 10 or 11 11

int CheckMisMatch(int value[2][2]) {
	if (value[0][0] == value[1][0] &&
		value[0][1] == value[1][1]) {
		return 0;
	} else {
		return 1;
	}
}

//hom hom the same but different 11 22 (pt 1)

int HomPair(int value1[2]) {
	if (value1[0] == value1[1]) {
		return 1;
	}
	return 0;
}

//hom hom the same but different 11 22 (pt 2)

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

// ---------------- Structure for passing data to comparison thread function ----------------

struct snipItStruct {
	int *lowindex, *highindex;  // Pass through the allele integer representation range
	Sample *workOnMe[2];		// A 2 index array of pointers to the two 'Sample's to compare
	string output;				// The 'output' string to write to file
};

// ---------------- This is the compare function for passing to threads to do the calculations simultaniously ----------------

void *snipAndString(void *cutMeUp) {
	snipItStruct *sampleSnip = (snipItStruct *)cutMeUp;  // Casting from the thread void * back to a snipItStruct

	int TotalSnps, mismatchedSnps, mismatchBothHoms, mismatchHetHomOverlap, mismatchHetHomNonOverlap, mismatchHetHet;			  // for counts
	float mismatchedSnpsPerc, mismatchBothHomsPerc, mismatchHetHomOverlapPerc, mismatchHetHomNonOverlapPerc, mismatchHetHetPerc;  // for percentages

	TotalSnps = 0;
	mismatchedSnps = 0;
	mismatchBothHoms = 0;
	mismatchHetHomOverlap = 0;
	mismatchHetHomNonOverlap = 0;
	mismatchHetHet = 0;

	int value[2][2];		  // Load an array with the sample values for comparison
	bool ok1, ok2, ok3, ok4;  // if false is returned then an end of the list has been reached

	Node *current[4], *next[4];  // pointers for independent thread traversal of the lists

	ok1 = sampleSnip->workOnMe[0]->firstOnQueue(0, &next[0], value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
	ok2 = sampleSnip->workOnMe[0]->firstOnQueue(1, &next[1], value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
	ok3 = sampleSnip->workOnMe[1]->firstOnQueue(0, &next[2], value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
	ok4 = sampleSnip->workOnMe[1]->firstOnQueue(1, &next[3], value[1][1]);  // j is the second sample name to compare, 1 is the 2nd allele from that sample name

	while (ok1 && ok2 && ok3 && ok4) {  // while there is still some of the list to compare
		sortSnips(value);				// sort the snips

		if (CheckTotal(value, *sampleSnip->lowindex, *sampleSnip->highindex)) {  // if value is within the allele integer index range
			TotalSnps++;														 // increment 'totalSnps'
			if (CheckMisMatch(value)) {											 // if any mismatch exists
				mismatchedSnps++;												 // increment 'mismatchedSnps'
				if (CheckHomMisMatch(value)) {									 // if Hom Hom mismatch
					mismatchBothHoms++;											 // increment hom hom mismatch count
				} else if (CheckHetHomOverlapMismatch(value)) {					 // or if its a het hom overlapping mismatch
					mismatchHetHomOverlap++;									 // increment the het hom overlap mismatch count
				} else if (CheckHetHomNonOverlapMismatch(value)) {				 // or if its a het hom non overlap mismatch
					mismatchHetHomNonOverlap++;									 // increment the het hom non overlap mismatch count
				} else if (CheckHetHetMisMatch(value)) {						 // or if its a het het mismatch
					mismatchHetHet++;											 // increment the het het mismatch count
				}
			}
		}

		for (int z = 0; z < 4; ++z) {  // for each of the allele lists
			current[z] = next[z];	  // set the current pointer to point to where next is poinitg
		}

		// get the values and pointers for the next place in the list
		ok1 = sampleSnip->workOnMe[0]->NextOnQueue(0, current[0], &next[0], value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
		ok2 = sampleSnip->workOnMe[0]->NextOnQueue(1, current[1], &next[1], value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
		ok3 = sampleSnip->workOnMe[1]->NextOnQueue(0, current[2], &next[2], value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
		ok4 = sampleSnip->workOnMe[1]->NextOnQueue(1, current[3], &next[3], value[1][1]);  // j is the second sample name to compare, 1 is the 2nd allele from that sample name
	}

	// perform the percentage calculations
	mismatchedSnpsPerc = static_cast<float>(mismatchedSnps) / static_cast<float>(TotalSnps) * 100.0;
	mismatchBothHomsPerc = static_cast<float>(mismatchBothHoms) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHomOverlapPerc = static_cast<float>(mismatchHetHomOverlap) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHomNonOverlapPerc = static_cast<float>(mismatchHetHomNonOverlap) / static_cast<float>(TotalSnps) * 100.0;
	mismatchHetHetPerc = static_cast<float>(mismatchHetHet) / static_cast<float>(TotalSnps) * 100.0;

	// set up for output string formatting
	string sample1, sample2, SnpsPerc, BothHomsPerc, HomLapPerc, HetHomNonLapPerc, HetHetPerc;
	stringstream temp22;

	// set the output sample names
	sampleSnip->workOnMe[0]->CopyName(&sample1);
	sampleSnip->workOnMe[1]->CopyName(&sample2);

	// format and string build for mismatched snps percent                          // these comments are the same for the following 4 code blocks
	temp22 << std::setprecision(2) << std::fixed << mismatchedSnpsPerc << "%";  // String build
	SnpsPerc = temp22.str();													// assign to relevant string
	temp22.str("");																// clear temp string for reuse

	// format and string build for mismatched Hom Homs percent
	temp22 << std::setprecision(2) << std::fixed << mismatchBothHomsPerc << "%";
	BothHomsPerc = temp22.str();
	temp22.str("");

	// format and string build for mismatched Het Hom overlaps percent
	temp22 << std::setprecision(2) << std::fixed << mismatchHetHomOverlapPerc << "%";
	HomLapPerc = temp22.str();
	temp22.str("");

	// format and string build for mismatched Het Hom non overlap percent
	temp22 << std::setprecision(2) << std::fixed << mismatchHetHomNonOverlapPerc << "%";
	HetHomNonLapPerc = temp22.str();
	temp22.str("");

	// format and string build for mismatched het het percent
	temp22 << std::setprecision(2) << std::fixed << mismatchHetHetPerc << "%";
	HetHetPerc = temp22.str();
	temp22.str("");

	// building the final outut stringstream
	temp22 << sample1 << "," << sample2 << "," << TotalSnps
		   << "," << mismatchedSnps << "," << SnpsPerc << "," << mismatchBothHoms << ","
		   << BothHomsPerc << "," << mismatchHetHomOverlap << "," << HomLapPerc << ","
		   << mismatchHetHomNonOverlap << "," << HetHomNonLapPerc << ","
		   << mismatchHetHet << "," << HetHetPerc << endl;

	// assigning the final output string to the output in the snipItStruct
	sampleSnip->output = temp22.str();
	return NULL;
}

// ---------------- Help and Exit Function ----------------

// If not enough command line args (or too many) this will be displayed on screen,
// doubles as a help screen if people dont know how to use SnipIt

void helpAndExit() {
	cout << "usage:" << endl
		 << "./SnipIt <input_filename> <lowest_allele> <delim_character>" << endl
		 << endl
		 << "<input_filename>   - The name of the structure file to be read in and snipped." << endl
		 << "<lowest_allele>    - Specify the lowest number used to represent an allele," << endl
		 << "\t\t\tthis program assumes they are represented by 4 consecutive numbers, " << endl
		 << "\t\t\tany number outside this are considered a non sample." << endl
		 << "<delim_characters> - The character used to delimit samples in the stru file," << endl
		 << "\t\t\tuse \\\\t or \\\\s for tab and space respectivly." << endl
		 << endl;

	exit(0);
}

// -------------------------- v Main Starts Here v --------------------- //

int main(int argc, char **argv) {  // get arguments from command line, i.e., yourexec filename
	int i, j, SampleCount;
	vector<Sample *> AllSamples;  // A vector to store pointer to samples in

	if (argc != 4) {  // if not enough command line args, or too many then call help and exit
		helpAndExit();
	}

	// Welcome note
	cout << endl
		 << "Welcome to SnipIt, type 'ctrl + c' to exit" << endl
		 << endl;

	// ******** Load command line args into relevant places, and print out for user to see

	int lowindex, highindex;
	char delim = '\0';

	istringstream(argv[2]) >> lowindex;
	highindex = lowindex + 3;
	cout << "The allele index number range is " << lowindex << " to " << highindex << endl;

	if (strlen(argv[3]) > 1) {
		if (argv[3][0] == '\\') {
			if (argv[3][1] == 's') {
				delim = ' ';
			}
			if (argv[3][1] == 't') {
				delim = '\t';
			}
		}
	} else {
		istringstream(argv[4]) >> delim;
	}

	cout << "The delimiter is \"" << delim << "\"" << endl;

	cout << endl
		 << "If the output file is all 0's or the program errors try a different delim character" << endl
		 << "Remember to use \\\\s or \\\\t in the command line arguments for 'space' or 'tab' keys" << endl
		 << endl;

	// ******** Reading in file

	cout << "Loading File" << endl;

	ifstream input_file;
	input_file.open(argv[1]);
	if (!input_file.good()) {								  // if cant load file
		std::cout << "cannot read file " << argv[1] << endl;  // notify user
		exit(0);											  // exit
	}

	i = 0;
	string row;
	pthread_t tempTid;						// to keep track of threads
	vector<pthread_t> tid;					// vector to keep them all Thread ID (tid)
	vector<ReadInSamples *> loadme;			// Vector of pointers to ReadInSample structures to build and pass to read in thread
	while (!input_file.eof()) {				// while there are lines to read
		while (getline(input_file, row)) {  // while there are lines to read
			loadme.push_back(new ReadInSamples);

			AllSamples.push_back(new Sample);
			loadme[i]->ReadInThisSample = AllSamples[i];  // Assign the sample to be loaded pointer to point to relevant spot in the allsamples vector
			loadme[i]->delim = &delim;					  // pass the delim to the structure to pass to the thread

			loadme[i]->ReadInLine[0].str(row);  // set the first roe into the structure to pass to the read-in thread
			getline(input_file, row);			// read in the next row
			loadme[i]->ReadInLine[1].str(row);  // set the second row into the structure to pass to the read-in thread

			pthread_create(&tempTid, NULL, ReadInSampleFunc, loadme[i]);  // create a thread and pass it the relavant loadme structure
			tid.push_back(tempTid);										  // push the thread ID onto the vector to use later in the join process

			i++;  // increment to keep track of the sample count (two lines per one sample)
		}
	}

	input_file.close();  // finished with the read in file so we can close it
	SampleCount = i;	 // i is the sample count so lets keep it safe somewhere

	for (i = 0; i < tid.size(); ++i) {  // for all the threads we just created
		pthread_join(tid[i], NULL);		// Wait for any loading threads still computing to complete before continuing
	}

	cout << "File loaded" << endl
		 << "Starting comparison" << endl;

	// ******** Comparing samples

	tid.resize(0);						// set thread Id vector to 0 elements
	vector<snipItStruct *> twoSamples;  // vector of pointers to twosample struct to build and pass to thread
	i = 0;
	int k = 0;
	while (i < SampleCount) {							 // while we are going through all the samples
		for (j = i + 1; j < SampleCount; ++j) {			 // compare to all the remaining samples after the one we started with
			twoSamples.push_back(new snipItStruct);		 // make a new struct on the twoSample vector
			twoSamples[k]->lowindex = &lowindex;		 // pass the low allele index integer
			twoSamples[k]->highindex = &highindex;		 // pass the high allele index integer
			twoSamples[k]->workOnMe[0] = AllSamples[i];  // pass the sample that i is pointing to
			twoSamples[k]->workOnMe[1] = AllSamples[j];  // pass the sample that j is pointing to

			pthread_create(&tempTid, NULL, snipAndString, twoSamples[k]);  // create a new thread to compare these two samples
			tid.push_back(tempTid);										   // add the thread Id's to the tid vector for joining later
			++k;														   // increment K, to keep track of which part of the twoSample vector we're accessing
		}
		++i;
	}

	// ******** Writing to file

	char *tempChar;
	tempChar = strrchr(argv[1], '.');					   // find the start of the extension
	*tempChar = '\0';									   // make it the end of the filename
	ofstream output_file(strcat(argv[1], "Snipped.csv"));  // concat Snipped.csv onto this

	if (output_file.is_open()) {  // open the file for writing too (will overwrite whatever exists there already)

		// put the titles in the file
		output_file << "sample1,sample2,total SNPs,mismatched SNPs,mismatch %,Hom Hom (ex: 00 11),Hom Homs %,";
		output_file << "Het Hom Lap (ex: 00 01),Het Hom Lap %,Het Hom non Lap (ex 00 12),Het Hom non Lap %,Het Het (ex: 01 12 or 01 23),Het Het %\n";

		// print to screen that writing to file will start now
		std::cout << "Comparison finishing, writing to file:" << endl
				  << argv[1] << endl;

		for (i = 0; i < tid.size(); ++i) {		   // for every comparison thread we started
			pthread_join(tid[i], NULL);			   // Wait for any comparing threads to complete before writing results to file
			output_file << twoSamples[i]->output;  // write results to file
			delete twoSamples[i];				   // delete what we put on the heap at this vector location
		}

		output_file.close();  // finished writing to file we can close it

		// tell the user they can use it now and do a shameless plug
		std::cout << endl
				  << "Processing Complete, you may now use " << argv[1] << endl
				  << endl
				  << "Thanks for using SnipIt feel free to donate " << endl
				  << "or request other features or programs at www.SierraAlpha.co.nz" << endl
				  << endl;

	} else {  // if file couldnt open then tell the user and exit
		cout << "Unable to open output file for writing too, program will now exit" << endl;
	}

	// ******** Memory clean up
	// for all the stuff we put on the heap lets be tidy kiwis and delete it

	for (vector<Sample *>::iterator it = AllSamples.begin(); it != AllSamples.end(); ++it) {
		delete (*it);
	}
}

// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz