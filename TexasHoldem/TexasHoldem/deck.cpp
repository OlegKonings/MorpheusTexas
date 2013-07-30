#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <queue>
#include <cmath>
#include <map>
#include <set>
#include <ctime>
#include "deck.h"

#ifndef USE_STD_NAMESPACE
#define USE_STD_NAMESPACE
#include <cstring>
#include <string>
using namespace std;
#endif

//struct definitions
//Card
card::card():value(-1),suit(-1){}//default constructor

card::card(const string &s){//constructor definition
	//first get the card value
	if(s[0]>='2' && s[0]<='9')value=int(s[0]-'0');//convert value to integer from char
	else{
		if(s[0]=='T')value=10;
		else if(s[0]=='J')value=11;
		else if(s[0]=='Q')value=12;
		else if(s[0]=='K')value=13;
		else if(s[0]=='A')value=14;
	}
	//then cast the suit as an int
	if(s[1]=='H')suit=0;
	else if(s[1]=='S')suit=1;
	else if(s[1]=='D')suit=2;
	else 
		suit=3;
}

//STL::set<card> needs this for ordering of hands
bool card::operator<(const card &other)const{// used to sort card type
	return (value!=other.value) ? (value<other.value):(suit<other.suit);
}

//TexasPoker default constructor which makes sure it starts fresh and clear
TexasPoker::TexasPoker(){//make sure to start with clean data struct can use 'this' pointer because struct/class will be initialized before it evaluates 
	this->clear_data();
}

void TexasPoker::clear_data(){//clear out any old data so can have a fresh start
	this->showing.clear();
	this->initialDeal.clear();
	this->Ranking.clear();
	this->used.clear();
}

RETURN_CODE TexasPoker::is_valid_data(const string &s){//verifies that card data is of correct type
	if(s.length()==0 || s.length()>2)return INCORRECT_FORMAT;
	bool a=(s[0]=='T' || s[0]=='J' || s[0]=='Q' || s[0]=='K' || s[0]=='A' || (s[0]>='2' && s[0]<='9'));
	bool b=(s[1]=='H' || s[1]=='S' || s[1]=='D' || s[1]=='C');
	return ( a && b ) ? NO_ERRORS:INCORRECT_FORMAT;
}

RETURN_CODE TexasPoker::get_cards_showing(const string &firstLine){//cache in vector<cards> the current showing cards
	string cur="";
	int num_cards=0;
	for(int i=0;i<firstLine.length();i+=3){//parses string to get the five first cards
		cur=firstLine.substr(i,2);
		if(this->is_valid_data(cur)==INCORRECT_FORMAT)return INCORRECT_FORMAT;
		card temp(cur);
		if(this->used.count(temp))return DUPLICATE_CARDS;
		this->used.insert(temp);
		this->showing.push_back(temp);
		num_cards++;
	}
	return (num_cards==NUM_CARDS) ? NO_ERRORS:INCORRECT_FORMAT;
}

