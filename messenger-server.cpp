#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <thread>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// Classes --  --  --  --  --  --  --  --  --  --

class User {
private:
	string Name;
	string userName;
	string Password;
public:
	User(string name, string user_name, string password) {
		Name = name;
		userName = user_name;
		Password = password;
	}
	string getName() {
		return Name;
	}
	string getID() {
		return userName;
	}
	string getPass() {
		return Password;
	}
};
class Messenger {
private:
	vector <User> Users;
public:
	void addUser(User _user) {
		Users.push_back(_user);
	}
	// -- -- -- -- -- --

	// -- -- -- -- -- -- 
	vector<User> getUser() {
		return Users;
	}
	bool findUser(string user_id) {
		for (User us : Users) {
			if (us.getID() == user_id) {
				return 1;
			}
		}
		return 0;
	}
	
	User* getUser(string user_id) {
		User* _u;
		for (User& us : Users) {
			if (us.getID() == user_id) {
				return &us;
			}
		}
		return nullptr;
	}
};
class Register {
private:
	string _Name;
	string _userName;
	string _Password;
public:
	Register(string name, string user_id, string password) {
		_Name = name;
		_userName = user_id;
		_Password = password;
	}
	bool checkID(string user_id, Messenger& mess) {
		return !(mess.findUser(user_id));
	}
	void setInformation(Messenger& mess) {
		User _user(_Name, _userName, _Password);
		mess.addUser(_user);
	}
};
class Login {
public:
	bool checkID(string user_id, Messenger& mess) {
		if (mess.findUser(user_id)) {
			return 0;
		}
		else {
			return 1;
		}
	}
	bool checkPass(string user_pass, string user_id, Messenger& mess) {
		if (mess.getUser(user_id)->getPass() == user_pass) {
			return 1;
		}
		else {
			return 0;
		}
	}

};
class Save {
private:
	string _Name;
	string _userName;
	string _Password;
public:
	string Encoding(string pass) {
		for (char& ps : pass) {
			ps += 2;
		}
		_Password = pass;
		return _Password;
	}
	void saveInFile(Messenger& mess) {
		ofstream file("Users.txt");
		if (file.is_open()) {
			for (User us : mess.getUser()) {
				file << us.getName() << ","
					<< us.getID() << ","
					<< Encoding(us.getPass()) << endl;
			}
		}
		file.close();
	}

};
class Load {
private:
	string _Name;
	string _userName;
	string _Password;
public:
	string Decoding(string pass) {
		for (char& ps : pass) {
			ps -= 2;
		}
		return  pass;
	}
	void loadFromFile(Messenger& mess) {
		ifstream file("Users.txt");
		string line, password;
		if (file.is_open()) {
			while (getline(file, _Name, ',') && getline(file, _userName, ',') && getline(file, password)) {
				_Password = Decoding(password);
				User u(_Name, _userName, _Password);
				mess.addUser(u);
			}
		}
		file.close();
	}
};

// Functions Prototypes--  --  --  --  --  --  --	  --  --  --
void Start(int);
string recv_Fu(int);
void send_Fu(int, string);
void sendMessage(int, User*, Messenger&);
void sendFile(int, User*, Messenger&);
void receiveMessage(int, User*, Messenger&);
void printFile(int, User*);
void Reply(int, string, string);
void saveFile(int, string, string, string, string);
void saveMessages(int, string, string, string, bool);
void LoadUsers_Fu(Messenger&);
void loadMessages(int, string, string, Messenger&);
void loadMessages(int, string, Messenger&);
void loadFiles(int, string);
User* Login_Fu(int, Messenger&);
void Registeration_Fu(int, Messenger&);
void loadFile(int, string, string);

// Functions Definetions-- -- -- --- -- -- -- - - - -- 
string recv_Fu(int clSocket) {
	char buff[1024];
	string Container;
	 recv(clSocket, buff, sizeof(buff), 0);
	Container = buff + '\0';
	return Container;
}
void send_Fu(int clSocket, string Message) {
	send(clSocket, Message.c_str(), Message.length(), 0);
}
void Registeration_Fu(int clSocket, Messenger& mess) {
	string Name, userName, Password;
	string S_Client;
	S_Client = "Enter your Name :";
	send_Fu(clSocket, S_Client);
	Name = recv_Fu(clSocket);
	S_Client = "Enter your userName :";
	send_Fu(clSocket, S_Client); 
	userName = recv_Fu(clSocket); 
	S_Client = "Enter your Password :";
	send_Fu(clSocket, S_Client); 
	Password = recv_Fu(clSocket);  
	Register _register(Name, userName, Password);
	if (_register.checkID(userName, mess)) {
		_register.setInformation(mess);
		Save _saveUser;
		_saveUser.saveInFile(mess);

	}
	else {
		S_Client = "userName Unavailabe !!";
		send_Fu(clSocket, S_Client);
	}
}
User* Login_Fu(int clSocket, Messenger& mess) {
	string userName, Password;
	string S_Client;
	S_Client = "Enter your userName :";
	send_Fu(clSocket, S_Client);  
	userName = recv_Fu(clSocket);  
	S_Client = "Enter your Password :";
	send_Fu(clSocket, S_Client );  
	Password = recv_Fu(clSocket); 
	if (mess.findUser(userName)) {
		if (mess.getUser(userName)->getPass() == Password) {
			S_Client = "-> Login Succesful <-\n";
			send_Fu(clSocket, S_Client);  
			return mess.getUser(userName);
		}
		else {
			S_Client = "The Password is not correct !!\n";
			send_Fu(clSocket, S_Client); 
			return nullptr;
		}
	}
	else {
		S_Client = "User not found !!\n";
		send_Fu(clSocket, S_Client);  
		return nullptr;
	}
}
void LoadUsers_Fu(Messenger& mess) {
	Load _loadUser;
	_loadUser.loadFromFile(mess);
}

