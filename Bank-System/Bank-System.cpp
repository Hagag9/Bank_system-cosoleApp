
#include <iostream>
#include <vector>
#include<fstream>
#include<string>
#include<cctype>
#include<iomanip>
using namespace std;
// Bank Project extenssion
const string ClientsFileName = "client.txt";
const string UsersFileName = "Users.txt";
enum enOptions { Show = 1, Add = 2, Delete = 3, update = 4, Find = 5, transaction = 6, ManageUser = 7, Logout = 8 };
enum enTransaction { Deposit = 1, Withdraw = 2, Totalbalance, MainMenue = 4 };
enum enManageUser { listuser = 1, addUser = 2, deletUser = 3, updateUser = 4, findUser = 5, mainMenue = 6 };
enum enMainMenuePermession { eAll = -1, eShowList = 1, eAdd = 2, eDelet = 4, eUpdate = 8, eFind = 16, eTransaction = 32, eManage = 64 };
struct StclientData
{
	string AcountNum;
	string  pinCode;
	string Name;
	string Phone;
	int Acountbalance = 0;
	bool MarkForDelete = false;
};
struct StUser
{
	string UserName;
	string Password;
	int Permession = 0;
	bool MarkForDelete = false;
};
StUser CurrentUser;      // global vaiable for current user
vector < StclientData> LoadClientsFromFile(string FileName);
vector < StUser> LoadUserFromFile(string FileName);
bool FindByAcountNumber(string FileName, string AccountNumber, StclientData& client);
bool FindUserByName(string FileName, string Name, StUser& user);
StclientData ConvertLineToRecord(string line, string Seperator);
StUser ConvertUserLineToRecord(string line, string Seperator);
string convertRecordToLine(StclientData client, string sperator);
string convertRecordUserToLine(StUser user, string sperator);
void PerformMainMenueOption(string FileName);
void PerformTransActionMenueOption(string FileName);
void ShowMainMenue(string FileName);
void ShowTransActionMenue(string FileName);
void LogIn();
void ManageUserMenue();
void PerformManageMenue(string);

StclientData ReaNewClient(string FileName)
{
	vector< StclientData> Vclient = LoadClientsFromFile(FileName);
	StclientData client;
	cout << "Add New client :\n";
	cout << "Enter account number ? ";
	getline(cin >> ws, client.AcountNum);
	while (FindByAcountNumber(FileName, client.AcountNum, client))
	{
		cout << "Clint with [" << client.AcountNum << "] already exists , Enter another account number ? ";
		getline(cin >> ws, client.AcountNum);
	}
	cout << "Enter pin code  ? ";
	getline(cin, client.pinCode);
	cout << "Enter name ? ";
	getline(cin, client.Name);
	cout << "Enter phone number ? ";
	getline(cin, client.Phone);
	cout << "Enter account Balance ? ";
	cin >> client.Acountbalance;
	return client;
}
int ReadPermessionTest()
{
	int Permession = 0;
	char Answer = 'n';
	cout << "\nDo you want to give full access ? y / n ? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		return -1;
	cout << "\nDo you want to give access to : \n ";
	cout << "\nShow Client List? y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eShowList;
	cout << "\n\nAdd New Client?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eAdd;
	cout << "\n\nDelete Client?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eDelet;
	cout << "\n\nUpdate Client?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eUpdate;
	cout << "\n\nFind Client?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eFind;
	cout << "\n\nTransaction?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eTransaction;
	cout << "\n\nManage User?  y/n? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
		Permession += enMainMenuePermession::eManage;

	return Permession;

}
bool CheckAccessPermission(enMainMenuePermession Permession)
{
	if (CurrentUser.Permession == enMainMenuePermession::eAll)
		return true;
	if ((CurrentUser.Permession & Permession) == Permession)
		return true;
	else
		return false;
}
void ShowAccessDeniedMessage()
{
	cout << "\n------------------------------------\n";
	cout << "Access Denied, \nYou dont Have Permission To Do this,\nPlease Conact Your Admin.";
	cout << "\n------------------------------------\n";
}