Psi TexasPoker::get_value_hand(const set<card> &S){//will have 5 cards, will return pair<string,int> were the string is the description of hand, and the int is the value

	if(S.size()!=NUM_CARDS)return make_pair("Error",-1);//if there are the incorrect number of cards return error message

	int freqValue[DIF_CHARACTERS+1]={0};
	int freqSuit[DIF_SUITS+1]={0};

	//get frequencies of card values and of suits
	for(set<card>::const_iterator sit=S.begin();sit!=S.end();sit++){
		freqValue[sit->value]++;
		freqSuit[sit->suit]++;
	}

	int max_freqv=-1,whichv=-1,highcard=-1,pair2a=-1,pair2b=-1,raw_value=0;//keep track of the type of hand, also finds frequency of each value or suit
	for(int i=2;i<DIF_CHARACTERS;i++){
		if(freqValue[i]>max_freqv){//find the value with the highest frequency
			max_freqv=freqValue[i];
			whichv=i;
		}
		if(freqValue[i]>0){//keep track of all values seen and highcard

			highcard=i;//keep track of highcard
			if(freqValue[i]==1){
				raw_value|=(1<<(i-1));
			}
			if(freqValue[i]==2){//there is a pair, store the value and if there is another, store that value
				if(pair2a==-1)pair2a=i;
				else
					pair2b=i;
			}
		}
	}

	int max_freqs=-1,whichs=-1;
	for(int i=0;i<4;i++){//get info for the suits in hand
		if(freqSuit[i]>max_freqs){
			max_freqs=freqSuit[i];
			whichs=i;
		}
	}
	//Now evaluate in order of highest hands first

	//Royal Flush
	if(max_freqv==1 && freqValue[14]==1 && freqValue[13]==1 && freqValue[12]==1 && freqValue[11]==1 && freqValue[10]==1 && max_freqs==5){//royal flush
		string hand="Royal Flush "+SUITS[whichs];//Royal Flush always starts with Ace, so no need to state that fact
		return make_pair(hand,INF);//the highest possible value, about 500,000,000
	}

	//get more hand info
	bool isflush=bool(max_freqs==5),isStraight=false;
	int b=2,c=0;
	while(freqValue[b]==0){b++;}
	while(freqValue[b]==1){b++;c++;}
	if(c==5)isStraight=true;

	//Straight Flush
	if(isflush && isStraight){
		string hand="Straight Flush "+SUITS[whichs]+" , with high card "+CARDNAMES[highcard];
		return make_pair(hand,10000000+highcard);//next step down at 10 million
	}

	//Four of a kind
	if(max_freqv==4){
		string hand="Four of a Kind "+CARDNAMES[whichv];
		return make_pair(hand,9000000+whichv);//cant have equal Four of a kinds
	}

	//Full House
	if(max_freqv==3){
		b=2;
		while(b<DIF_CHARACTERS && freqValue[b]!=2){b++;}//check for another pair
		if(b<DIF_CHARACTERS && freqValue[b]==2){
			string hand="Full House, with 3 "+CARDNAMES[whichv]+"s , and 2 "+CARDNAMES[b]+'s';
			return make_pair(hand,8000000+43*whichv+b);
		}
		//if there is NOT another pair move on
	}

	//Flush*
	if(isflush){
		string hand="Flush "+SUITS[whichs]+", with high card "+CARDNAMES[highcard];
		return make_pair(hand,7000000+raw_value);
	}

	//Straight
	if(isStraight){
		string hand="Straight, with high card "+CARDNAMES[highcard];
		return make_pair(hand,6000000+highcard);
	}

	//Three of a Kind
	if(max_freqv==3){
		b=2;c=-1;
		int otherC=-1;
		while(b<DIF_CHARACTERS){//otherC will be higher of the two rema
			if(freqValue[b]==1){
				if(c==-1)c=b;
				else
					otherC=b;
			}
			b++;
		}
		string hand="Three of a Kind "+CARDNAMES[whichv]+"s , with a "+CARDNAMES[otherC]+" and a "+CARDNAMES[c];
		return make_pair(hand,5000000+811*whichv+otherC*17+c);
	}

	//Two Pair
	if(max_freqv==2 && pair2a>1 && pair2b>1 && pair2a!=pair2b){
		b=2;c=-1;
		while(b<DIF_CHARACTERS){//find the other card value
			if(freqValue[b]==1){
				c=b;
				break;
			}
			b++;
		}
		string hand="Two Pair ";
		b=0;
		if(pair2a>pair2b){//make sure higher pair value has much larger weight
			hand+=(" with the high pair of "+CARDNAMES[pair2a]+" , the other pair of "+CARDNAMES[pair2b]+" and the last card "+CARDNAMES[c]);
			b=pair2a*2663+pair2b*2557;
		}else{
			hand+=(" with the high pair of "+CARDNAMES[pair2b]+" , the other pair of "+CARDNAMES[pair2a]+" and the last card "+CARDNAMES[c]);
			b=pair2b*2663+pair2a*2557;
		}
		return make_pair(hand,4000000+b+c);
	}

	//Pair
	if(max_freqv==2 && pair2b==-1 && pair2a>0){
		string hand="Pair of "+CARDNAMES[pair2a]+"s , with ";
		b=DIF_CHARACTERS-1;
		while(b>1){//get other cards in descending order
			if(freqValue[b]==1){
				hand+=CARDNAMES[b]+' ';
			}
			b--;
		}
		return make_pair(hand,20000+pair2a*(1<<15)+raw_value);//here multiply the pair value by a larger value of 2^15 (which is above the max raw value) then add raw value of other cards
	}

	//High Card
	if(max_freqv==1){
		string hand="High Card: ";
		b=DIF_CHARACTERS-1;
		while(b>1){//get other cards in descending order
			if(freqValue[b]==1){
				hand+=CARDNAMES[b]+' ';
			}
			b--;
		}
		return make_pair(hand,raw_value);//no more than 2^13(8192)
	}
	return make_pair("Error",-1);//this should not be reached unless error, which is checked by calling function
}

