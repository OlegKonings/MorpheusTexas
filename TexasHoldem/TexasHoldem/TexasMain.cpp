//Code by Oleg Konings 7/27/2013
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <utility>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <map>
#include <set>
#include <ctime>
#include <cassert>
#include "deck.h"
using namespace std;

int main(int argc, char **argv){
	
	srand(time(NULL));//for random number generation

	RETURN_CODE RET=NO_ERRORS;//this will reflect the current error state of the application
	TexasPoker CurGame;//instance of the TexasPoker struct
	bool ok=true;
	int line_num=0;

	if(argc<=1){//if there are no additional command line arguments it will default to using data entered by user
		string datain;
		cout<<"\nEnter in data. First the showing cards, then the player name and their repective two cards(follow given example format rules). Limit 23 players per single deck.\n";
		cout<<"\nEnter 0 to end input:\n";
		while(getline(cin,datain) && ok){
			if(datain=="0")break;//signifys the end of console data entry
			if(line_num==0){//the first 5 cards
				RET=CurGame.get_cards_showing(datain);
				if(RET!=NO_ERRORS){
					cout<<get_error_string(RET);
					ok=false;
				}
			}else{//players
				RET=CurGame.get_best_hand_for_player(datain);
				if(RET!=NO_ERRORS){
					cout<<get_error_string(RET);
					ok=false;
				}
			}
			line_num++;
			if(line_num>(MAX_PLAYERS+1) ){//error checking for too many players
				RET=TOO_MANY_PLAYERS;
				cout<<get_error_string(RET);
				ok=false;
			}
		}
		if(RET==NO_ERRORS && ok && line_num>1){//all was error-free, so show ranking list
			CurGame.show_ranking();
		}else{
			if(RET!=NO_ERRORS){
				cout<<get_error_string(RET);
			}else{//means that the data entry had a problem(like an empty line)
				RET=DATA_ENTRY_ERROR;
				cout<<get_error_string(RET);
			}
		}
	}else if(argc>1){//read from file
		ifstream testfile;
		testfile.open(argv[1],ios::in);
		string line;

		if(testfile.is_open()){
			while(getline(testfile,line) && ok){
				if(line_num==0){
					RET=CurGame.get_cards_showing(line);//get the first five cards and verify correct
					if(RET!=NO_ERRORS){//if something goes wrong there will be a somewhat specific error message
						cout<<get_error_string(RET);
						ok=false;
					}
				}else{
					RET=CurGame.get_best_hand_for_player(line);//get the players
					if(RET!=NO_ERRORS){
						cout<<get_error_string(RET);
						ok=false;
					}
				}
				line_num++;
				if(line_num>(MAX_PLAYERS+1) ){
					RET=TOO_MANY_PLAYERS;
					cout<<get_error_string(RET);
					ok=false;
				}
			}
			testfile.close();
			if(ok && RET==NO_ERRORS && line_num>0){
				CurGame.show_ranking();
			}else if(ok && RET==NO_ERRORS){//means no player data entered
				RET=NOT_ENOUGH_DATA;
				cout<<get_error_string(RET);
				ok=false;
			}

		}else{//means it could not find the file with that name in the current directory
			RET=FILE_NOT_FOUND;
			cout<<get_error_string(RET);
			ok=false;
		}
	}


	char ch;
	cout<<"\nEnter in any key to exit:";
	cin>>ch;
	return 0;
}