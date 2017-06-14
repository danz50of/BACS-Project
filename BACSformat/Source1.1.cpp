#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <time.h>
	using namespace std;
//	Set MAXLEN to a size which will accomidate the maximum amount of BACS lines
//	in a payment run
	const int MAXLEN = 1000;
/****************************************************************************************
 *																						*
 *  Program:  BACSformat																*
 *  Author:  Dan Zabinski																*
 *  Purpose:  When processing files using the BACS payment method IFS is missing some   *
 *		      some static data.  This program will add the values for Peerless's        *
 *            banking sort code and bank account number to the BACS file which will     *
 *            then be uploaded to the bank.                                             *
 *                                                                                      *
 * Pre-Conditions:  A file must exist in c:\BACS on the computer of the person running  *
 *                  the program.                                                        *
 *                                                                                      *
 * Post-Conditions:  The program will write a file, inserting the static data to the    *
 *                   same c:\BACS folder location on the users computer                 *
 *                                                                                      *
 * File Format Example:                                                                 *
 *FH,PEERLESSIN,20140108,142000,01100,,,,,,,,,,,,,,,,,,,,,,,
 *TR,TEST4,20140108,GB,,123456,45678901,0,331500,GBP,GIR,01,99,0041001352,,,Hamlin Knight Ltd,,,,,,,,,,,TEST4
 *TR,TEST2,20140108,GB,,123456,23456789,0,300000,GBP,GIR,01,99,0041001352,,,CCS MEDIA LIMITED,,,,,,,,,,,TEST2
 *TR,TEST1,20140108,GB,,123456,12345678,0,270000,GBP,GIR,01,99,0041001352,,,ANGLIA FORWARDING LIMITED,,,,,,,,,,,TEST1
 *TR,TEST3,20140108,GB,,123456,34567890,0,290000,GBP,GIR,01,99,0041001352,,,ECOPAC LTD,,,,,,,,,,,TEST3
 *FT,4,6,1191500,,,,,,,,,,,,,,,,,,,,,,,,                                                *
 ****************************************************************************************
 ****************************************************************************************
 *                      Version History                                                 *
 *                      Change Log                                                      *
 * Version 1.0                             Initial development / first prod release     *
 *                                                                                      *
 * 12/23/2013          DZ                    Initial create of program to meet BACS     *
 *                                           specifications.                            *
 *                                                                                      *
 * 1/8/2014			   DZ					 Began adding file header logic             *
 *                                           added class FileTime to automatically add  *
 *                                           the date of the file run to the header     *
 *                                           record.                                    *
 *																						*
 * Version 1.1                                                                          *
 * 4/14/2014		   DZ					 Modify program to allow user to enter date *
 *                                           at run time and use date in header file and*
 *                                           detail lines.                              *
 *                                                                                      *
 * Version 1.2		   DZ					 Updated per UK request to change acc #     *
 ***************************************************************************************/

//Class BACS_FORMAT ***   Used to define the information required for JP morgan bacs processing
	class BACS_FORMAT
	{
	public:
		BACS_FORMAT();
		char BACS_OUT[80];
		void populate(char input[], string date_in);
		int getAmount();
		void setCurrency(string in_cur);
		void setDate(string date_in);
		string getDate();
		friend ostream& operator <<(ostream& out, const BACS_FORMAT& output);
	private:
		char BACS_IN[80];
		char dest_sort[10];
		char dest_num[25];
		char UK_sort[9];
		char UK_Account[15];
		char Amount[20];
		char user_name[100];
		char user_ref[200];
		char dest_account_name[30];
		char currency[4];
		char date[9];
		
	};
	class FileTime
	{
	public:
		FileTime();
		string getDate();
	private:
		int day,
			month,
			year;
	};
void BACSFileHeader(ofstream& BACS_OUT);
void BACSFileTrailer(ofstream& BACS_OUT, int line_num, int file_total);
int GetFile(ifstream& infile, string file_in[]);

//DZ 4/14/2014 ** Removed file time which retrieves the current date **
//             ** and sets the transaction lines to the current date **
//FileTime getTheDate;
//string TODAY = getTheDate.getDate();
void PrintWelcome();
void printClose();