RETURN_CODE TexasPoker::get_best_hand_for_player(const string &line){

	size_t f=line.find(' ');//break apart the string to get the current player and initial two cards
	string name=line.substr(0,f),c0=line.substr(f+1,2),c1=line.substr(f+4,2);
	if(this->initialDeal.count(name))return DUPLICATE_NAME;//duplicate name
	if(this->is_valid_data(c0)!=NO_ERRORS )return INCORRECT_FORMAT;//no valid card data
	if(this->is_valid_data(c1)!=NO_ERRORS )return INCORRECT_FORMAT;//no valid card data
	card C0(c0),C1(c1);
	if(this->used.count(C0) || this->used.count(C1))return DUPLICATE_CARDS;//duplicate cards
	this->initialDeal[name].push_back(C0);//keep track of who has what cards
	this->initialDeal[name].push_back(C1);
	this->used.insert(C0);//mark as 'used'
	this->used.insert(C1);//ditto

	set<card> active;//will store the current possible hand under examination
	int best=-1;//the staring value of the best hand
	pair<set<card>,string> Temp;//for caching the best hand for a given player

	for(int i=0;i<(1<<NUM_CARDS);i++)if(cntbt(i)==3){// 5 choose 3 possiblities of cards, will iterate through all 2^5 possible, but will only evaluate those using three cards
		active.clear();
		active.insert(C0);
		active.insert(C1);
		for(int j=0;j<NUM_CARDS;j++){//get the unique set of 3 cards to add to the 2 already held
			if(i&(1<<j))active.insert(this->showing[j]);
		}
		Psi cur_val=get_value_hand(active);//get the best value associated with this hand
		if(cur_val.first=="Error" || cur_val.second<0)return ANALYSIS_ERROR;//make sure we got a valid value for hand

		if(cur_val.second>best){//cache the best value and the hand which gave that value, there may be more than one hand which results in same high value, so it will cache the first
			Temp=make_pair(active,cur_val.first);
			best=cur_val.second;	
		}
	}

	if(Temp.first.size()==0 || Temp.second.length()==0)return ANALYSIS_ERROR;//no hand was picked, so error

	this->Ranking[best].insert(make_pair(name,Temp));//store the best value
	return NO_ERRORS;//all was ok
}

void TexasPoker::show_ranking(){//displays rankings, the player name associated with the ranking, and the hand type
	map<int,set<pair<string,pair<set<card>,string> > >,greater<int> >::iterator mit=this->Ranking.begin();//iterator declaration for ranking hash table(STD::Map<>)
	int rank=1;
	cout<<"\nHere is the current ranking:\n\n";
	for(;mit!=this->Ranking.end();mit++){
		if(mit->second.size()>1){//tie in value of hands
			for(set<pair<string,pair<set<card>,string> > >::iterator sit=mit->second.begin();sit!=mit->second.end();sit++){
				cout<<rank<<' '<<sit->first<<' '<<sit->second.second<<'\n';
				cout<<"Full Hand: \n";
				int cnt=0;
				for(set<card>::iterator sic=sit->second.first.begin();sic!=sit->second.first.end();++sic,++cnt){
					cout<<CARDNAMES[sic->value]<<" of "<<SUITS[sic->suit];
					if(cnt<4)cout<<',';
				}
				cout<<"\n\n";
			}
			rank++;
		}else{
			cout<<rank<<' '<<mit->second.begin()->first<<' '<<mit->second.begin()->second.second<<'\n';
			cout<<"Full Hand: ";
			int cnt=0;
			for(set<card>::iterator sic=mit->second.begin()->second.first.begin();sic!=mit->second.begin()->second.first.end();++sic,++cnt){
					cout<<CARDNAMES[sic->value]<<" of "<<SUITS[sic->suit];
					if(cnt<4)cout<<',';
				}
				cout<<"\n\n";
			rank++;
		}
	}
}