StUser ReadNewUser(string FileName)
{
	vector< StUser> Vclient = LoadUserFromFile(FileName);
	StUser user;
	cout << "Add New user :\n";
	cout << "Enter user name ? ";
	getline(cin >> ws, user.UserName);
	while (FindUserByName(FileName, user.UserName, user))
	{
		cout << "User with [" << user.UserName << "] already exists , Enter another user name ? ";
		getline(cin >> ws, user.UserName);
	}
	cout << "Enter password  ? ";
	cin >> user.Password;
	cout << "Enter name ? ";
	user.Permession = ReadPermessionTest();

	return user;
}
int ReadNumber(string message)
{
	int number;
	cout << message;
	cin >> number;
	return number;
	while (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid Number, Enter a valid one: ";
		cin >> number;
	}
}
vector <string>Split(string S, string delim)
{
	vector <string> Vwords;
	short pos = 0;
	string S1;
	while ((pos = S.find(delim)) != S.npos)
	{
		S1 = S.substr(0, pos);
		if (S1 != "")
		{
			Vwords.push_back(S1);
		}
		S.erase(0, pos + delim.length());
	}
	if (S != "")
	{
		Vwords.push_back(S);
	}
	return Vwords;
}

StclientData ConvertLineToRecord(string line, string Seperator)
{
	vector<string>Vwords = Split(line, Seperator);
	StclientData StClient;
	StClient.AcountNum = Vwords[0];
	StClient.pinCode = Vwords[1];
	StClient.Name = Vwords[2];
	StClient.Phone = Vwords[3];
	StClient.Acountbalance = stoi(Vwords[4]);

	return StClient;
}
StUser ConvertUserLineToRecord(string line, string Seperator)
{
	vector<string>Vwords = Split(line, Seperator);
	StUser User;
	User.UserName = Vwords[0];
	User.Password = Vwords[1];
	User.Permession = stoi(Vwords[2]);
	return User;
}
vector < StclientData> LoadClientsFromFile(string FileName)
{
	vector < StclientData> Vclients;
	fstream MyFile;
	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		string line;
		while (getline(MyFile, line))
		{
			Vclients.push_back(ConvertLineToRecord(line, "#//#"));
		}
		MyFile.close();
	}
	return Vclients;
}
vector < StUser> LoadUserFromFile(string FileName)
{
	vector < StUser> Vclients;
	fstream MyFile;
	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		string line;
		while (getline(MyFile, line))
		{
			Vclients.push_back(ConvertUserLineToRecord(line, "#//#"));
		}
		MyFile.close();
	}
	return Vclients;
}
vector <StclientData> SaveClientDataToFile(string FileName, vector <StclientData>& Vclient)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	if (MyFile.is_open())
	{
		string line;
		for (StclientData& c : Vclient)
		{
			if (!(c.MarkForDelete))
			{
				line = convertRecordToLine(c, "#//#");
				MyFile << line << endl;
			}
		}
		MyFile.close();
	}
	return Vclient;
}
vector <StUser> SaveUserDataToFile(string FileName, vector <StUser>& Vclient)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	if (MyFile.is_open())
	{
		string line;
		for (StUser& c : Vclient)
		{
			if (!(c.MarkForDelete))
			{
				line = convertRecordUserToLine(c, "#//#");
				MyFile << line << endl;
			}
		}
		MyFile.close();
	}
	return Vclient;
}


string ReadClientAccountNumber()
{
	string AccountNumber = "";
	cout << "\nPlease enter AccountNumber? ";
	cin >> AccountNumber;
	return AccountNumber;
}
string ReadUserName()
{
	string UserName = "";
	cout << "\nPlease enter User Name? ";
	cin >> UserName;
	return UserName;
}
bool FindByAcountNumber(string FileName, string AccountNumber, StclientData& client)
{
	vector < StclientData> Vclients = LoadClientsFromFile(FileName);
	for (StclientData c : Vclients)
	{
		if (c.AcountNum == AccountNumber)
		{
			client = c;
			return true;
		}
	}
	return false;
}
bool FindUserByName(string FileName, string Name, StUser& user)
{
	vector < StUser> Vclients = LoadUserFromFile(FileName);
	for (StUser& c : Vclients)
	{
		if (c.UserName == Name)
		{
			user = c;
			return true;
		}
	}
	return false;
}

void PrintClientRecord(StclientData Client)
{
	cout << "|" << setw(15) << left << Client.AcountNum
		<< "|" << setw(10) << left << Client.pinCode
		<< "|" << setw(20) << left << Client.Name
		<< "|" << setw(12) << left << Client.Phone
		<< "|" << setw(12) << left << Client.Acountbalance;
}
void PrintUserRecord(StUser User)
{
	cout << "|" << setw(15) << left << User.UserName
		<< "|" << setw(20) << left << User.Password
		<< "|" << setw(12) << left << User.Permession;
}