void sendMessage(int clSocket, User* us, Messenger& mess) {
	string dis_ID, S_Client;
	bool Exit = false;
	while (Exit == false) {
		S_Client = "-> Send Message <-\n- Enter username you want to send a message to (0 for Exit):";
		send_Fu(clSocket, S_Client);
		dis_ID = recv_Fu(clSocket);
		if (dis_ID != "0") {
			if (mess.findUser(dis_ID)) {
				string _Message;
				S_Client = "Enter your Message to send to user " + dis_ID + " :";
				send_Fu(clSocket, S_Client);  
				_Message = recv_Fu(clSocket);
				saveMessages(clSocket, us->getID(), _Message, dis_ID, false);
				S_Client = "The message was sent !!\n";
				send_Fu(clSocket, S_Client);  
			}
			else {
				S_Client = "Invalid username !!\n";
				send_Fu(clSocket, S_Client);  
			}
		}
		else {
			S_Client = "Exit !!\n";
			send_Fu(clSocket, S_Client); 
			Exit = true;
		}
	}
}
void receiveMessage(int clSocket, User* us, Messenger& mess) {
	string choice, S_Client;
	bool Exit = false;
	while (Exit == false) {
		S_Client = "-> Print Messages <-\n1 - All messages\n2 - User messages\n3 - Exit\n:";
		send_Fu(clSocket, S_Client); 
		choice = recv_Fu(clSocket);  
		if (choice != "3") {
			if (choice == "1") {
				string _wait;
				loadMessages(clSocket, us->getID(), mess);
				S_Client = "Enter any key to continue :";
				send_Fu(clSocket, S_Client); 
				_wait = recv_Fu(clSocket);
			}
			else if (choice == "2") {
				string _userID;
				bool _Exit = false;
				while (_Exit == false) {
					S_Client = "Enter userName (0 for Exit):";
					send_Fu(clSocket, S_Client); 
					_userID = recv_Fu(clSocket);
					if (_userID != "0") {
						if (mess.findUser(_userID)) {
							string _choice;
							loadMessages(clSocket, us->getID(), _userID, mess);
							S_Client = "Enter 1 to answer and 0 to exit :";
							send_Fu(clSocket, S_Client);
							_choice = recv_Fu(clSocket);
							if (_choice == "1") {
								Reply(clSocket, us->getID(), _userID);
							}
						}
						else {
							S_Client = "Invalid username !!\n";
							send_Fu(clSocket, S_Client); 					
						}
					}
					else {
						S_Client = "Exit !!\n";
						send_Fu(clSocket, S_Client);  					
						_Exit = true;
					}

				}
			}
			else {
				S_Client = "Invalid Number !!\n";
				send_Fu(clSocket, S_Client);  					
			}
		}
		else {
			S_Client = "Exit !!\n";
			send_Fu(clSocket, S_Client);  				
			Exit = true;
		}
	}
}
void saveMessages(int clSocket, string o_ID, string message, string d_ID, bool type) {
	ofstream file("usersMessages.txt", ios::app);
	if (file.is_open()) {
		file << (type ? 'F' : 'M') << ',' << o_ID << ',' << message << ',' << d_ID << endl;
		file.close();
	}
	else {
		string S_Client = "Error while opening file !!\n";
		send_Fu(clSocket, S_Client); 
	}
}
void loadMessages(int clSocket, string d_ID, string o_ID, Messenger& mess) {
	string Line, _O_ID, _D_ID, Message, S_Client, type;
	int i = 0;
	ifstream file("usersMessages.txt");
	if (file.is_open()) {
		while ( getline(file,type,',') && getline(file, _O_ID, ',') && getline(file, Line, ',') && getline(file, _D_ID)) {
			if ((type[0] == 'M') && (_O_ID == o_ID) && (_D_ID == d_ID)) {
				Message += ('-' + Line + '\n');
				i++;
			}
		}
		if (i > 0) {
			string Name = mess.getUser(o_ID)->getName();
			S_Client = "Name : " + Name + " , ID : " + o_ID + '\n' + "Messages : " + Message;
			send_Fu(clSocket, S_Client);
		}
		else {
			S_Client = "You have no messages from this user !!\n";
			send_Fu(clSocket, S_Client); 
		}
	}
	else {
		S_Client = "Error while opening file !!\n";
		send_Fu(clSocket, S_Client); 
	}
}
void loadMessages(int clSocket, string d_ID, Messenger& mess) {
	vector <User> users = mess.getUser();
	string _D_ID, _O_ID, _line, S_Client;
	for (User _us : users) {
		ifstream file("usersMessages.txt");
		int i = 0;
		string _userID = _us.getID(),type ;
		while (getline(file,type,',')&& getline(file, _O_ID, ',') && getline(file, _line, ',') && getline(file, _D_ID)) {
			if ((type == "M") && (_D_ID == d_ID) && (_O_ID == _userID)) {
				i++;
			}
		}
		file.close();
		if (i > 0) {
			S_Client = "You have " + to_string(i);
			S_Client += " Message from " + _userID + '\n';
			send_Fu(clSocket, S_Client);
		}
	}
	
}
void Reply(int clSocket, string o_ID, string d_ID) {
	string Message, S_Client;
	S_Client = "Reply to " + d_ID + " :";
	send_Fu(clSocket, S_Client); 
	Message = recv_Fu(clSocket);
	saveMessages(clSocket, o_ID, Message, d_ID, false);
}