string get_error_string(const RETURN_CODE &TEMP){
	string ret;
	switch(TEMP){
		case INCORRECT_FORMAT:
			ret="Error #1: Incorrect Data Format\n";
			break;
		case DUPLICATE_NAME:
			ret="Error #2: Duplicate Player name(s)\n";
			break;
		case DUPLICATE_CARDS:
			ret="Error #3: Duplicate Cards\n";
			break;
		case ANALYSIS_ERROR:
			ret="Error #4: Hand Analysis Error\n";
			break;
		case FILE_NOT_FOUND:
			ret="Error #5: File name/location Error\n";
			break;
		case TOO_MANY_PLAYERS:
			ret="Error #6: Too many Players for single Deck\n";
			break;
		case NOT_ENOUGH_DATA:
			ret="Error #7: Not enough player or Deck Data\n";
			break;
		default:
			ret="";
	}
	return ret;
}

//extra utility functions used for testing//

string generate_test_set(int num_players){//will write to file a test set
	if(num_players>MAX_PLAYERS){
		cout<<"\nToo many players for one deck!\n";
		return "";
	}
	bool done[CARDS_IN_DECK]={0},doneNames[MAX_PLAYERS]={0};
	//first the 5 showing cards
	int c=0;
	string first="";
	while(c<NUM_CARDS){
		int cur_loc=rand()%CARDS_IN_DECK;
		if(done[cur_loc])continue;

		done[cur_loc]=true;
		if(first==""){
			first=DECK[cur_loc]+' ';
		}else{
			first+=DECK[cur_loc];
			if(c<(NUM_CARDS-1))first+=' ';
		}
		c++;
	}
	string filename="";
	cout<<"\nEnter in name of file for randomly generated data(.txt will be appened to end of file): ";
	do{
		getline(cin,filename);
	}while(filename=="");
	filename+=".txt";
	ofstream myfile;
	myfile.open(filename);
	myfile<<first<<'\n';
	for(int i=0;i<num_players;i++){
		int cur_name;
		string initial="";
		do{
			cur_name=rand()%MAX_PLAYERS;
		}while(doneNames[cur_name]);
		doneNames[cur_name]=true;

		initial=Names[cur_name]+' ';

		int cur_card;
		do{
			cur_card=rand()%CARDS_IN_DECK;
		}while(done[cur_card]);
		done[cur_card]=true;

		initial+=(DECK[cur_card]+' ');

		do{
			cur_card=rand()%CARDS_IN_DECK;
		}while(done[cur_card]);
		done[cur_card]=true;

		initial+=DECK[cur_card];

		myfile<<initial<<'\n';
	}

	myfile.close();
	cout<<"\nTest set "<<filename<<" has been generated and saved in the current working directory.\n";
	return filename;
}

void TexasPoker::display_start(){//show the intial hands
	cout<<"\nShowing initial cards: ";
	for(int i=0;i<NUM_CARDS;i++){
		cout<<CARDNAMES[this->showing[i].value]<<" of "<<SUITS[this->showing[i].suit]<<',';
	}
	cout<<'\n';
	cout<<"Showing each player's hands:\n";
	for(map<string,vector<card> >::iterator mit=this->initialDeal.begin();mit!=this->initialDeal.end();mit++){
		cout<<mit->first<<" has "<<CARDNAMES[mit->second[0].value]<<" of "<<SUITS[mit->second[0].suit]<<" and "<<CARDNAMES[mit->second[1].value]<<" of "<<SUITS[mit->second[1].suit]<<'\n';
	}
}

void view_data_in_file(const string &filename){
	ifstream readfile;
	readfile.open(filename);
	if(!readfile.good()){
		cout<<"\nError! Could not find file "<<filename<<" in the current working directory!\n";
		return;
	}
	string line;
	cout<<"\nHere are the contents of the file "<<filename<<'\n';
	while(getline(readfile,line)){
		cout<<line<<'\n';
	}
	cout<<'\n';
	readfile.close();
}


	