void PrintClientBalance(StclientData Client)
{
	cout << "|" << setw(15) << left << Client.AcountNum
		<< "|" << setw(20) << left << Client.Name
		<< "|" << setw(12) << left << Client.Acountbalance;
}
void PrintUserCard(StUser User)
{
	cout << "the follwing are the User detils\n\n";
	cout << "User Name      : " << User.UserName << endl;
	cout << "User Password  : " << User.Password << endl;
	cout << "User Permession: " << User.Permession << endl;
}
void PrintClientCard(StclientData StClient)
{
	cout << "the follwing are the Client detils\n\n";
	cout << "Acount Number  : " << StClient.AcountNum << endl;
	cout << "Pin code       : " << StClient.pinCode << endl;
	cout << "Name           : " << StClient.Name << endl;
	cout << "Phone          : " << StClient.Phone << endl;
	cout << "Account balance: " << StClient.Acountbalance << endl;
}

void ShowClientList(string FileName)
{
	if (!CheckAccessPermission(enMainMenuePermession::eShowList))
	{
		ShowAccessDeniedMessage();
		return;
	}
	vector< StclientData> Vclient = LoadClientsFromFile(FileName);
	cout << "\t\t\tClient List (" << Vclient.size() << ") client (s)\n";
	cout << "-------------------------------------------------------------------------------\n";
	cout << "|" << setw(15) << left << "Acount Number" << "|" << setw(10) << left << "Pin code"
		<< "|" << setw(20) << left << "Name" << "|" << setw(12) << left << "Phone"
		<< "|" << setw(12) << left << "Acount balance" << endl;
	cout << "-------------------------------------------------------------------------------\n";
	for (StclientData client : Vclient)
	{
		PrintClientRecord(client);
		cout << endl;
	}
	cout << "-------------------------------------------------------------------------------\n";

}
void ShowUsersList(string FileName)
{
	vector< StUser> Vclient = LoadUserFromFile(FileName);
	cout << "\t\t\tUsers List (" << Vclient.size() << ") User (s)\n";
	cout << "----------------------------------------------------------------\n";
	cout << "|" << setw(15) << left << "User Name"
		<< "|" << setw(20) << left << "Password"
		<< "|" << setw(12) << left << "Permissiois" << endl;
	cout << "----------------------------------------------------------------\n";
	for (StUser User : Vclient)
	{
		PrintUserRecord(User);
		cout << endl;
	}
	cout << "-----------------------------------------------------------------\n";
}
string convertRecordToLine(StclientData client, string sperator)
{
	string clientRecord = "";
	clientRecord += client.AcountNum + sperator;
	clientRecord += client.pinCode + sperator;
	clientRecord += client.Name + sperator;
	clientRecord += client.Phone + sperator;
	clientRecord += to_string(client.Acountbalance);
	return clientRecord;
}
string convertRecordUserToLine(StUser user, string sperator)
{
	string userRecord = "";
	userRecord += user.UserName + sperator;
	userRecord += user.Password + sperator;
	userRecord += to_string(user.Permession);
	return userRecord;
}
void AddlineToFile(string FileName, string line)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out | ios::app);
	if (MyFile.is_open())
	{
		MyFile << line << endl;
		MyFile.close();
	}
}
void AddNewClient(string FileName)
{
	StclientData NewClinet = ReaNewClient(FileName);
	AddlineToFile(FileName, convertRecordToLine(NewClinet, "#//#"));
}
void AddNewUser(string FileName)
{
	StUser NewUser = ReadNewUser(FileName);
	AddlineToFile(FileName, convertRecordUserToLine(NewUser, "#//#"));
}