void printFile(int clSocket, User* us) {
	string _fileName, S_Client;
	bool Exit = false;
	while (Exit == false) {
		S_Client = "-> Print Files <-\n";
		send_Fu(clSocket, S_Client);
		loadFiles(clSocket, us->getID());
		S_Client = "Enter filename (0 to Exit):";
		send_Fu(clSocket, S_Client);  
		_fileName = recv_Fu(clSocket);
		if (_fileName != "0") {
			loadFile(clSocket, us->getID(), _fileName);
		}
		else {
			S_Client = "Exit !!\n";
			send_Fu(clSocket, S_Client); 
			Exit = true;
		}
	}
}
void sendFile(int clSocket, User* us, Messenger& mess) {
	string dis_ID, S_Client;
	bool Exit = false;
	while (Exit == false) {
		S_Client = "-> Send File <-\n-Enter username you want to send a file to (0 for Exit) :";
		send_Fu(clSocket, S_Client); 
		dis_ID = recv_Fu(clSocket);
		if (dis_ID != "0") {
			if (mess.findUser(dis_ID)) {
				string Line = "", fileName, Container;
				S_Client = "Enter your Filename to send to user " + dis_ID + " :";
				send_Fu(clSocket, S_Client);  
				fileName = recv_Fu(clSocket);
				while (Line != "-1") {
					S_Client = "Enter the information in your file to send to user " + dis_ID + " line by line(-1 for Exit):";
					send_Fu(clSocket, S_Client);  
					Line = recv_Fu(clSocket);
					Container += (Line + '\n');
				}
				saveMessages(clSocket, us->getID(), fileName, dis_ID, true);
				saveFile(clSocket, us->getID(), fileName, Container, dis_ID);
			}
			else {
				S_Client = "Invalid username !!\n";
				send_Fu(clSocket, S_Client);
			}
		}
		else {
			S_Client = "Exit !!\n";
			send_Fu(clSocket, S_Client); 
			Exit = true;
		}
	}
}
void saveFile(int clSocket, string o_ID, string fileName, string Container, string d_ID) {
	ofstream file(fileName);
	if (file.is_open()) {
		saveMessages(clSocket, o_ID, fileName, d_ID, true);
		file << Container;
		file.close();
	}
	else {
		string S_Client = "Error while opening file !!\n";
		send_Fu(clSocket, S_Client);
	}
}
void loadFiles(int clSocket, string d_ID) {
	ifstream file("usersMessages.txt");
	string _O_ID, _Message, _D_ID, _Type, S_Client;
	if (file.is_open()) {
		while (getline(file, _Type, ',') && getline(file, _O_ID, ',') && getline(file, _Message, ',') && getline(file, _D_ID)) {
			if ((d_ID == _D_ID) && (_Type == "F")) {
				S_Client = _O_ID + " : " + _Message + '\n';
				send_Fu(clSocket, S_Client);
			}
		}
	}
	else {
		S_Client = "Error while opening file !!\n";
		send_Fu(clSocket, S_Client);
	}
}
void loadFile(int clSocket, string o_ID, string _fileName) {
	ifstream file(_fileName);
	string line, S_Client;
	if (file.is_open()) {
		while (getline(file, line)) {
			S_Client = line + '\n';
			send_Fu(clSocket, S_Client);
		}
	}
	else {
		S_Client = "Error while opening file !!\n";
		send_Fu(clSocket, S_Client);
	}
}


