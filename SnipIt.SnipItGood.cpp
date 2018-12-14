#include <cstdlib>
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <vector>

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
        vector <char> samplename;
        class Queue alleolelist[2];
    
    public:
        Sample();
        ~Sample();
        void addCharToTitle(int i, char c);
        void PrintName();
        bool AddToAllele(int i, int newvalue);
        bool firstOnQueue(class Queue queue);
        bool nextOnQueue(class Queue queue);
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

void Sample::PrintName(){
    
}
bool Sample::AddToAllele(int i, int newvalue){
    alleolelist[i].Join(newvalue);
}
bool Sample::firstOnQueue(class Queue queue){
    
}
bool Sample::nextOnQueue(class Queue queue){

}

int main( int argc, char** argv ){ //get arguments from command line, i.e., yourexec filename
	int i,j;
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
	while(!input_file.eof()){
		getline(input_file,expression);
        j=0;
		while (!isspace(expression[j])){
		  AllSamples[i].addCharToTitle(j, expression[j]);
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
