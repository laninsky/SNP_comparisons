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

using namespace std;

template <class T>
class List {
   private:
	struct Node {
		T data;
		Node *next;
	};
	Node *front, *current;

   public:
	List();
	~List();
	void AddtoFront(T newthing);
	bool FirstItem(T &item);
	bool NextItem(T &item);
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
	Node *temp;
	temp = new Node;
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


class Sample {
   private:
	string samplename;
	class List<int> alleolelist[2];

   public:
	Sample(){};
	~Sample(){};
	void MakeTitle(string s);
	void CopyName(string *copyname);
	void AddToAllele(int i, int newvalue);
	bool firstOnQueue(int i, int &value);
	bool NextOnQueue(int i, int &value);
};

void Sample::MakeTitle(string s) {
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

struct ReadInSamples {
    Sample *ReadInThisSample;
    istringstream ReadInLine[2];
    char delim;
};

void *ReadInSampleFunc(void *sampleReadIn) {
	ReadInSamples *readMeIn = (ReadInSamples *)sampleReadIn;
    string token;
	for (int i = 0; i < 2; ++i) {
            cout << "Make it here?" << endl;
		getline( readMeIn -> ReadInLine[i], token, readMeIn -> delim);
		if (i % 2 == 0) {
			readMeIn->ReadInThisSample->MakeTitle(token);

			// string temp1;                        // Debugging to see that names ae loading correctly
			// AllSamples[idiv2].CopyName(&temp1);
			// std::cout << temp1 << endl;
		}
		while (getline (readMeIn->ReadInLine[i], token, readMeIn->delim) ) {
			// cout << token << endl; // for debugging
			if (token[0] == '-' || isdigit(token[0])) {
				int mynumber = stoi(token);
				readMeIn->ReadInThisSample->AddToAllele(i, mynumber);  // to add to the correct array of allele queues

        //         // int temp;
        //         // AllSamples[idiv2]->firstOnQueue((i%2), temp);   // Debugging code
        //         // std::cout << temp << endl;

            }
		}
	}
    delete readMeIn;
    cout << "or here" << endl;
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

// -------------------------- v Main Starts Here v --------------------- //

int main(int argc, char **argv) {  //get arguments from command line, i.e., yourexec filename
	int i, j, SampleCount, ChromosoneCount;
	vector<Sample *> AllSamples;

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
	int idiv2 = 0;
	ChromosoneCount = 0;
	string row;
    pthread_t tempTid;
    vector<pthread_t> tid;
    vector<ReadInSamples *> loadme;
	while (!input_file.eof()) {
		while (getline(input_file, row)) {

            loadme.push_back(new ReadInSamples);

            AllSamples.push_back(new Sample);
            loadme[i] -> ReadInThisSample = AllSamples[i];
            loadme[i] -> delim = delim;

			loadme[i] -> ReadInLine[0].str(row);
            getline(input_file, row);
            loadme[i] -> ReadInLine[1].str(row);

            pthread_create( &tempTid, NULL, ReadInSampleFunc, loadme[i] );
            tid.push_back(tempTid);

            // sleep(1000);
            // cout << "press enter for main to continue" << endl;
            // getchar();

			// getline(line1, Token, delim);
			// if (i % 2 == 0) {
			// 	AllSamples.push_back(new Sample);
			// 	AllSamples[idiv2]->MakeTitle(Token);
			// 	// string temp1;                        // Debugging to see that names ae loading correctly
			// 	// AllSamples[idiv2].CopyName(&temp1);
			// 	// std::cout << temp1 << endl;
			// }

			// while (getline(line1, Token, delim)) {
			// 	// cout << Token << endl; // for debugging
			// 	if (Token[0] == '-' || isdigit(Token[0])) {
			// 		int mynumber = stoi(Token);
			// 		AllSamples[idiv2]->AddToAllele((i % 2), mynumber);  // to add to the correct array of allele queues
			// 		// int temp;
			// 		// AllSamples[idiv2]->firstOnQueue((i%2), temp);   // Debugging code
			// 		// std::cout << temp << endl;
			// 	}
			// }

			i++;
			// idiv2 = i / 2;
			// std::cout << idiv2 << " | " << i % 2 << endl;
		}
	}
	// bool ok22; //debugging printout
	// int temp73;
	// ok22 = AllSamples[0]->firstOnQueue(0, temp73);

	// while (ok22) {
	//        std::cout << temp73 << " | ";
	//     ok22 = AllSamples[0]->NextOnQueue(0, temp73);

	// }
	// std::cout << endl ;

	input_file.close();

	SampleCount = i;
	int totalTest = (SampleCount * (SampleCount - 1)) / 2;
	//std::cout << SampleCount << endl;
	i = 0;
	float k = 0;
    // ----------      join here



    // Write compare and write string stream here



    // write stirng stream to file here
	ofstream output_file(strcat(argv[1], "Snipped.csv"));

	if (output_file.is_open()) {
		output_file << "sample1,sample2,total SNPs,mismatched SNPs,mismatch %,Hom Hom (ex: 00 11),Hom Homs %,";
		output_file << "Het Hom Lap (ex: 00 01),Het Hom Lap %,Het Hom non Lap (ex 00 12),Het Hom non Lap %,Het Het (ex: 01 12 or 01 23),Het Het %\n";

		int TotalSnps, mismatchedSnps, mismatchBothHoms, mismatchHetHomOverlap, mismatchHetHomNonOverlap, mismatchHetHet;
		float mismatchedSnpsPerc, mismatchBothHomsPerc, mismatchHetHomOverlapPerc, mismatchHetHomNonOverlapPerc, mismatchHetHetPerc;

		std::cout << "File is loaded, output file is being written to: " << endl
				  << argv[1] << endl
				  << endl;

		while (i < SampleCount - 1) {
			for (j = i + 1; j < SampleCount; j++) {

                // Functionise this for THreading

				TotalSnps = 0;
				mismatchedSnps = 0;
				mismatchBothHoms = 0;
				mismatchHetHomOverlap = 0;
				mismatchHetHomNonOverlap = 0;
				mismatchHetHet = 0;

				int value[2][2];  // Load an array with the sample values for comparison
				bool ok1, ok2, ok3, ok4;
				ok1 = AllSamples[i]->firstOnQueue(0, value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
				ok2 = AllSamples[i]->firstOnQueue(1, value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
				ok3 = AllSamples[j]->firstOnQueue(0, value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
				ok4 = AllSamples[j]->firstOnQueue(1, value[1][1]);  // j is the second sample name to compare, 1 is the 2nd allele from that sample name

				while (ok1 && ok2 && ok3 && ok4) {
					sortSnips(value);

					if (CheckTotal(value, lowindex, highindex)) {
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

					// std::cout << value[0][0] << value[0][1] << value[1][0] << value[1][1] << endl; // for debugging
					ok1 = AllSamples[i]->NextOnQueue(0, value[0][0]);  // i is the first sample name to compare, 0 is the 1st allele from that sample name
					ok2 = AllSamples[i]->NextOnQueue(1, value[0][1]);  // i is the first sample name to compare, 1 is the 2nd allele from that sample name
					ok3 = AllSamples[j]->NextOnQueue(0, value[1][0]);  // j is the second sample name to compare, 0 is the 1st allele from that sample name
					ok4 = AllSamples[j]->NextOnQueue(1, value[1][1]);
				}

				mismatchedSnpsPerc = static_cast<float>(mismatchedSnps) / static_cast<float>(TotalSnps) * 100.0;
				mismatchBothHomsPerc = static_cast<float>(mismatchBothHoms) / static_cast<float>(TotalSnps) * 100.0;
				mismatchHetHomOverlapPerc = static_cast<float>(mismatchHetHomOverlap) / static_cast<float>(TotalSnps) * 100.0;
				mismatchHetHomNonOverlapPerc = static_cast<float>(mismatchHetHomNonOverlap) / static_cast<float>(TotalSnps) * 100.0;
				mismatchHetHetPerc = static_cast<float>(mismatchHetHet) / static_cast<float>(TotalSnps) * 100.0;

				std::cout << std::setprecision(2) << std::fixed;

				string sample1, sample2, SnpsPerc, BothHomsPerc, HomLapPerc, HetHomNonLapPerc, HetHetPerc;

				AllSamples[i]->CopyName(&sample1);
				AllSamples[j]->CopyName(&sample2);

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
				// temp22.str("");

				// std::cout << sample1 << " vs " << sample2 << endl; //for debugging
				output_file << sample1 << "," << sample2 << "," << TotalSnps
							<< "," << mismatchedSnps << "," << SnpsPerc << "," << mismatchBothHoms << ","
							<< BothHomsPerc << "," << mismatchHetHomOverlap << "," << HomLapPerc << ","
							<< mismatchHetHomNonOverlap << "," << HetHomNonLapPerc << ","
							<< mismatchHetHet << "," << HetHetPerc << endl;

				if (argc == 3) {
					k++;
					stringstream output, leftsample2;
					leftsample2 << left << setw(15) << sample2;
					string opt, ls2;
					ls2 = leftsample2.str();
					output << setw(8) << setprecision(4) << k / static_cast<float>(totalTest) * 100.0 << "% records processed so far, sample pair: " << setw(15) << sample1 << " vs " << ls2 << " do not open " << argv[1] << " until completed." << '\r';
					opt = output.str();
					leftsample2.str("");
					output.str();

					cout << opt << '\r';
				}
			}
			i++;
		}
	}
	output_file.close();
	for (vector<Sample *>::iterator it = AllSamples.begin(); it != AllSamples.end(); it++) {
		delete (*it);
	}

	std::cout << endl
			  << endl
			  << "Processing Complete, you may now use " << argv[1] << endl
			  << endl
			  << "Thanks for using SnipIt.SnipitGood feel free to donate " << endl
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