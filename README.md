# SNP_comparisons
Calculating numbers of mismatching loci based on structure files, using multithreading so its really fast.

An example of the speedup for a .stru that was 600+ lines long outputing 50000+ results
R = 45mins   
- [ ] [@laninsky]( https://github.com/laninsky ) to confirm
Cpp (Single Thread)  = 15mins
Cpp (Multi Threaded) = 1min 35secs

## Contents:

- [SNP_comparisons](#snp_comparisons)
  * [Contents](#contents)
  * [What](#what)
  * [Why](#why)
  * [How Does it Work](#how-does-it-work)
  * [How Do I Use It](#how-do-i-use-it)
  * [R integration](#r-integration)

## What

SnipIt is a program written to help genetiscists compare locii matches and mismatches between different samples. 
- [ ] [@laninsky]( https://github.com/laninsky ) to explain more here

Currently only supports Unix like operating systems

## Why

- [ ] [@laninsky]( https://github.com/laninsky ) also to explain more here

## How Does it Work?

### SnipIt
Uses multiple threads to read in the structure files into memory, once these are read in it again uses multiple threads to run through the different combinations of pairs of samples to compare their locii matches or different mismatches. It then writes this to a .CSV for use in other applications as required.

## How Do I Use It?

### SnipIt

1. You need to be using a Unix based machine (if you want a windows support let me [@Sierra-Alpha]( https://github.com/Sierra-Alpha ) know)

2. You need a cpp compiler probably gnu gcc available here: https://gcc.gnu.org/

3. Download the SnipIt.cpp and makefile from this repository into the folder where you want it to live

4. Using your terminal navigate to that folder and simply type `make`

5. Easiest is to operate from that folder and copy in the stru file you want to use or alternativly call the program with the path to your structure file then type:

   `./SnipIt <input_filename> <lowest_allele> <delim_character>`

   ###### `<input_filename>`
   The name of the structure file to be read in and snipped, include the path if it is not in the same directory.
   ###### `<lowest_allele>`
   Specify the lowest number used to represent an allele, this program assumes they are represented by 4 consecutive numbers, any number outside this are considered a non sample.
   ###### `<delim_characters>`
   The character used to delimit samples in the stru file, use \\t or \\s for tab and space respectivly.
                        
6. Checkout your results at `<input_filename>`Snipped.csv at either the folder SnipIt is in or at the location of your path to `<input_filename>`


## R integration

- [ ] [@laninsky]( https://github.com/laninsky ) to figure out and document here
