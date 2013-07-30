#ifndef MORPHEUS_TEXAS
#define MORPHEUS_TEXAS

//hard values for gameplay
#define CARDS_IN_DECK 52
#define DIF_CHARACTERS 15
#define DIF_SUITS 4
#define NUM_CARDS 5
#define MAX_PLAYERS 23
#define INF (1<<29)//for the highest hand value Royal Flush

//will be using std namespace
#ifndef USE_STD_NAMESPACE
#define USE_STD_NAMESPACE
#include <string>
#include <cstring>
using namespace std;
#endif

//const values used in code and for testing
const enum RETURN_CODE{NO_ERRORS, INCORRECT_FORMAT, DUPLICATE_NAME, DUPLICATE_CARDS, ANALYSIS_ERROR, 
	FILE_NOT_FOUND, TOO_MANY_PLAYERS,NOT_ENOUGH_DATA,DATA_ENTRY_ERROR};//Error codes which will determine type of error if any, starts at 0 which means no errors

const string DECK[CARDS_IN_DECK]={"2H", "2S", "2D", "2C", "3H", "3S", "3D", "3C", "4H", "4S", "4D", "4C", "5H", "5S",
	"5D", "5C", "6H", "6S", "6D", "6C", "7H", "7S", "7D", "7C", "8H", "8S", "8D", "8C", "9H", "9S", "9D", "9C", "TH", "TS", "TD",
	"TC", "JH", "JS", "JD", "JC", "QH", "QS", "QD", "QC", "KH", "KS", "KD", "KC", "AH", "AS", "AD", "AC"};//this should be obvious

const string SUITS[DIF_SUITS]={"Hearts","Spades","Diamonds","Clubs"};//ditto

const string CARDNAMES[DIF_CHARACTERS]={"0","1","2","3","4","5","6","7","8","9","Ten","Jack","Queen","King","Ace"};//The zero and one are there just to make the indexing easier

const string Names[MAX_PLAYERS]={"Bernette", "Juanita", "Judi", "Arnold", "Tom", "Sam", "Alicia", "Mark","John", "Pam",
	"Medusa", "Fred", "Albert", "Tim", "Joe", "Sally", "Lisa", "Ralph","Elric", "Arioch", "Frodo", "Luke", "Lisbeth"};//just random names for testing

//tyepdef declarations
typedef pair<string,int> Psi;

//inline countbit function used for determining optimal hand value
inline int cntbt(int n){n=n-((n>>1)&0x55555555);n=(n&0x33333333)+((n>>2)&0x33333333);return (((n+(n>>4))&0x0F0F0F0F)*0x01010101)>>24;}

//struct declarations

struct card{//card struct declaration
	int value,suit;
	card();
	card(const string &);
	bool operator<(const card &)const;
};

//declaration of main struct used in poker hand ranking
struct TexasPoker{
	//data structures used
	map<int,set<pair<string,pair<set<card>,string> > >,greater<int> > Ranking;//for sorting and ordering the final result rankings
	vector<card> showing;//discarded/used cards, and cards which are showing
	map<string,vector<card> > initialDeal;//caches the inital two hands dealt to each player by name
	set<card> used;//keeps track of each card dealt

	TexasPoker();//default constructor declaration

	//member function declarations
	void clear_data();//this is obvious
	RETURN_CODE is_valid_data(const string &);//utility to make sure card data is valid
	RETURN_CODE get_cards_showing(const string &);//this put fills the showing vector with the initial 5 cards
	void display_start();//will show the starting card layout, for testing purposes only
	Psi get_value_hand(const set<card> &);//get the value of the current possible hand
	RETURN_CODE get_best_hand_for_player(const string &);//gets best hand value of the current had under examination
	void show_ranking();//displays the final ranking of players	
};

//general utilities
string get_error_string(const RETURN_CODE &);//returns a the string which maps to the current error type if any
string generate_test_set(int);//declaration of testing function, which generates a valid random data set, saves it in current working directory, and returns the name of the file

#endif