void AddClients(string FileName)
{
	if (!CheckAccessPermission(enMainMenuePermession::eAdd))
	{
		ShowAccessDeniedMessage();
		return;
	}
	char AddMore = 'Y';
	do
	{
		system("cls");
		cout << "------------------------------------------------------\n";
		cout << "\t\t Add New Client Screen\n";
		cout << "------------------------------------------------------\n";
		AddNewClient(FileName);
		cout << "Add seccessfully , do you want add more clients ? Y/N ? ";
		cin >> AddMore;

	} while (toupper(AddMore) == 'Y');
}
void AddUsers(string FileName)
{

	char AddMore = 'Y';
	do
	{
		system("cls");
		cout << "------------------------------------------------------\n";
		cout << "\t\t Add New User Screen\n";
		cout << "------------------------------------------------------\n";
		AddNewUser(FileName);
		cout << "Add seccessfully , do you want add more Users ? Y/N ? ";
		cin >> AddMore;

	} while (toupper(AddMore) == 'Y');
}

bool MarkClientForDeleteByAccountNumber(string accountnumber, vector <StclientData>& Vclient)
{
	for (StclientData& client : Vclient)
	{
		if (client.AcountNum == accountnumber)
		{
			client.MarkForDelete = true;
			return true;
		}
	}
	return false;
}
bool MarkUserForDeleteByAccountNumber(string username, vector <StUser>& Vclient)
{
	for (StUser& user : Vclient)
	{
		if (user.UserName == username)
		{
			user.MarkForDelete = true;
			return true;
		}
	}
	return false;
}
bool  DeleteClientByAccountNumber(string FileName)
{

	string accountnumber = ReadClientAccountNumber();
	vector <StclientData> Vclient = LoadClientsFromFile(FileName);
	StclientData client;
	char Answer = 'n';
	if (FindByAcountNumber(FileName, accountnumber, client))
	{
		PrintClientCard(client);
		cout << "\n\nAre you sure you want delete this client? y/n ? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			MarkClientForDeleteByAccountNumber(accountnumber, Vclient);
			SaveClientDataToFile(ClientsFileName, Vclient);
			Vclient = LoadClientsFromFile(FileName);
			cout << "\n\nClient Deleted Successfully.";
			return true;
		}
	}
	else
	{
		cout << "\nClient with Account Number (" << accountnumber << ") is Not Found!";
		return false;
	}
}
void ShowDeleteClientScreen()
{

	if (!CheckAccessPermission(enMainMenuePermession::eDelet))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n-----------------------------------\n";
	cout << "\tDelete Client Screen";
	cout << "\n-----------------------------------\n";

	DeleteClientByAccountNumber(ClientsFileName);
}
bool  DeleteUser(string FileName)
{
	string UserName = ReadUserName();
	if (UserName == "Admin")
	{
		cout << "\n\nYou cannot Delete This User.";
		return false;
	}
	vector <StUser> Vclient = LoadUserFromFile(FileName);
	StUser client;
	char Answer = 'n';
	if (FindUserByName(FileName, UserName, client))
	{

		PrintUserCard(client);
		cout << "\n\nAre you sure you want delete this User? y/n ? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			MarkUserForDeleteByAccountNumber(UserName, Vclient);
			SaveUserDataToFile(ClientsFileName, Vclient);
			Vclient = LoadUserFromFile(FileName);
			cout << "\n\nUser Deleted Successfully.";
			return true;
		}
	}
	else
	{
		cout << "\nUser with Name (" << UserName << ") is Not Found!";
		return false;
	}
}
void ShowDeleteUserScreen()
{
	cout << "\n-----------------------------------\n";
	cout << "\tDelete Users Screen";
	cout << "\n-----------------------------------\n";
	DeleteUser(UsersFileName);

}

