#include "Email.h"
#include<iostream>
#include<sstream>

using std::vector;
using std::string;
using std::stringstream;

Email::Email(){
	word_set_size_ = WORDS_SET_SIZE;
	vector<double> tmp(WORDS_SET_SIZE,0);
	words_frequency_ = tmp;
	spam_or_ham_ = 0;
	num_words_ = 0;
}

Email::~Email(){}

int Email::isHam() const{ return spam_or_ham_; }

double Email::numWords() const {return num_words_; }

const vector<double>& Email::wordsFrequency() const{
	return words_frequency_;
}

void Email::setEmailData(string& email_data){
	//  email_data is like "1,2,3, ... ,1", which has (word_set_size+1) many 
	//  numbers (last one is 0 or 1), separated by a single coma.
	//  the last number is for spam_or_ham, others are used to fill the vector
	// words_frequency_.

	//  convert email_data to a stringstream ss;
	stringstream ss(email_data);
	//  use string s to catch each number in the stringstream ss (equivalently,
	// catch each number in email_data)
	string s;
	for(int i=0;i<word_set_size_+1;i++){
		//  get a number from ss, and store in string s
		getline(ss,s,',');
		//  use stringstream toNumber to convert s to a double type number
		stringstream toNumber(s);
		if(i == word_set_size_){
			//  the last number is for spam_or_ham_
			toNumber>>spam_or_ham_; 
		}else{
			//  other numbers are to fill the vector words_frequency_
			toNumber>>words_frequency_[i];
			//  the algorithm requires to add a tiny positive number (call it 
			//  OFF_SET) to avoid 0 in words_frequency_
			words_frequency_[i] += OFF_SET; 
			//  count the total number of words in this email
			num_words_ += words_frequency_[i];
		}
	}	
}
