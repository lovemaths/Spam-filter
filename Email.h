#pragma once

#include <vector>
#include <string>

extern const int WORDS_SET_SIZE;
extern const double OFF_SET;

using std::vector;
using std::string;


class Email{
 private: 
	 //  the size of a fixed word set for all email objects
	int word_set_size_;  

	//  represent the frequency of words (from the word set) in the email, the 
	//  size of the vector equals word_set_size
	vector<double> words_frequency_;  

	//  0 for spam and 1 for ham								 
	int spam_or_ham_;  

	//  the total number of words in this email
	double num_words_;  

public:
	Email();
	~Email();

	int isHam() const;

	//  return total number of words
	double numWords() const; 

	//  return words_frequency_
	const vector<double>& wordsFrequency() const;

	//  the string email_data has (word_set_size+1) many numbers, the first 
	//  word_set_size many numbers record the word frequency, and are used to
	//  fill the vector words_frequency_. The last number is 0 for spam and 1 
	//  for ham, and is used to fill spam_or_ham
	void setEmailData(string& email_data);
};