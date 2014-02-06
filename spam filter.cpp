/******************************************************************************
* Program:
*    Bayesian Spam Filter
*
* Author:
*    Sijun Liu
*
* Data resource:
*    This program uses the Spam Data Set from UCI machine learning repository.
*    http://archive.ics.uci.edu/ml/datasets/Spambase
*      The database has 4601 emails. Each email is one line in the data file. 
*    Each line has 58 numbers, where the first 57 numbers are the frequencies 
*    of a fixed set of 57 words in this corresponding email. The last number
*    (0 or 1) represents if the email is a spam (0) or good (or ham, 1) email.
*    The traning set (traing_set.data) has 4201 emails, with 1613 hams and 2588
*	 spams. The test set (test_set.data) has 400 emails, with 200 hams and 
*	 spams each.  
*
* Algorithm:
*	  This program uses the Naive Bayesian algorithm to build the spam filter, 
*	and use 4401 labeled (spam/ham as 0/1) emails to train the filter. Then it
*	uses 400 labeled emails as the test set, predicts spam/ham of each email, 
*	and compare the result with the label. The accuracy of this filter on the 
*	test set is about 78%.
*
*	  Let X be a vector with 57 entries, representing the frequency of a fixed
*	set of 57 words in an email. Let Y be the label of an email, Y = spam or ham.
*	Given X, the algorithm predicts the probability of it being spam or ham, i.e.
*	P(Y|X).
*
*	  The Bayesian formula says P(Y|X) = P(X|Y)*P(Y)/P(X), so this means 
*	    P(spam|X)/P(ham|X) = (P(X|spam)*P(spam))/(P(X|ham)*P(ham))
*	The email is considered spam, if the ratio is greater than 1, and ham if not.
*	  In the formula, 
*	(1) P(spam) = #spams/#all_emails, P(ham) = #hams/#all_emails
*	(2) P(X|spam) = product of P(word i|spam)^(#occurrance of word i in X) 
*			= prod_{i=1 to 57} P(word i|spam)^X[i], 
*		here 
*		P(word i|spam) = #word_i_occurrence_in_all_spams/#all_words_in_spams
*	(3) Similarly, P(X|ham) = prod_{i=1 to 57} P(word i|ham)^X[i], and 
*		P(word i|ham) = #word_i_occurrence_in_all_hams/#all_words_in_hams
*	
*	  When applying the formula, we make the following two modifications.
*	(1) Since the involved probabilities are generally very tiny numbers, using
*		the original formula may cause big a computational error.
*		To minimize the error, we use the log version, i.e. we use
*		log(P(spam|X)/P(ham|X)) = ( sum_{i=1 to 57} X[i]*(log(P(word i|spam)) 
*			- log(P(word i|ham))) ) + log(P(spam)) - log(P(ham))
*	(2) If all spam (resp. ham) emails don't have word i, then 
*		P(word i|spam) = 0 (resp. P(word i|ham) = 0), this become problematic 
*		in the above formula. To resolve this, we add a small number (call it 
*		OFF_SET) to the frequency of each word in each email. For the data sets
*		we use in this program, we choose OFF_SET = 1/4000. 
******************************************************************************/


#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<math.h>
#include<vector>
#include"Email.h"

using namespace std;

//  We fix 57 distinct words. For each email, count the number of occurrence of
//  each of the 57 words, and use the numbers to form a vector. In the data 
//  sets, each email is given by such a vector.
const int WORDS_SET_SIZE = 57;   

//  see the last part of the description of the algorithm for OFF_SET
const double OFF_SET = 1.0/4000;  

//  need this function frequently
vector<double> operator+(vector<double>& A, const vector<double>& B);