StclientData ChangeClientRecord(string accountnumber)
{
	StclientData client;

	client.AcountNum = accountnumber;
	cout << "Enter pin code  ? ";
	getline(cin >> ws, client.pinCode);
	cout << "Enter name ? ";
	getline(cin, client.Name);
	cout << "Enter phone number ? ";
	getline(cin, client.Phone);
	cout << "Enter account Balance ? ";
	cin >> client.Acountbalance;

	return client;
}
StUser ChangeUserRecord(string username)
{
	StUser user;
	cout << "Enter new username ? ";
	getline(cin >> ws, user.UserName);
	cout << "Enter New password ? ";
	cin >> user.Password;
	cout << "Enter permission ? ";
	cin >> user.Permession;

	return user;
}
bool  UpdateClientByAccountNumber(string FileName)
{
	string accountnumber = ReadClientAccountNumber();
	vector <StclientData> Vclient = LoadClientsFromFile(FileName);
	StclientData client;
	char Answer = 'n';
	if (FindByAcountNumber(FileName, accountnumber, client))
	{
		PrintClientCard(client);
		cout << "\n\nAre you sure you want update this client? y/n ? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			for (StclientData& c : Vclient)
			{
				if (c.AcountNum == accountnumber)
				{
					c = ChangeClientRecord(accountnumber);
					break;
				}
			}
			SaveClientDataToFile(ClientsFileName, Vclient);
			cout << "\n\nClient updated Successfully.";
			return true;
		}
	}
	else
	{
		cout << "\nClient with Account Number (" << accountnumber << ") is Not Found!";
		return false;
	}
}
void ShowUpdateClientScreen()
{

	if (!CheckAccessPermission(enMainMenuePermession::eUpdate))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n-----------------------------------\n";
	cout << "\tUpdate Client Info Screen";
	cout << "\n-----------------------------------\n";
	UpdateClientByAccountNumber(ClientsFileName);
}
bool  UpdateUser(string FileName)
{
	string UserName = ReadUserName();
	vector <StUser> Vclient = LoadUserFromFile(FileName);
	StUser user;
	char Answer = 'n';
	if (FindUserByName(FileName, UserName, user))
	{
		PrintUserCard(user);
		cout << "\n\nAre you sure you want update this user? y/n ? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			for (StUser& c : Vclient)
			{
				if (c.UserName == UserName)
				{
					c = ChangeUserRecord(UserName);
					break;
				}
			}
			SaveUserDataToFile(ClientsFileName, Vclient);
			cout << "\n\nUser updated Successfully.";
			return true;
		}
		else return false;
	}
	else
	{
		cout << "\nUser with Name (" << UserName << ") is Not Found!";
		return false;
	}
}
void ShowUpdateUserScreen()
{
	cout << "\n-----------------------------------\n";
	cout << "\tUpdate Users Screen";
	cout << "\n-----------------------------------\n";
	UpdateUser(UsersFileName);
}
void FindClient(string FileName)
{
	if (!CheckAccessPermission(enMainMenuePermession::eFind))
	{
		ShowAccessDeniedMessage();
		return;
	}
	string accountnumber = ReadClientAccountNumber();
	StclientData client;
	if (FindByAcountNumber(FileName, accountnumber, client))
	{
		PrintClientCard(client);
	}
	else
		cout << "\nClient with Account Number (" << accountnumber << ") is Not Found!";
}
void FindUser(string FileName)
{
	string UserName = ReadUserName();
	StUser User;
	if (FindUserByName(FileName, UserName, User))
	{
		PrintUserCard(User);
	}
	else
		cout << "\nUser with Name (" << UserName << ") is Not Found!";
}

bool FindUserByNamePassword(string username, string password, StUser& user)
{
	vector < StUser> VUsers = LoadUserFromFile(UsersFileName);
	for (StUser U : VUsers)
	{
		if (U.UserName == username && U.Password == password)
		{
			user = U;
			return true;
		}
	}
	return false;
}
bool LoadUserInfo(string UserName, string Password)
{
	if (FindUserByNamePassword(UserName, Password, CurrentUser))
		return true;
	else
		return false;
}
void LogOut()
{
	system("cls");
	LogIn();
}
void DrwaMainMeue()
{
	system("cls");
	cout << "=======================================\n";
	cout << "\t\tMain menue screen \n";
	cout << "=======================================\n";
	cout << "\t[1] Show client list\n";
	cout << "\t[2] Add new client \n";
	cout << "\t[3] Delete client\n";
	cout << "\t[4] Update client info\n";
	cout << "\t[5] Find client \n";
	cout << "\t[6] Transaction \n";
	cout << "\t[7] Manage User\n";
	cout << "\t[8] log Out\n";
	cout << "=======================================\n";

}
enOptions SelectMainMenueOption()
{
	int option;
	cout << "What do you want to do ? [1 to 8 ]?\n";
	cin >> option;
	return enOptions(option);
}
enTransaction SelectTransactionOption()
{
	int option;
	cout << "What do you want to do ? [1 to 4 ]?\n";
	cin >> option;
	return enTransaction(option);
}
enManageUser SelectManageOption()
{
	int option;
	cout << "What do you want to do ? [1 to 6 ]?\n";
	cin >> option;
	return enManageUser(option);
}
void GoBackToMainMenue()
{
	cout << "\n\n\n\n Press any key to go back to main menu ..... ";
	system("pause>0");
	system("cls");
	ShowMainMenue(ClientsFileName);
}

