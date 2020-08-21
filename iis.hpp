#ifndef IIS_HPP
#define IIS_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

class interactiveShell{
	private:
		// Private and internal values
		std::map<std::string, void(*)()> commands; // This holds the argument and the function to which it will do a callback to
	   	std::map<std::string, std::string> variables; // The internal set of variables that the user can set
		std::map<std::string, std::vector<std::string>> passedArgs; // Used to pass arguments internally to functions
		const std::string interactiveShellVersion = "1.1";
		const std::string interactiveShellAuthor = "@s9rA16Bf4";
		const std::string configPath = ".iis.config";

		// The user configuration
		std::string programName = "null";
		std::string programVersion = "null";
		std::string programAuthor = "null";
		std::string terminalBackground = "light-blue"; // ?
		bool showProgramNameOnStart = false;
		bool showProgramVersionOnStart = false;
		bool showProgramAuthorOnStart = false;
		std::string lineStarterSymbol = ":: "; // Default
		std::string lineBreaker = "-"; // Default
		int lineBreakerAmount = 25; // Default
		std::string delimiterToSplitLinesWith = " "; // Default
	public:
		interactiveShell();
		void readConfig();
		bool addCommand(std::string, void(*f)());
		bool addVariable(std::string, std::string);
		bool matchInput(std::vector<std::string>);
		void loop();
		std::vector<std::string> split(const std::string&, const std::string&);
		void shellInfo();
		void definedValues();
		void helpMark(std::string);
		std::vector<std::string> getArgs(std::string);
};

interactiveShell::interactiveShell(){
	this->readConfig();
}

void interactiveShell::readConfig(){
	std::ifstream openFile(this->configPath);
	if (openFile.is_open()){
		std::string line = "";
		std::vector<std::string> fileContent;
		while(std::getline(openFile, line)){ if (line[0] != '#' && !line.empty()){ fileContent.push_back(line); } }
		openFile.close();

		for (std::string entry:fileContent){
			std::vector<std::string> proccessedLine = this->split(entry, " ");
			if (proccessedLine[0] == "name"){ this->programName = proccessedLine[1]; }
			else if (proccessedLine[0] == "author"){ this->programAuthor = proccessedLine[1]; }
			else if (proccessedLine[0] == "version"){ this->programVersion = proccessedLine[1]; }
			else if (proccessedLine[0] == "showNameOnStart" && proccessedLine[1] == "true"){ this->showProgramNameOnStart = true; }
			else if (proccessedLine[0] == "showVersionOnStart" && proccessedLine[1] == "true"){ this->showProgramVersionOnStart = true; }
			else if (proccessedLine[0] == "showAuthorOnStart" && proccessedLine[1] == "true"){ this->showProgramAuthorOnStart = true; }
			else if (proccessedLine[0] == "lineStartSymbol"){ this->lineStarterSymbol = proccessedLine[1]; }
			else if (proccessedLine[0] == "lineBreaker"){ this->lineBreaker = proccessedLine[1]; }
			else if (proccessedLine[0] == "lineBreakerAmount"){ this->lineBreakerAmount = std::stoi(proccessedLine[1]); }
		}
	}else{
		// Failed to open file so we need to create it
		std::ofstream writeFile(this->configPath);
		std::vector<std::string> defaultConfig = {"# This is how a comment looks like", "","# Program", "#name test", "#author Peter_Burger", "#version 1.0"
		, "#showNameOnStart false", "#showAuthorNameOnStart false", "#showVersionOnStart false", "","# Screen", "#lineStartSymbol ::", "#lineBreaker --", 
		"#lineBreakerAmount 25"};

		for (std::string entry:defaultConfig){ writeFile << entry << std::endl; } // Write the config files
		writeFile.close(); // And close
		this->readConfig(); // Now read the config again, and we are done!
	}
}

bool interactiveShell::addCommand(std::string trigger, void(*f)()){
	bool toReturn = false;
	try {
		this->commands[trigger] = &(*f);
		toReturn = true;
	}catch(std::exception &e){ std::cerr << "Error: " << e.what() << std::endl;	}
	return toReturn;
}

bool interactiveShell::addVariable(std::string trigger, std::string value){
	bool toReturn = false;
	try {
		this->variables[trigger] = value;
		toReturn = true;
	}catch(std::exception &e){ std::cerr << "Error: " << e.what() << std::endl;	}
	return toReturn;
}

bool interactiveShell::matchInput(std::vector<std::string> input){
	bool toReturn = false;
	for (auto entry:this->commands){
		if (input[0] == entry.first){
			std::vector<std::string> passedArgs;
			for (unsigned int i = 1; i < input.size(); i++){ passedArgs.push_back(input[i]); } // Gather all that potential arguments
			this->passedArgs[input[0]] = passedArgs; // Add it to the "pool", it becomes the functions job to check the arguments
			entry.second(); // Run the command
			toReturn = true;
			break;
		}
	}

	if (!toReturn){ // Might it be a variable?
		for (auto entry:this->variables){
			if (input[0] == entry.first){
				std::cout << entry.second << std::endl; // Print the variable value
				toReturn = true;
				break;
			}
		}
	}
	if (!toReturn){ std::cerr << "Error: Unknown input " << input[0] << std::endl; }
	return toReturn;
}


