#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <regex>

const std::map<std::string, int> MONTH_LOOKUP = {{"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4}, {"May", 5}, {"July", 6}, {"June", 7}, {"Aug", 8}, {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}};
const std::map<int, std::string> MONTH_LOOKUP_REVERSE = {{1, "Jan"}, {2, "Feb"}, {3, "Mar"}, {4, "Apr"}, {5, "May"}, {6, "July"}, {7, "June"}, {8, "Aug"}, {9, "Sep"}, {10, "Oct"}, {11, "Nov"}, {12, "Dec"}};
const char* PATH = "list.txt";
const char* RESPV = "resp v0.2";

std::ostream& bold_on(std::ostream& os) {
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os){
    return os << "\e[0m";
}

bool is_num(std::string& line){
	char* p;
	strtol(line.c_str(), &p, 10);
	return *p == 0;
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
	int id, day, year;
	std::string month, topic, description;

	Resp(){}

	Resp(int x, int y, std::string z, int t, std::string n, std::string m){
		id = x; day = y; month = z; year = t; topic = n; description = m;
	}

	void print() const{
		std::cout << std::left << std::setw(3) << id << "  " << std::left << std::setw(10) << topic << "  " << description << std::endl;
	}
};

bool cmp(Resp x, Resp y){
	if(x.year < y.year) return true;
	else if(x.year == y.year){
		if(MONTH_LOOKUP.at(x.month) < MONTH_LOOKUP.at(y.month)) return true;
		else if(MONTH_LOOKUP.at(x.month) == MONTH_LOOKUP.at(y.month)){
			if(x.day < y.day) return true;
			else if(x.day == y.day){
				if(x.topic < y.topic) return true;
				else if(x.topic == y.topic) return x.description < y.description;
				return false;
			}
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
		resps.insert({stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), temptokens[4], temptokens[5]});
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
	std::cout << bold_on << "\n" << RESPV << " help:\n";
	std::cout << "---------------\n\n";
	std::cout << "Commands:\n\n" << bold_off;
	std::cout << bold_on << "q: " << bold_off << "quits program\n";
	std::cout << bold_on << "cls: " << bold_off << "clears screen\n";
	std::cout << bold_on << "help: "<< bold_off << "prints this text\n";
	std::cout << bold_on << "show [TOPIC]: " << bold_off << "shows the responsibility list, if topic is specified only shows responisibilities of that topic\n";
	std::cout << bold_on << "add <DAY> <MONTH> <YEAR> <TOPIC> <DESCRIPTION>: " << bold_off << "adds responsibility to list\n";
	std::cout << bold_on << "del <ID>: "<< bold_off << "deletes responsibility from list\n";
	std::cout << bold_on << "change <ID> {day|month|year|topic|description}: " << bold_off << "changes the item of given id\n";
	std::cout << "\n";
}

void printResp(const std::set<Resp, decltype(cmp)*>& resps, const std::string& topic){
	bool topic_check = false;
	if(resps.empty()){
		std::cout << "no responsibilities\n";
		return;
	}
	int lastDate[3] = {-1, -1, -1};
	for(const auto& resp: resps){
		if(topic != "all" && topic != resp.topic) continue;
		topic_check = true;
		if(lastDate[0] != resp.day || lastDate[1] != MONTH_LOOKUP.at(resp.month) || lastDate[2] != resp.year){
			std::cout << "\n\n" << bold_on << resp.day << " " << resp.month << " " << resp.year << bold_off << "\n";
			lastDate[0] = resp.day; lastDate[1] = MONTH_LOOKUP.at(resp.month); lastDate[2] = resp.year;
		}
		resp.print();
	}
	if(topic != "all" && !topic_check) std::cout << "no responsibility of topic \"" << topic << "\" found\n";
	else std::cout << "\n\n";
}

void addResp(const std::string& command, std::set<Resp, decltype(cmp)*>& resps){
	std::ofstream output(PATH, std::ios_base::app);
	std::vector<std::string> temptokens = tokenize(command);
	if(is_num(temptokens[2])) temptokens[2] = MONTH_LOOKUP_REVERSE.at(stoi(temptokens[2]));

	int temp = getID();
	output << temp << " ";
	output << temptokens[1] << " " << temptokens[2] << " " << temptokens[3] << " " << temptokens[4] << " \"" << temptokens[5] << "\"\n";
	output.close();
	
	resps.insert({temp, stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), temptokens[4], temptokens[5]});
}

void deleteResp(const std::string& command, const std::string& id, std::set<Resp, decltype(cmp)*>& resps){
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
				resps.erase({stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), temptokens[4], temptokens[5]});
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

void changeResp(std::set<Resp, decltype(cmp)*>& resps, const std::string& id, const std::string& change){
	bool changed = false;
	std::cout << "write the changed version: ";
	std::string changed_str;
	std::getline(std::cin, changed_str);
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
				changed = true;
				std::vector<std::string> temptokens = tokenize(line);
				resps.erase({stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), temptokens[4], temptokens[5]});
				Resp temp;
				if(change == "day") temp = {stoi(temptokens[0]), stoi(changed_str), temptokens[2], stoi(temptokens[3]), temptokens[4], temptokens[5]};
				else if(change == "month"){
					if(is_num(changed_str)) changed_str = MONTH_LOOKUP_REVERSE.at(stoi(changed_str));
					temp = {stoi(temptokens[0]), stoi(temptokens[1]), changed_str, stoi(temptokens[3]), temptokens[4], temptokens[5]};
				}
				else if(change == "year") temp = {stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(changed_str), temptokens[4], temptokens[5]};
				else if(change == "topic") temp = {stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), changed_str, temptokens[5]};
				else if(change == "description") temp = {stoi(temptokens[0]), stoi(temptokens[1]), temptokens[2], stoi(temptokens[3]), temptokens[4], changed_str};
				
				resps.insert(temp);
				std::string temp_str;
				temp_str = std::to_string(temp.id) + " " + std::to_string(temp.day) + " " + temp.month + " " + std::to_string(temp.year) + " " + temp.topic + " \"" + temp.description + "\""; 
				std::cout << temp_str << "\n";
				lines.push_back(temp_str);
			}
		}
	}
    myfile.close();
    if(!changed){
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
	std::cout << RESPV << "\n";
	std::string command;
	std::set<Resp, decltype(cmp)*> resps = setResps();

	while(true){
		std::cout << "> ";
		std::getline(std::cin, command);

		if(command.empty()) continue;
		std::vector<std::string> tokens = tokenize(command);

		if(tokens[0] == "help") printHelp();
		else if(tokens[0] == "show"){
			if(tokens.size() == 1) printResp(resps, "all");
			else if(tokens.size() == 2) printResp(resps, tokens[1]);
		}
		else if(tokens[0] == "q") return 0;
		else if(tokens[0] == "add") addResp(command, resps);
		else if(tokens[0] == "del") deleteResp(command, tokens[1], resps);
		else if(tokens[0] == "cls") system("clear");
		else if(tokens[0] == "change") changeResp(resps, tokens[1], tokens[2]);
		else std::cout << "command not found\n";
	}
}