void GoBackToTransactionMenu()
{
	cout << "\n\n\n\n Press any key to go back to Transaction Menue ..... ";
	system("pause>0");
	system("cls");
	ShowTransActionMenue(ClientsFileName);
}
void GoBackToManageUserMenu()
{
	cout << "\n\n\n\n Press any key to go back to Manage user Menue ..... ";
	system("pause>0");
	system("cls");
	ManageUserMenue();
}

void TransactionMenue()
{
	system("cls");
	cout << "=======================================\n";
	cout << "\t\tTransaction Menue Screen \n";
	cout << "=======================================\n";
	cout << "\t[1] Deposite\n";
	cout << "\t[2] Withdraw \n";
	cout << "\t[3] Total Balances\n";
	cout << "\t[4] Main menue\n";
	cout << "=======================================\n";
}
void ManageUserMenue()
{
	if (!CheckAccessPermission(enMainMenuePermession::eManage))
	{
		ShowAccessDeniedMessage();
		return;
	}
	system("cls");
	cout << "=======================================\n";
	cout << "\t\tManage User Menue Screen \n";
	cout << "=======================================\n";
	cout << "\t[1] List User\n";
	cout << "\t[2] Add New User \n";
	cout << "\t[3] Delet User\n";
	cout << "\t[4] Update User\n";
	cout << "\t[5] Find user\n";
	cout << "\t[6] Main Menue\n";
	cout << "=======================================\n";
	PerformManageMenue(UsersFileName);
}
void Deposite(string FileName)
{
	string accountnumber = ReadClientAccountNumber();
	StclientData client;
	while (!(FindByAcountNumber(FileName, accountnumber, client)))
	{
		cout << "Client With [" << accountnumber << "] dose not exist.";
		accountnumber = ReadClientAccountNumber();
	}
	PrintClientCard(client);
	vector <StclientData> Vclient = LoadClientsFromFile(FileName);
	int Amount = ReadNumber("Please Enter Depsite Amount ? ");
	char Answer;
	cout << "\n\nAre you sure you want perform this transaction? y/n ? ";
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
	{
		for (StclientData& c : Vclient)
		{
			if (c.AcountNum == accountnumber)
			{
				c.Acountbalance = c.Acountbalance + Amount;
				SaveClientDataToFile(FileName, Vclient);
				cout << "Done Secsessfully ..\nNew balance is : " << c.Acountbalance << endl;
				break;
			}
		}

	}
}
void Withdrae(string FileName)
{
	string accountnumber = ReadClientAccountNumber();
	StclientData client;
	while (!(FindByAcountNumber(FileName, accountnumber, client)))
	{
		cout << "Client With [" << accountnumber << "] dose not exist.";
		accountnumber = ReadClientAccountNumber();
	}
	PrintClientCard(client);
	vector <StclientData> Vclient = LoadClientsFromFile(FileName);
	int Amount = ReadNumber("Please Enter Withdraw Amount ? ");
	while (Amount > client.Acountbalance)
	{
		cout << "Amount Exceeds the Balance , you can Withdraw up to :" << client.Acountbalance << endl;
		cout << "Please enter another Amount ? ";
		cin >> Amount;
	}
	cout << "\n\nAre you sure you want perform this transaction? y/n ? ";

	char Answer;
	cin >> Answer;
	if (Answer == 'y' || Answer == 'Y')
	{
		for (StclientData& c : Vclient)
		{
			if (c.AcountNum == accountnumber)
			{
				c.Acountbalance = c.Acountbalance - Amount;
				SaveClientDataToFile(FileName, Vclient);
				cout << "Done Secsessfully ..\nNew balance is : " << c.Acountbalance << endl;
				break;
			}
		}

	}
}
void DepositeScreen(string FileName)
{
	system("cls");
	cout << "-----------------------------------------\n";
	cout << "\t Deposite Screen  :-) \n";
	cout << "-----------------------------------------\n";
	Deposite(FileName);
	GoBackToTransactionMenu();
}
void WithdrawScreen(string FileName)
{
	system("cls");
	cout << "-----------------------------------------\n";
	cout << "\t Withdraw Screen  :-) \n";
	cout << "-----------------------------------------\n";
	Withdrae(FileName);
	GoBackToTransactionMenu();
}
void ShowBalancestList(string FileName)
{
	system("cls");
	vector< StclientData> Vclient = LoadClientsFromFile(FileName);
	cout << "\t\t\tClient List (" << Vclient.size() << ") client (s)\n";
	cout << "-------------------------------------------------------------------------------\n";
	cout << "|" << setw(15) << left << "Acount Number"
		<< "|" << setw(20) << left << "Name"
		<< "|" << setw(12) << left << "Acount balance" << endl;
	cout << "-------------------------------------------------------------------------------\n";
	int Sum = 0;
	for (StclientData& client : Vclient)
	{
		PrintClientBalance(client);
		cout << endl;
		Sum += client.Acountbalance;
	}
	cout << "-------------------------------------------------------------------------------\n";
	cout << "\t\t\t Total Balnces = " << Sum << "\n\n";
	GoBackToTransactionMenu();
}
void PerformTransActionMenueOption(string FileName)
{
	enTransaction option = SelectTransactionOption();
	switch (option)
	{
	case enTransaction::Deposit:
	{
		DepositeScreen(FileName);
		break;
	}
	case enTransaction::Withdraw:
	{
		WithdrawScreen(FileName);
		break;
	}
	case enTransaction::Totalbalance:
	{
		ShowBalancestList(FileName);
		break;
	}
	case enTransaction::MainMenue:
	{
		ShowMainMenue(ClientsFileName);
		break;
	}
	}


}

