#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <regex>

const std::map<std::string, int> MONTH_LOOKUP = {{"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4}, {"May", 5}, {"July", 6}, {"June", 7}, {"Aug", 8}, {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}};
const char* PATH = "list.txt";

std::ostream& bold_on(std::ostream& os){
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os){
    return os << "\e[0m";
}

std::vector<std::string> tokenize(const std::string& command){
	const std::regex regexpr("[^\\s\"']+|\"([^\"]*)\"|'([^']*)'");
	std::sregex_token_iterator rend;
	std::vector<std::string> temp;

	std::sregex_token_iterator a (command.begin(), command.end(), regexpr);
	while (a!=rend){
		std::string tempstr = *a;
		if(tempstr[0] == '"' || tempstr[0] == '\'') temp.push_back(tempstr.substr(1, tempstr.size()-2));
		else temp.push_back(tempstr);
		a++;
	}

	return temp;
}

struct Resp{
	int id, day;
	std::string month, course, description;

	Resp(int x, int y, std::string z, std::string t, std::string n){
		id = x; day = y; month = z; course = t; description = n;
	}

	void print() const{
		std::cout << std::left << std::setw(3) << id << "  " << std::left << std::setw(10) << course << "  " << description << std::endl;
	}
};

bool cmp(Resp x, Resp y){
	if(MONTH_LOOKUP.at(x.month) < MONTH_LOOKUP.at(y.month)) return true;
	else if(MONTH_LOOKUP.at(x.month) == MONTH_LOOKUP.at(y.month)){
		if(x.day < y.day) return true;
		else if(x.day == y.day){
			if(x.course < y.course) return true;
			else if(x.course == y.course) return x.description < y.description;
			return false;
		}
		return false;
	}
	return false;
}

std::set<Resp, decltype(cmp)*> setResps(){
	std::ifstream myfile(PATH);
	std::string line;
	std::vector<std::string> temptokens;
	std::set<Resp, decltype(cmp)*> resps(cmp);

	while(std::getline(myfile, line)){
		temptokens = tokenize(line);
		resps.insert({stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], temptokens[3], temptokens[4]});
	}
	myfile.close();
	return resps;
}

int getID(){
	std::ifstream myfile(PATH);
	if(myfile.peek() == std::ifstream::traits_type::eof()) return 0;
	std::string line, temp;
	while(myfile >> std::ws && std::getline(myfile, line));
	int count = 0;
	for(size_t i = 0; i < line.size(); i++, count++) if(line[i] == ' ') break;
    myfile.close();
	temp = line.substr(0, count);
	myfile.close();
	return std::stoi(temp) + 1;
}

void printHelp(){
	std::cout << bold_on << "\nresp v0.1 help:\n";
	std::cout << "---------------\n\n";
	std::cout << "Commands:\n\n" << bold_off;
	std::cout << bold_on << "q: " << bold_off << "quits program\n";
	std::cout << bold_on << "help: "<< bold_off << "prints this text\n";
	std::cout << bold_on << "show: " << bold_off << "shows the responsibility list\n";
	std::cout << bold_on << "add <DAY> <MONTH> <COURSE> <DESCRIPTION>: " << bold_off << "adds responsibility to list\n";
	std::cout << bold_on << "del <ID>: "<< bold_off << "deletes responsibility from list\n\n";
}

void printResp(std::set<Resp, decltype(cmp)*>& resps){
	if(resps.empty()){
		std::cout << "no responsibilities\n";
		return;
	}
	int lastDate[2] = {-1, -1};
	for(const auto& resp: resps){
		if(lastDate[0] != resp.day || lastDate[1] != MONTH_LOOKUP.at(resp.month)){
			std::cout << "\n\n" << bold_on << resp.day << " " << resp.month << bold_off << "\n";
			lastDate[0] = resp.day; lastDate[1] = MONTH_LOOKUP.at(resp.month);
		}
		resp.print();
	}
	std::cout << "\n\n";
}

void addResp(const std::string& command, std::set<Resp, decltype(cmp)*>& resps){
	std::ofstream output(PATH, std::ios_base::app);
	int temp = getID();
	output << temp << " ";
	output << command.substr(4, command.size() - 4) << "\n";
	output.close();

	std::vector<std::string> temptokens = tokenize(command);
	resps.insert({temp, stoi(temptokens[1]), temptokens[2], temptokens[3], temptokens[4]});
}

void deleteResp(const std::string& command, std::string& id, std::set<Resp, decltype(cmp)*>& resps){
	bool deleted = false;
	std::ifstream myfile(PATH);
	std::vector<std::string> lines;
	std::string line;
	while(std::getline(myfile, line)){
		for(size_t i = 0; i < id.size(); i++){
			if(line[i] != id[i]){
				lines.push_back(line);
				continue;
			}
			else{
				deleted = true;
				std::vector<std::string> temptokens = tokenize(line);
				resps.erase({stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], temptokens[3], temptokens[4]});
			}
		}
	}
    myfile.close();
    if(!deleted){
    	std::cout << "ID not found\n";
    	return;
    }

    std::ofstream output("list.txt");
    for(std::string& el: lines){
    	output << el << "\n";
    }
    output.close();
}

int main(){
	std::ofstream temp(PATH, std::ios_base::app);
	temp.close();
	std::cout << "resp v0.1\n";
	std::string command;
	std::set<Resp, decltype(cmp)*> resps = setResps();

	while(true){
		std::cout << "> ";
		std::getline(std::cin, command);

		if(command.empty()) continue;
		std::vector<std::string> tokens = tokenize(command);

		if(tokens[0] == "help") printHelp();
		else if(tokens[0] == "show") printResp(resps);
		else if(tokens[0] == "q") return 0;
		else if(tokens[0] == "add") addResp(command, resps);
		else if(tokens[0] == "del") deleteResp(command, tokens[1], resps);
		else std::cout << "command not found\n";
	}
}
