#ifndef IIS_HPP
#define IIS_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

class interactiveShell{
	private:
		// Values that make the magic work
		std::map<std::string, void(*)()> commands; // This holds the argument and the function to which it will do a callback to
	   	std::map<std::string, std::string> variables; // The internal set of variables that the user can set
		std::map<std::string, std::vector<std::string>> passedArgs; // Used to pass arguments internally to functions
		std::map<std::string, std::string> returnedArgs; // This will hold all the values that a function returns
		std::vector<std::string> history; // This will hold the commands that the user has entered in realtime
		
		// Const values
		const std::string interactiveShellVersion = "1.2";
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
		void setReturnValue(std::string, std::string);
		std::string getReturnValue(std::string);
		void showHistory(); // Shows the last 10 entries
		void showHistory(std::string); // Shows the amount of entries that the user wants to see
};

interactiveShell::interactiveShell(){ this->readConfig(); }

void interactiveShell::readConfig(){
	std::ifstream openFile(this->configPath);
	if (openFile.is_open()){
		std::string line = "";
		std::vector<std::string> fileContent;
		while(std::getline(openFile, line)){ if (line[0] != '#' && !line.empty()){ fileContent.push_back(line); } } // Only keeps lines that dont start with # and are not empty
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
	auto commandPos = this->commands.find(input[0]);
	if (commandPos != this->commands.end()){
		std::vector<std::string> passedArgs;
		for (unsigned int i = 1; i < input.size(); i++){ // Gather all that potential arguments
			if (input[i][0] == '$'){ // One of the inputs are a variable, so we need to get its value
				input[i].erase(0,1); // Removes the dollar sign
				auto variablePos = this->variables.find(input[i]); // Does it exist?
				if (variablePos != this->variables.end()){ input[i] = variablePos->second; } // Add the value and continue
				else{ std::cerr << "Error: Undefined variable " << input[i] << std::endl; toReturn = false; break; }
			}
			passedArgs.push_back(input[i]);
		}
		this->passedArgs[input[0]] = passedArgs; // Add it to the "pool", it becomes the functions job to check the arguments
		commandPos->second(); // Run the command
		toReturn = true;
	}

	if (!toReturn){ // Might it be a variable?
		auto variablePos = this->variables.find(input[0]);
		if (variablePos != this->variables.end()){
			std::cout << variablePos->second << std::endl; // Print the variable value
			toReturn = true;
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
		if (!userInput.empty()){
			this->history.push_back(userInput); // Add to history
			std::vector<std::string> parsedLine = this->split(userInput, this->delimiterToSplitLinesWith);	
			if (userInput == "exit"){ run = false; }
			else if (userInput.back() == '?'){ userInput.pop_back(); helpMark(userInput); } // If the user has a "question"
			else if ((userInput.rfind("set", 0) == 0) && parsedLine.size() == 3){ this->addVariable(parsedLine[1], parsedLine[2]); } // If the user wants to define a variable
			else if (userInput == "defined"){ this->definedValues(); } // Lists all the variables and functions defined by the user
			else if (userInput == "shell_info"){ this->shellInfo(); } // Contains some information about the project
			else if (userInput == "history"){ this->showHistory(); }
			else if (parsedLine[0] == "history"){ this->showHistory(parsedLine[1]); }
			else{ this->matchInput(parsedLine); } // Match the input to a function/variable
		}
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
	// We are gonna guess what the user is entering
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
		
	if (possibleCommands.size() > 0){ // Now to list them all
			std::cout << "Possible commands are" << std::endl;
		for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }
		std::cout << std::endl;
		for(auto e:possibleCommands){ std::cout << "* " << e.first << " [" << e.second << "]" << std::endl; }
		std::cout << std::endl;	
		for(int i = 0; i < this->lineBreakerAmount; i++){ std::cout << this->lineBreaker; }			std::cout << std::endl;		
	}
}

std::vector<std::string> interactiveShell::getArgs(std::string functionName){
	std::vector<std::string> passedArguments;
	auto commandPos = this->passedArgs.find(functionName);
	if (commandPos != this->passedArgs.end()){ passedArguments = commandPos->second; }
	return passedArguments;
}

// ------------ Return statements ------------

void interactiveShell::setReturnValue(std::string functionName, std::string returnValue){
	auto pos = this->returnedArgs.find(functionName);
	if (pos == this->returnedArgs.end()){ this->returnedArgs[functionName] = returnValue; } // New entry
	else{ pos->second = returnValue; } // Update an old entry
}


std::string interactiveShell::getReturnValue(std::string functionName){
	std::string toReturn = "none";
	auto pos = this->returnedArgs.find(functionName);
	if (pos != this->returnedArgs.end()){ toReturn = pos->second; }
	return toReturn;
}

// ------------  ------------

void interactiveShell::showHistory(std::string amount){
	unsigned int roof = 0;
	try{ roof = std::stoi(amount); }
	catch(std::exception &e){ std::cerr << "Error: Invalid value " << amount << std::endl; }
	for (unsigned int i = 0; (i < roof) && (i < this->history.size()) ; i++){
		std::cout << i+1 << ") " << history[i] << std::endl;
	}		
}
void interactiveShell::showHistory(){ this->showHistory("10"); }

#endif