int main(){
	//  total number of occurrance of the words (from the word set) in spams
	vector<double> num_words_spam(WORDS_SET_SIZE,0);  
	//  total number of occurrance of the words (from the word set) in hams
	vector<double> num_words_ham(WORDS_SET_SIZE,0);
	//  total number of spam emails
	int num_spam = 0;
	//  total number of ham emails
	int num_ham = 0; 

	//  this vector contains the probability that word i occurs in spam emails,
	//  in other word, log(P(word i|spam))
	vector<double> word_probability_spam(WORDS_SET_SIZE,0);  
	
	//  this vector contains log(P(word i|ham)) for each word i in the word set
	vector<double> word_probability_ham(WORDS_SET_SIZE,0);   


	/**************************************************************************
	* read data from training_set, each line of data represents an email      *
	**************************************************************************/
	//  open training_set.data
	ifstream file; 
	file.open("training_set.data");
	if(!file){
		cerr<<"file cannot be open";
		exit(1);
	}

	Email* my_email = new Email();
	// email_data is to accept each line in the data file
	string email_data;
	while(!file.eof()){
		getline(file,email_data);
		//  my_email get all the data from email_data
		my_email->setEmailData(email_data);

		//  count the words in spam and ham, based on my_email is spam or not
		if(my_email->isHam() == 0){
			num_spam++;
			num_words_spam = num_words_spam
				+ my_email->wordsFrequency();
		}
		if(my_email->isHam() == 1){
			num_ham++;
			num_words_ham = num_words_ham
				+ my_email->wordsFrequency();
		}
	}
	file.close();

	/**************************************************************************
	* calculate word_probability_spam and word_probability_ham                *
	**************************************************************************/
	//  total number of words in spam and ham
	double total_words_spam = 0;
	double total_words_ham = 0;

	for(int i=0;i<57;i++){
		//  sum up the word i occurrence in spams and hams
		total_words_spam += num_words_spam[i];
		total_words_ham += num_words_ham[i];
	}

	for(int i=0;i<57;i++){
		//  calculate log(P(word i|spam)) and log(P(word i|ham))
		word_probability_spam[i] = log(num_words_spam[i]/total_words_spam);
		word_probability_ham[i] = log(num_words_ham[i]/total_words_ham);
	}


	/**************************************************************************
	* read emails from the test_set, predict if the email is spam or ham      *
	**************************************************************************/
	//  count total number of emails, spam/ham emails in the test set
	int num_test_email = 0;
	int num_ham_in_test = 0;
	int num_spam_in_test = 0;

	//  count the number of spam/ham emails predicted as spam/ham
	int ham_predict_as_ham = 0;
	int spam_predict_as_spam = 0;
	int ham_predict_as_spam = 0;
	int spam_predict_as_ham = 0;

	ifstream test;
	test.open("test_set.data");
	if(!test){
		cerr<<"cannot open the test set";
		exit(1);
	}

	while(!test.eof()){
		//  use string email_data again to get a line of data from test file
		getline(test,email_data);
		//  use email_data to set up an email
		my_email->setEmailData(email_data);
		num_test_email++;

		//  calculate the log ratio log(P(spam|X)/P(ham|X)), where X is my_email
		//  log ratio > 0 predicts my_email is spam, <0 predicts ham
		double log_ratio=0;

		// calculate log ratio
		for(int i=0;i<57;i++){
			log_ratio += word_probability_spam[i]*((my_email->wordsFrequency())[i])
				- word_probability_ham[i]*((my_email->wordsFrequency())[i]);
		}
		log_ratio += log((double)num_spam) - log((double)num_ham);

		//  predict the email, and count how many are predicted correct (spam as
		//  spam, ham as ham) and predicted wrong (spam as ham, ham as spam)
		if(!my_email->isHam()) {num_ham_in_test++;}
		else {num_spam_in_test++;}
		if(log_ratio<0 && my_email->isHam()) ham_predict_as_ham++;
		if(log_ratio<0 && !my_email->isHam()) spam_predict_as_ham++;
		if(log_ratio>0 && my_email->isHam()) ham_predict_as_spam++;
		if(log_ratio>0 && !my_email->isHam()) spam_predict_as_spam++;
	}
	test.close();

	/**************************************************************************
	* display the results                                                     *
	**************************************************************************/

	cout<<"The training set has "<<(num_ham + num_spam)<<" emails"<<endl;
	cout<<"Spam emails: "<<num_spam<<endl;
	cout<<"Ham emails: "<<num_ham<<endl;
	
	cout<<"The test set has "<<num_test_email<<" emails"<<endl;
	cout<<"Spam emails: "<<num_spam_in_test<<endl;
	cout<<"Ham emails: "<<num_ham_in_test<<endl;
	cout<<"Ham predicted as ham: "<<ham_predict_as_ham<<endl;
	cout<<"Spam predicted as spam: "<<spam_predict_as_spam<<endl;
	cout<<"Spam predicted as ham: "<<spam_predict_as_ham<<endl;
	cout<<"Ham predicted as spam: "<<ham_predict_as_spam<<endl;
	cout<<"The prediction accurary is: "<<
		(ham_predict_as_ham+spam_predict_as_spam)*1.0/num_test_email*100<<
		" percents"<<endl;
	system("pause");
	return 1;
}

// need the addition of vectors frequently
vector<double> operator+(vector<double>& A, const vector<double>& B){
	if(A.size()!=B.size()){
		throw "The sizes of the two vector objects are not equal";
	}
	vector<double> ans;
	for(int i=0;i<A.size();i++){
		ans.push_back(A[i]+B[i]);
	}
	return ans;
}