void ShowTransActionMenue(string FileName)
{
	if (!CheckAccessPermission(enMainMenuePermession::eTransaction))
	{
		ShowAccessDeniedMessage();
		return;
	}
	TransactionMenue();
	PerformTransActionMenueOption(FileName);
}
void PerformMainMenueOption(string FileName)
{
	enOptions option = SelectMainMenueOption();
	if (option == enOptions::Show)
	{
		ShowClientList(FileName);
		GoBackToMainMenue();
	}
	if (option == enOptions::Add)
	{
		AddClients(FileName);
		GoBackToMainMenue();
	}
	if (option == enOptions::Delete)
	{
		ShowDeleteClientScreen();
		GoBackToMainMenue();
	}
	if (option == enOptions::update)
	{
		ShowUpdateClientScreen();
		GoBackToMainMenue();
	}
	if (option == enOptions::Find)
	{
		FindClient(FileName);
		GoBackToMainMenue();
	}
	if (option == enOptions::transaction)
	{
		ShowTransActionMenue(FileName);
	}
	if (option == enOptions::ManageUser)
	{
		ManageUserMenue();
	}
	if (option == enOptions::Logout)
		LogOut();
}
void PerformManageMenue(string FileName)
{
	enManageUser option = SelectManageOption();
	switch (option)
	{
	case enManageUser::listuser:
	{
		ShowUsersList(FileName);
		GoBackToManageUserMenu();
		break;
	}
	case enManageUser::addUser:
	{
		AddUsers(FileName);
		GoBackToManageUserMenu();
		break;
	}
	case enManageUser::deletUser:
	{
		ShowDeleteUserScreen();
		GoBackToManageUserMenu();
		break;
	}
	case enManageUser::findUser:
	{
		FindUser(FileName);
		GoBackToManageUserMenu();
		break;
	}
	case enManageUser::updateUser:
	{
		ShowUpdateUserScreen();
		GoBackToManageUserMenu();
		break;
	}
	case enManageUser::mainMenue:
	{
		ShowMainMenue(ClientsFileName);
		break;
	}


	}
}

void ShowMainMenue(string FileName)
{
	DrwaMainMeue();
	PerformMainMenueOption(FileName);
}
void LogIn()
{
	bool FailLogIn = false;
	string UserName, Password;
	do
	{
		system("cls");
		cout << "\n---------------------------------\n";
		cout << "\tLogin Screen";
		cout << "\n---------------------------------\n";
		if (FailLogIn)
		{
			cout << "Invlaid Username/Password!\n";
		}
		cout << "Enter Usernmae\n";
		cin >> UserName;
		cout << "Enter Password\n";
		cin >> Password;
		FailLogIn = !LoadUserInfo(UserName, Password);

	} while (FailLogIn);
	ShowMainMenue(ClientsFileName);
}




int main()
{

	LogIn();


	system("pause>0");
	return 0;
}



