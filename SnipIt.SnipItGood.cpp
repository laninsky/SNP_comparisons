#include <cstdlib>
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <vector>
#include <string>

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
        vector<char> samplename;
        class List<int> alleolelist[2];
    
    public:
        Sample();
        ~Sample();
        void addCharToTitle(int i, char c);
        void CopyName(vector<char> *copyname);
        void AddToAllele(int i, int newvalue);
        bool firstOnQueue(int i, int *value);
        bool NextOnQueue(int i, int *value);
};

Sample::Sample() {
    samplename[0] = '/0';

}

Sample::~Sample() {

}

void Sample::addCharToTitle(int i, char c){
    samplename[i] = c;
    samplename[i+1] = '\0';
}

void Sample::CopyName(vector<char> *copyname){
    copyname = &samplename;
}

void Sample::AddToAllele(int i, int newvalue){
    alleolelist[i].AddtoFront(newvalue);
}

bool Sample::firstOnQueue(int i, int *value){
    return alleolelist[i].FirstItem(*value);
}
bool Sample::NextOnQueue(int i, int *value){
    return alleolelist[i].NextItem(*value);
}

int CheckTotal( int value[4]){
    for (int i=0; i<4; i++){
        if (value[i] < 0 || value[i] > 3) {
            return 0;
        }
    }
    return 1;
}

int CheckMisMatch( int value[4]){
    if ( value[0] == value[1] && 
            value[1] == value[2] && 
            value[2] == value[3]) {
        return 0;
    } else {
        return 1;
    }
}

int CheckHetMisMatch( int value[4]){
    if ( value[0] != value[1] || 
            value[3] != value [4]) {
        return 1;
    } else {
        return 0;
    }
}

int CheckHomMisMatch( int value[4]){
  if ( value[0] == value[1] && 
            value[1] != value[2] && 
            value[2] == value[3] ) {
        return 1;
    } else {
        return 0;
    }
}

// int CheckHetHomMismatch( int value[4]){
//     for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++){

//         }
//     }
// }

int main( int argc, char** argv ){ //get arguments from command line, i.e., yourexec filename
	int i,j, SampleCount, ChromosoneCount;
    vector <Sample> AllSamples;
	string expression;
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
    ChromosoneCount = 0;
	while(!input_file.eof()){
		getline(input_file,expression);
        j=0;
		while (!isspace(expression[j])) {
            if ( j % 2 == 0) {
		        AllSamples[i].addCharToTitle(j, expression[j]);
            } else {}
            j++;
        } 
        while (expression[j] != '\n' || expression[j] != '\0'){
		    int temp;
            sscanf(&expression[j], "%d", &temp );
            AllSamples[i].AddToAllele((i % 2), temp); // to add to the correct array of allele queues
            j++;
            if (temp < 0) {
                j++;
            }
        }
        i++;
    }
    input_file.close();
    SampleCount = i - 1;
    i=0;
    ofstream output_file ( strcat( argv[1], ".output.txt"));
    if ( output_file.is_open() ){
        output_file << "sample1 \t| sample2 \t| total_SNPs \t| mismatched_SNPs \t| mismatch_perc \t| mismatch_both_hets \t| both_hets_perc \t| mismatch_both_homs \t| non_both_hets_both_homs_perc \t| mismatch_het_hom \t| non_both_hets_perc_het_hom\n";
        int TotalSnps, mismatchedSnps, mismatchBothHets, mismatchBothHoms, mismatchHetHom;
        float mismatchedSnpsPerc, mismatchBothHetsPerc, mismatchBothHomsPerc, mismatchHetHomPerc;
        while ( i < SampleCount-1 ){
            TotalSnps = 0;
            mismatchedSnps = 0;
            mismatchBothHets = 0;
            mismatchBothHoms = 0;
            mismatchHetHom = 0;
            for (j = i+1 ; j < SampleCount; j++){
                int value[4];
                bool ok1, ok2, ok3, ok4;
                ok1 =  AllSamples[i].firstOnQueue(0, value);   // i is the first sample name to compare, 0 is the 1st allele from that sample name
                ok2 =  AllSamples[i].firstOnQueue(1, value + 1);   // i is the first sample name to compare, 1 is the 2nd allele from that sample name
                ok3 =  AllSamples[j].firstOnQueue(0, value + 2);   // j is the second sample name to compare, 0 is the 1st allele from that sample name
                ok4 =  AllSamples[j].firstOnQueue(1, value + 3);   // j is the second sample name to compare, 1 is the 2nd allele from that sample name
                while (ok1 && ok2 && ok3 && ok4) {
                    if (CheckTotal(value)) {
                        TotalSnps++;
                        if (CheckMisMatch(value)){
                            mismatchedSnps++;
                            if (CheckHetMisMatch(value)){
                                mismatchBothHets++;
                            } else if (CheckHomMisMatch(value)) {
                                mismatchBothHoms ++;
                            } else {                      
                                mismatchHetHom ++;
                            }
                        }
                    }

                    ok1 =  AllSamples[i].NextOnQueue(0, value);   // i is the first sample name to compare, 0 is the 1st allele from that sample name
                    ok2 =  AllSamples[i].NextOnQueue(1, value + 1);   // i is the first sample name to compare, 1 is the 2nd allele from that sample name
                    ok3 =  AllSamples[j].NextOnQueue(0, value + 2);   // j is the second sample name to compare, 0 is the 1st allele from that sample name
                    ok4 =  AllSamples[j].NextOnQueue(1, value + 3); 
                }
            }
            mismatchedSnpsPerc = mismatchedSnps / TotalSnps;
            mismatchBothHetsPerc = mismatchBothHets / TotalSnps;
            mismatchBothHomsPerc = mismatchBothHoms / TotalSnps;
            mismatchHetHomPerc = mismatchHetHom / TotalSnps;

            vector<char> *sample1, *sample2;

            AllSamples[i].CopyName(sample1);
            AllSamples[j].CopyName(sample2);
            
            output_file << sample1 << " \t| " << sample2 << " \t| " << TotalSnps << " \t| " << mismatchedSnps << " \t| " <<  mismatchedSnpsPerc << " \t| " << mismatchBothHets << " \t| " << mismatchBothHetsPerc << " \t| " << 
                    mismatchBothHoms << " \t| " << mismatchBothHomsPerc << " \t| " << mismatchHetHom << " \t| " << mismatchHetHomPerc << endl;
        }
    }
    output_file.close();
}
