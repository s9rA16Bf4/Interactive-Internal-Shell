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

IIS is also highly customizable! After running the project for the first time <b>and</b> creating an iis class object, a file called .iis.config will be created in the local directory!

I've written a lot more about the different functions of the project and how you can suite it for your needs in the wiki!
