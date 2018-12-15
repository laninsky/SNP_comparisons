// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

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
  bool FirstItem(T & item);
  bool NextItem(T & item);
};

template <class T>
List<T>::List() {
  front = NULL;  current = NULL;
}

template <class T>
List<T>::~List() {

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
bool List<T>::FirstItem(T & item) {
  if (front == NULL) { return false; }
  item = front->data;
  current = front;
  return true;
}

template <class T>
bool List<T>::NextItem(T & item) {
  current = current->next;
  if (current == NULL) { return false; }
  item = current->data;
  return true;
}

class Sample {
    private:
        string samplename;
        class List<int> alleolelist[2];
    
    public:
        Sample();
        ~Sample();
        void MakeTitle(string s);
        void CopyName(string *copyname);
        void AddToAllele(int i, int newvalue);
        bool firstOnQueue(int i, int &value);
        bool NextOnQueue(int i, int &value);
};

Sample::Sample() {


}

Sample::~Sample() {

}

void Sample::MakeTitle(string s){
    samplename = s;
}

void Sample::CopyName(string *copyname){
    *copyname = samplename;
    // string dog = *copyname;
    // cout << dog << endl;
}

void Sample::AddToAllele(int i, int newvalue){
    alleolelist[i].AddtoFront(newvalue);
}

bool Sample::firstOnQueue(int i, int &value){
    bool ok = alleolelist[i].FirstItem(value);
    return ok;
}
bool Sample::NextOnQueue(int i, int &value){
    bool ok = alleolelist[i].NextItem(value); 
    return ok;
}

void sortSnips(int value[2][2]){
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

int CheckTotal( int value[2][2]){
    for (int i=0; i<2; i++){
        for(int j=0 ; j<2; j++){
            if (value[i][j] < 0 || value[i][j] > 3) {
                return 0;
            }
        }
    }
    return 1;
}

//matches 01 01 or 01 10 or 11 11

int CheckMisMatch( int value[2][2]) {
    if ( value[0][0] == value[1][0] && 
            value[0][1] == value[1][1]) {
        return 0;
    } else {
        return 1;
    }
}

//hom hom the same but different 11 22

int HomPair(int value1[2]){
    if (value1[0] == value1[1]){
        return 1;
    }
    return 0;
}

int CheckHomMisMatch( int value[2][2]){
  if ( HomPair(value[0]) && HomPair(value[1]) && (value[0][0] != value[1][0])) {
        return 1;
    } else {
        return 0;
    }
}

//het hom overlapping one is hom one of the hets is the same value as the hom 11 12

int CheckHetHomOverlapMismatch( int value[2][2]){
    if ( (HomPair(value[0]) && 
        (value[0][0] == value[1][0] || value[0][0] == value[1][1])) ||
        (HomPair(value[1]) && 
        (value[1][0] == value[0][0] || value[1][0] == value[0][1]))) {
    return 1;
    }
    return 0;
}


//het hom non overlapping 11 23

int CheckHetHomNonOverlapMismatch( int value[2][2]){
    if ( (HomPair(value[0]) && 
        (value[0][0] != value[1][0] || value[0][0] != value[1][1])) ||
        (HomPair(value[1]) && 
        (value[1][0] != value[0][0] || value[1][0] != value[0][1]) ) ) {
    return 1;
    }
    return 0;
}

//het het 0112  or 0123

int CheckHetHetMisMatch( int value[2][2]){
    if ( value[0][0] != value[0][1] || 
            value[1][0] != value[1][1]) {
        return 1;
    } else {
        return 0;
    }
}








// -------------------------- v Main Starts Here v --------------------- //

int main( int argc, char** argv ){ //get arguments from command line, i.e., yourexec filename
	int i,j, SampleCount, ChromosoneCount;
    vector <Sample> AllSamples;

	ifstream input_file;
	if(argc!=2) {
	    cout<< "needs a filename as argument  " << endl;
	    exit(0);
	}
	input_file.open(argv[1]);
	if(!input_file.good()){
	    cout<< "cannot read file " << argv[1] << endl;
	    exit(0);
	}
    i=0;
    int idiv2 = 0;
    ChromosoneCount = 0;
    string Token, row;
    istringstream line;
	while(!input_file.eof()){
        
        while( getline(input_file, row)){
            line.str(row);

            getline(line, Token, ' ');
            if ( i % 2 == 0) {
                class Sample s;
                AllSamples.push_back(s);
                AllSamples[idiv2].MakeTitle( Token);
                // string temp1;                        // Debugging to see that names ae loading correctly
                // AllSamples[idiv2].CopyName(&temp1);
                // cout << temp1 << endl;
            }

            while ( getline(line, Token, ' ')) {        
                if (Token[0] == '-' || isdigit(Token[0]) ) {
                    int mynumber  = stoi(Token );
                    AllSamples[idiv2].AddToAllele((i % 2), mynumber); // to add to the correct array of allele queues
                    // int temp;
                    // AllSamples[idiv2].firstOnQueue((i%2), temp);   // Debugging code             
                    // cout << temp << endl;
                }
            }
            line.clear();
            i++;
            idiv2 = i / 2;
            // cout << idiv2 << " | " << i % 2 << endl;

        }
    }
    // bool ok22; //debugging printout
    // int temp73;
    // ok22 = AllSamples[0].firstOnQueue(0, temp73);
    
    // while (ok22) {
    //        cout << temp73 << " | ";
    //     ok22 = AllSamples[0].NextOnQueue(0, temp73);
     
    // }
    // cout << endl ;

    input_file.close();
    SampleCount = idiv2;
    //cout << SampleCount << endl;
    i=0;
    int LazyCount = 1000;
    ofstream output_file ( strcat( argv[1], ".output.txt"));
    
    if ( output_file.is_open() ){
        output_file << "# sample1 \t\t\t| sample2 \t\t\t| total SNPs \t| mismatched SNPs \t| mismatch % \t| Hom Hom (ex: 00 11) \t| Hom Homs % \t| ";
        output_file << "Het Hom Lap (ex: 00 01) \t| Het Hom Lap % \t| Het Hom non Lap (ex 00 12) \t| Het Hom non Lap % \t| Het Het (ex: 01 12 or 01 23) \t| Het Het %\n";
 
        int TotalSnps, mismatchedSnps, mismatchBothHoms, mismatchHetHomOverlap, mismatchHetHomNonOverlap, mismatchHetHet;
        float mismatchedSnpsPerc, mismatchBothHomsPerc, mismatchHetHomOverlapPerc, mismatchHetHomNonOverlapPerc, mismatchHetHetPerc;

        cout << "File is loaded correctly, output file is being written to " << argv[1] << endl << "you will be notified every 1000 records processed" << endl << endl;

        while ( i < SampleCount-1 ){

            for (j = i+1 ; j < SampleCount; j++){

                TotalSnps = 0;
                mismatchedSnps = 0;
                mismatchBothHoms = 0;
                mismatchHetHomOverlap = 0;
                mismatchHetHomNonOverlap = 0;
                mismatchHetHet = 0;
                
                int value[2][2];                                      // Load an array with the sample values for comparison
                bool ok1, ok2, ok3, ok4;
                ok1 =  AllSamples[i].firstOnQueue(0, value[0][0]);   // i is the first sample name to compare, 0 is the 1st allele from that sample name
                ok2 =  AllSamples[i].firstOnQueue(1, value[0][1]);   // i is the first sample name to compare, 1 is the 2nd allele from that sample name
                ok3 =  AllSamples[j].firstOnQueue(0, value[1][0]);   // j is the second sample name to compare, 0 is the 1st allele from that sample name
                ok4 =  AllSamples[j].firstOnQueue(1, value[1][1]);   // j is the second sample name to compare, 1 is the 2nd allele from that sample name

               
                while (ok1 && ok2 && ok3 && ok4) {

                    sortSnips(value);
                                                
                    if (CheckTotal(value)) {
                        TotalSnps++;
                        if (CheckMisMatch(value)){
                            mismatchedSnps++;
                            if (CheckHomMisMatch(value)){
                                mismatchBothHoms++;
                            } else if (CheckHetHomOverlapMismatch(value)){
                                mismatchHetHomOverlap++;
                            } else if (CheckHetHomNonOverlapMismatch(value)){
                                mismatchHetHomNonOverlap++;
                            } else if (CheckHetHetMisMatch( value)) {
                                mismatchHetHet++;
                            }
                        }
                    }

                    // cout << value[0][0] << value[0][1] << value[1][0] << value[1][1] << endl; // for debugging
                    ok1 =  AllSamples[i].NextOnQueue(0, value[0][0]);   // i is the first sample name to compare, 0 is the 1st allele from that sample name
                    ok2 =  AllSamples[i].NextOnQueue(1, value[0][1]);   // i is the first sample name to compare, 1 is the 2nd allele from that sample name
                    ok3 =  AllSamples[j].NextOnQueue(0, value[1][0]);   // j is the second sample name to compare, 0 is the 1st allele from that sample name
                    ok4 =  AllSamples[j].NextOnQueue(1, value[1][1]);

                }

                mismatchedSnpsPerc = static_cast<float>( mismatchedSnps) / static_cast<float>( TotalSnps) * 100.0;
                mismatchBothHomsPerc = static_cast<float>( mismatchBothHoms) / static_cast<float>( TotalSnps) * 100.0;
                mismatchHetHomOverlapPerc = static_cast<float>(mismatchHetHomOverlap) / static_cast<float>( TotalSnps )* 100.0;
                mismatchHetHomNonOverlapPerc = static_cast<float>(mismatchHetHomNonOverlap) / static_cast<float>( TotalSnps) * 100.0;
                mismatchHetHetPerc = static_cast<float>(mismatchHetHet) / static_cast<float>( TotalSnps) * 100.0;

                std::cout << std::setprecision(2) << std::fixed;

                string sample1, sample2 , SnpsPerc, BothHomsPerc, HomLapPerc, HetHomNonLapPerc, HetHetPerc;

                AllSamples[i].CopyName(&sample1);
                AllSamples[j].CopyName(&sample2);

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

                // cout << sample1 << " vs " << sample2 << endl; //for debugging
                output_file << left << setw(19) << sample1 << " | " << setw(17) << sample2 << " | " << setw(13) << TotalSnps 
                        << " | " << setw(17) << mismatchedSnps << " | " <<  setw(13) << SnpsPerc << " | " << setw(21) << mismatchBothHoms << " | " 
                        << setw(13) << BothHomsPerc << " | " << setw(19+6) << mismatchHetHomOverlap << " | " << setw(19-2) << HomLapPerc << " | " 
                        << setw(19+10) << mismatchHetHomNonOverlap << " | " << setw(19+2) << HetHomNonLapPerc << " | " 
                        << setw(19+10) << mismatchHetHet << " | " << setw(19) << HetHetPerc << endl;
            
            }
            i++;
            if (i % LazyCount == 0){
                cout << i <<" records processed so far, do not open output file until completed." << endl;
            }
        }
    }
    output_file.close();
    if (i < LazyCount) {
        cout << "Less than 1000 entries!?" << endl << "Are you working hard or hardly working ;)" << endl << endl;
    }
    cout << "Processing Complete, you may now use " << argv[1] << endl << endl << "Thanks for using SnipIt.SnipitGood feel free to donate " << endl << "or request other features or programs at www.SierraAlpha.co.nz" << endl << endl;
}

//cases
//matches 01 01 or 01 10 or 11 11
//hom hom the same but different 11 22
//het hom overlapping one is hom one of the hets is the same value as the hom 11 12
//het hom non overlapping 11 23
//het het 0112  or 0123

// Carefully Coded by Sierra Alpha Innovations --> www.SierraAlpha.co.nz