void Start(int ClientSocket) {
	Messenger _messenger;
	LoadUsers_Fu(_messenger);
	string choice, S_Client;
	bool Exit = false;
	while (!Exit) {
		S_Client = "-> Welcome <-\n1- Login\n2- Register\n(quit for Exit):";
		send_Fu(ClientSocket, S_Client);
		choice = recv_Fu(ClientSocket);
		if (choice == "2") {
			Registeration_Fu(ClientSocket, _messenger);
		}
		else if (choice == "1") {
			for (User us : _messenger.getUser()) {
				string username = us.getID();
				string name = us.getName();
				string password = us.getPass();
				send_Fu(ClientSocket, "Id : " + username);
			}
			send_Fu(ClientSocket, "\n");
			User* _us;
			_us = Login_Fu(ClientSocket, _messenger);
			if (_us != nullptr) {
				bool _Exit = true;
				while (_Exit == true) {
					string choice, _S_Client;
					_S_Client = "-> Welcome To Messenger <-\n1- Send the message\n2- View received messages\n3- Exit\n:";
					send_Fu(ClientSocket, _S_Client);
					choice = recv_Fu(ClientSocket);
					if (choice == "1") {
						string Send_ch;
						bool __Exit = false;
						while (__Exit == false) {
							_S_Client = "1- Messages\n2- Files\n3- Exit\n:";
							send_Fu(ClientSocket, _S_Client);
							Send_ch = recv_Fu(ClientSocket);
							if (Send_ch != "3") {
								if (Send_ch == "1") {
									sendMessage(ClientSocket, _us, _messenger);
								}
								else if (Send_ch == "2") {
									sendFile(ClientSocket, _us, _messenger);
								}
								else {
									_S_Client = "Invalid number !!\n";
									send_Fu(ClientSocket, _S_Client);
								}
							}
							else {
								__Exit = true;
							}
						}
					}
					else if (choice == "2") {
						string Rec_ch;
						bool __Exit = false;
						while (__Exit == false) {
							_S_Client = "1- Messages\n2- Files\n3- Exit\n:";
							send_Fu(ClientSocket, _S_Client);
							Rec_ch = recv_Fu(ClientSocket);
							if (Rec_ch != "3") {
								if (Rec_ch == "1") {
									receiveMessage(ClientSocket, _us, _messenger);
								}
								else if (Rec_ch == "2") {
									printFile(ClientSocket, _us);
								}
								else {
									S_Client = "Invalid number !!\n";
									send_Fu(ClientSocket, _S_Client);
								}
							}
							else {
								__Exit = true;
							}
						}
					}
					else if (choice == "3") {
						_Exit = false;
					}
					else {
						S_Client = "Invalid number !!\n";
						send_Fu(ClientSocket, _S_Client);
					}
				}
			}
		}
		else if (choice == "quit") {
			Exit = true;
		}
		else {
			S_Client = "Invalid number !!\n";
			send_Fu(ClientSocket, S_Client);
		}
	}
	closesocket(ClientSocket);
}



int main() {
	WSADATA wsadata;
	SOCKET ServerSocket, ClientSocket;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) == 0) {
		// WSADATA -- -- --- -- -- --
		cout << "Run complete!\n";
		ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (ServerSocket != -1) {
			// MAKE SOCKETS -- -- -- -- -- -- --
			sockaddr_in serverAddr;
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(8888);
			serverAddr.sin_addr.s_addr = INADDR_ANY;
			// MAKE ADDRESS -- -- --- -- -- -- 
			if (bind(ServerSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != -1) {
				// BINDIING -- -- - -- - -- - - -
				listen(ServerSocket, 3);
				vector <thread> Clients;
				// listening -- -- - -  --- -
				while(true){
					sockaddr_in clientAddr;
					int cl_AddrSize = sizeof(clientAddr);
					ClientSocket = accept(ServerSocket, (sockaddr*)&clientAddr, &cl_AddrSize);
					if (ClientSocket != -1) {
						/*char messa[] = "salam";
						send(ClientSocket, messa, strlen(messa), 0);*/
						Clients.emplace_back(Start,ClientSocket);
					}
					else {
						cout << "Error when Accept client!!" << endl;
					}
				}
				for (thread& th : Clients) {
					th.join();
				}
			}
			else {
				cout << "Error when Bind server!!" << endl;
			}
		}
		else {
			cout << "Error when Create Socket!!" << endl;
		}
	}
	else {
		cout << "Error when Start WSA!!" << endl;
	}
	closesocket(ServerSocket);
	WSACleanup();
	return 0;
}
