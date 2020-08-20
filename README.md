# I.I.S

The IIS or Interactive Internal Shell is a simple yet customizable shell for usage within projects. 
Have you ever sat and coded multiple interactive functions for an easier experince for your users? Well that's why IIS exists!
With IIS you can say kiss goodbye to writing multiple if statements to see if the user has entered an existing value or not!

IIS works fairly simply, the example below shows you how to set up a function that will trigger (and run) when the user enters 'test' in the console

```
#include "iis.hpp"

void test(){
	std::cout << "hi my name is peter burker" << std::endl;
}

int main(){
	interactiveShell i;
	i.addCommand("test", test);
	i.loop();
	return 0;
}
```
After `i.loop()` has been triggered, the user will be provided with this display<br/>
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic1.png)<br/>
<br/>
And when the user enters ``test``<br/>
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic2.png)<br/>
<br/>
You can also define internal variables<br/>
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic3.png)<br/>

IIS is also very customizable! After running the project for the first time <b>and</b> creating an iis class object, a file called .iis.config will be created. This is how it will look!
```
# This is how a comment looks like

# Program
#name test
#author Peter_Burger
#version 1.0
#showNameOnStart false
#showAuthorNameOnStart false
#showVersionOnStart false


# Screen
#lineStartSymbol ::
#lineBreaker --
#lineBreakerAmount 25
```
IIS comes with some prebuilt functions!<br/>
<br/>
<b>defined</b> - This commands shows all the commands and variables which has been set<br/>
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic4.png)<br/>
<br/>

<b>?</b> - This commands is used to see which commands and variables which match what has already been enterd
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic5.png)<br/>
<br/>

<b>shell_info</b> - This commands shows some small information about the project, and not the program in which this might be used in 
![](https://github.com/s9rA16Bf4/InteractiveInternalShell/blob/master/pictures/pic6.png)<br/>
<br/>