void interactiveShell::loop(){
	bool run = true;
	bool showedProgInfo = false;
	if (this->showProgramNameOnStart){ std::cout << "Name: " << this->programName << std::endl; showedProgInfo = true; }
	if (this->showProgramAuthorOnStart){ std::cout << "Author: " << this->programAuthor << std::endl; showedProgInfo = true;}
	if (this->showProgramVersionOnStart){ std::cout << "Version: " << this->programVersion << std::endl; showedProgInfo = true;}
	if (showedProgInfo){ for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }  std::cout << std::endl; }
	std::string userInput = "";	
	
	while(run){
		std::cout << std::endl << this->lineStarterSymbol;
		std::getline(std::cin, userInput);
		std::vector<std::string> parsedLine = this->split(userInput, this->delimiterToSplitLinesWith);
		
		if (userInput == "exit"){ run = false; }
		else if (userInput.back() == '?'){ userInput.pop_back(); helpMark(userInput); } // If the user has a "question"
		else if (userInput.rfind("set", 0) == 0){ this->addVariable(parsedLine[1], parsedLine[2]); } // If the user wants to define a variable
		else if (userInput == "defined"){ this->definedValues(); } // Lists all the variables and functions defined by the user
		else if (userInput == "shell_info"){ this->shellInfo(); } // Contains some information about the project
		else{ this->matchInput(parsedLine); } // Match the input to a function/variable
	}
}

std::vector<std::string> interactiveShell::split(const std::string& lineToSplit, const std::string& delim){
  std::vector<std::string> toReturn;
  std::size_t delimPrev = 0, delimPos = 0; // Positions to keep track of where we have been and where the delimiter is
  while(lineToSplit.size() > delimPrev && lineToSplit.size() > delimPos){
      delimPos = lineToSplit.find(delim, delimPrev);
      if (delimPos == std::string::npos){ delimPos = lineToSplit.size(); }
      std::string subStr = lineToSplit.substr(delimPrev, delimPos-delimPrev);
      if (!subStr.empty()){ toReturn.push_back(subStr); }
      delimPrev = delimPos + delim.size();
  }
  return toReturn;
}

void interactiveShell::definedValues(){
	for (int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
	std::cout << "\nFunctions: " << std::endl;
	for (auto e:this->commands){ std::cout << "* " << e.first << std::endl; }
	for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
	std::cout << "\nVariables: " << std::endl;
	for (auto e:this->variables){ std::cout << "* " << e.first << std::endl; }
	for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
	std::cout << std::endl;
}

void interactiveShell::shellInfo(){
	for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
	std::cout << "\nShell version: " << this->interactiveShellVersion << std::endl;
	std::cout << "Author: " << this->interactiveShellAuthor << std::endl;
	for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
	std::cout << std::endl;
}

void interactiveShell::helpMark(std::string line){
	if (line.rfind("set", 0) == 0){ std::cout << "'set <variableName> <value>'. Later accessible when entering <variableName>. I.e 'set N 10'" << std::endl; }
	else{ // We are gonna guess what the user is entering
		std::map<std::string, std::string> possibleCommands;
		int sameAmount = 0;
		for (auto e:this->commands){ // Investigating if its a command
			for (unsigned int i = 0; i < line.size(); i++){ 
				if (e.first[i] == line[i]){ sameAmount++; }
				if ((i+1) >= e.first.size() && (i+1) < line.size()){ sameAmount = 0; break; } // is there more left on the user input then the amount of chars that the function has in its name?	
			}
			if ((sameAmount != 0) && (-1 < line.size() - sameAmount <= 3)){ possibleCommands[e.first] = "command"; }
		}

		for (auto e:this->variables){ // or if its a variable
			for (unsigned int i = 0; i < line.size(); i++){
				if (e.first[i] == line[i]){ sameAmount++; }
				if ((i+1) >= e.first.size() && (i+1) < line.size()){ sameAmount = 0; break; }				
			}
			if ((sameAmount != 0) && (-1 < line.size() - sameAmount <= 3)){ possibleCommands[e.first] = "variable"; }
		}

		std::cout << "Possible commands are" << std::endl;
		for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
		std::cout << std::endl;
		for(auto e:possibleCommands){ std::cout << "* " << e.first << " [" << e.second << "]" << std::endl; }
		std::cout << std::endl;	
		for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
		std::cout << std::endl;		
	}
}

std::vector<std::string> interactiveShell::getArgs(std::string functionName){
	std::vector<std::string> passedArguments;
	for (auto e:this->commands){ 
		if (e.first == functionName){
			passedArguments = this->passedArgs[functionName];
			break;	
		}
	}
	return passedArguments;
}


#endif