int main()
{

	ifstream BACS_IN;
	ofstream BACS_OUT;
	char input[100];
	string file_in[MAXLEN];
	string date_in;
	int line_num, file_total;
	int i=0, MAX;
	BACS_FORMAT bacs[MAXLEN];
	char cur_selection;
	bool cur_valid;
	char temp_date[9];

	

//Open Input Files

	PrintWelcome();


	cur_valid = false;

	while(!cur_valid)
	{

		cout << "Please enter the date you would like on the bank file\n"
			 << "***Enter as: yyyymmdd: ";
		cin >> date_in;
		cout << "\n\n Please select from the following Currency Options: ";
		cout << "\n 1.) Enter 1 for GBP";
		cout << "\n 2.) Enter 2 for EUR";
		cout  << "\n  :Enter option: ";
		cin >> cur_selection;

		if(cur_selection == '1')
			cur_valid = true;
		else if (cur_selection == '2')
			cur_valid = true;
		else
			cur_valid = false;

	} 
		

	BACS_IN.open("EPAY.DAT");
	if (BACS_IN.fail())
	{
		cout << "File not found in same folder as program has been"
			 << "run out of.  Please try again, program will exit.";
		cin.get();
		cin.ignore();
		exit(1);
	}
	BACS_OUT.open("ECONVERT.CSV");
	if (BACS_OUT.fail())
		exit(2);

	MAX = GetFile(BACS_IN, file_in);

	for(i=0; i < MAX; i++)
	{
		strcpy(input, file_in[i].c_str());
		bacs[i].populate(input, date_in);

		// set currency for output

		if(cur_selection == '1')
			bacs[i].setCurrency("GBP");
		else if(cur_selection == '2')
			bacs[i].setCurrency("EUR");


	}
	MAX = i;

	line_num = i;  //Add 1 because the array's start at position 0
	file_total = 0;

	//strcpy(temp_date,date_in.c_str());

	BACSFileHeader(BACS_OUT);



	for (i = 0; i < MAX; i++)
	{
		BACS_OUT << bacs[i];
		file_total += bacs[i].getAmount();
	}


	BACSFileTrailer(BACS_OUT, line_num, file_total);

//Close Files At end of Program
	BACS_IN.close();
	BACS_OUT.close();

	printClose();


	
}

BACS_FORMAT::BACS_FORMAT()
{
	/*************************************************************************************
	 *                                                                                   *
	 *  In this routine, a hard coded sort code and bank account number will be added    *
	 *  to create the standard BACS file.  Please Update sort and account code with the  *
	 *  correct values for your application.                                             *
	 *																				     *
	 *************************************************************************************/
	strcpy_s(UK_sort, "########");      //Populate with the correct banking sort code
	strcpy_s(UK_Account, "##########");   //Populate with the correct account number to use

}

void BACS_FORMAT::populate(char input[], string date_in)
{
	string temp;
	//char date_temp[9];
	int index, prev;
	string mon, day, year;

	//Put date in date field
	strcpy(date, date_in.c_str());
	

	//Process Sort Code
	temp = input;
	index = temp.find(",", 0);
	strncpy_s(dest_sort, input, index);
	dest_sort[index] = '\0';
//	cout << "dest_sort" << dest_sort << endl;

	//Process Dest_account Number

	prev = index+1;
	index = temp.find(",", index+1);
	strncpy_s(dest_num, input+prev, index-prev);
	dest_num[index-prev] = '\0';
//	cout << "dest_num:" << dest_num << endl;

	//Process amount

	prev = index+1;
	index = temp.find(",", index+1);
	strncpy_s(Amount, input+prev, index-prev);
	Amount[index-prev] = '\0';
//	cout << "amount:" << Amount << endl;

	//Process user name

	prev = index+1;
	index = temp.find(",", index+1);
	strncpy_s(user_name, input+prev, index-prev);
	user_name[index-prev] = '\0';
//	cout << "User Name:" << user_name << endl;

	//Process space holder

	prev = index+1;
	index = temp.find(",", index+1);
/*	strncpy(user_ref, input+prev, index-prev);
	user_ref[index-prev] = '\0';
	cout << "User Ref:" << user_ref << endl;*/

	//Process user reference

	prev = index+1;
	index = temp.find(",", index+1);
	strncpy_s(user_ref, input+prev, index-prev);
	user_ref[index-prev] = '\0';
//	cout << "User Ref:" << user_ref << endl;

	//Dest Account Name

	prev = index+1;
//	index = temp.find("\n", index+1);
	strncpy_s(dest_account_name, input+prev, temp.length()-prev);
	dest_account_name[temp.length()-prev] = '\0';
//	cout << "dest_account_name:" << dest_account_name << endl;

}

ostream& operator <<(ostream& out, const BACS_FORMAT& bacs)
{
		out << "TR,"
		    << bacs.dest_account_name << "," ;
//		cout << "\ndate entered on this line: " << bacs.date << "\n";     //Debug statement
		out	<< bacs.date
			<< ",GB,,"
			<< bacs.dest_sort << ","
		    << bacs.dest_num  << ","
//		    << bacs.UK_sort  << ","
            << "0,"
		    << bacs.Amount  << ","
			<< bacs.currency << ","
			<< "GIR,01,99,"
		    << bacs.UK_Account  << ",,,"
		    << bacs.user_name  << ",,,,,,,,,,,"
			<< bacs.user_ref  //tRANSACTION REFERENCE
			<< "-Company reference info"
			<< "\n";

		return out;
}

void BACSFileHeader(ofstream& BACS_OUT)
{

	FileTime date1;
	string print_date;
	BACS_OUT << "additional info,";  //    9/12/2014 comment out date entered and use current date for header line
//	print_date = date. .getDate();
	BACS_OUT << date1.getDate();
	BACS_OUT << ",142000,01100,,,,,,,,,,,,,,,,,,,,,,,," << endl;

}
void BACSFileTrailer(ofstream& BACS_OUT, int line_num, int file_total)
{

	BACS_OUT << "FT," << line_num << "," << line_num + 2 << "," << file_total;
	BACS_OUT << ",,,,,,,,,,,,,,,,,,,,,,,,,";

}

FileTime::FileTime()
{

time_t theTime = time(NULL);
struct tm *aTime = localtime(&theTime);

day = aTime->tm_mday;
month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
year = aTime->tm_year + 1900; // Year is # years since 1900


}

string FileTime::getDate()
{
	string tempDate;

	tempDate = to_string(year);
	if(month < 10)
		tempDate += "0";
	tempDate += to_string(month);
	if(day < 10)
		tempDate += "0";
	tempDate += to_string(day);

	return(tempDate);
}

int BACS_FORMAT::getAmount()
{
	return atoi(Amount);
}
int GetFile(ifstream& infile, string file_in[])
{

	int i = 0;

	getline(infile, file_in[i]);


	while (!infile.eof())
	{
		i++;
		getline(infile, file_in[i]);


	}

	return(i);

}

void PrintWelcome()
{
	cout << "*************************************************\n"
		 << "* Welcome to BACS File Format Program           *\n"
		 << "* Version: 1.1                                  *\n"
		 << "* Date Created: 4/14/2014                       *\n"
		 << "* Description:                                  *\n"
		 << "*     Program will convert an IFS generated BACS*\n"
		 << "*     file into a format that will be accepted  *\n"
		 << "*     by JP Morgan Chase Banking system.        *\n"
		 << "*                                               *\n"
		 << "*************************************************\n"
		 << "\n\n\n";
	cout << "The file you wish to format must be located on the local machine\n"
		 << "and must have the following name: \n"
		 << "c:\bacs\bacs.dat\n\n";

	return;

}

void printClose()
{
	cout << "\n\nProgram finished successfully.  Please review file c:\\bacs\\econvert.dat\n\n";
	cout << "press any key to exit";

	cin.get();
	cin.ignore();

	return;
}

void BACS_FORMAT::setCurrency(string in_cur)
{
	// change to copy in currency because it is c-string var
	strcpy(currency, in_cur.c_str());
	return;
}

void BACS_FORMAT::setDate(string date_in)
{

}
string BACS_FORMAT::getDate()
{
	return date;
}