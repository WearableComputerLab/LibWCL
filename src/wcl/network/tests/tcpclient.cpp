#include <stdio.h>
#include "../TCPSocket.h" //header for the networking

#include "serial.h"//header for the serial dirver

#include <unistd.h>//used for sleep
#include <sys/time.h>//used for nano sleep and time struct
#include <time.h>
#include <sys/types.h>

#include <stdio.h>  //for file input and out put


	char buffer[4096];
	TCPSocket s("10.0.0.5", 1234);
	int fd;//file descriptor for the serial port
	int c, res;  //int's used for the serial buffer reading
        char buf[255];//the serial buffer
	FILE *file;//a file for the output to be written to
	int cont = 1;
	struct timeval foo;//strcuts for time
	struct timezone bar;//strcuts for time

	int sleeptime = 2;//the amount of time in seconds between inputs
	
	int timeSecs = 0;//time in seconds difference used to write to the file
	int timeuSecs = 0;//time in u seconds difference used to write to the file
	int t2Secs, t1Secs, t2uSecs, t1uSecs;//used to temp storage of times
	
	void closeAll();//method definition
	void sendPacket(int number);//method definition
	void init();//method definition
	int getRand(int range);//method definition
	void processInput(int category);//method definition
	int waitForInput();//method definition
	void getInput (int input, int cat);//method definition
	void checkReturnInput();//check to see if the stick has been returned to the nuteral position
	void calcTime(int t1, int t2, int ut1, int ut2);//this should calc the time difference 
							//and write it back to timeSecs and timeUSencs

	
	/*
	fl  face buttons
	fh
	fr
	wh  wheel buttons
	wr
	wl
	sl  stalk buttons
	sr
	
	*/

int main(void)
{
	printf("starting program\n");
	
	/*while (1){
		gettimeofday (&foo, &bar);//get current time for t1
		int t1Secs = foo.tv_sec;
		int t1uSecs = foo.tv_usec;
		printf("secs= %i \tuSecs= %i\n",t1Secs, t1uSecs);
		usleep(4);
		
}*/
	
	
	calcTime(1,2,999999,2);//testing calcTime
	printf("Secs = %i uSucs = %i\n",timeSecs, timeuSecs);

	init();
/*
1= welcome
2= left
3= right
4= horn
5= wheel0
6= stalk
7= face
8= rest
9= end
10=instr
*/

	sendPacket(10);//send the instructions		
		
	printf("please read these instructions and press the horn button when ready\n");	

	waitForInput();
	
	sendPacket(11);//blank the screen
	
	sleep(2);//sleep so the user doesn't hold the button down

	int temp;//a small loop to clear the serial buffer incase the horn was held down
	for(temp = 0; temp < 10000; temp++)
		res = read(fd,buf,255);	
	
	
	printf("sleeping for a little while\n");	

	int wheel =0;
	int face =0;
	int stalk =0;
	int numberOfTests = 5;//the number of times each group will be tested
	
	int rand;
	
	//rand = getRand(2);
	//printf("randon number is = %i\n", rand);

	//cat 1 is wheel
	//cat 2 is face
	//cat 3 is stalk

	
	//gettimeofday (&foo, &bar);//get current time

	//t1Secs = foo.tv_sec; //set cuttent time
	//t1uSecs = foo.tv_usec;
	
	//printf("Elapesed time - Secs=%i \t uSecs=%i \n",t2Secs-t1Secs, t2uSecs - t1uSecs);
	file = fopen ("out.txt", "wt");//open a file called out.txt//wt is the properties of the time 
	
	//printf("in getInput with input of = %i and cat of = %i\n",input, cat);
	printf("trying to write to file with only a message\n");	
	fprintf (file, "test print \n");//print stats to file
	//printf("trying to write to file with time\n");
	//fprintf (file, "input H \t time elapsed\t %d \n",(t2Secs*t2uSecs)-(t1Secs*t1uSecs));//print stats to file

	
	int i=0;
	int testRun = 0;//this is used so that it will not prompt the user if it is cycling through
	                //randrom numbers near the end of the test.
	
	for(i=0; i < (numberOfTests*3) ;){
		rand = getRand(3);
		printf("outer loop test number = %i\n" , i);
		if(rand == 0  && wheel != numberOfTests){
			printf("processing wheel\n");
			wheel ++;
			i++;
			testRun = 1;
			fprintf (file, "\n\nWheel test\n");//print to file 
			processInput(1);		
		}
		if(rand == 1  && face != numberOfTests){
			printf("processing face\n");
			face++;
			i++;
			testRun = 1;
			fprintf (file, "\n\nFace test\n");//print to file 
			processInput(2);
		}
		if(rand == 2 && stalk != numberOfTests){
			printf("processing stalk\n");
			stalk++;
			i++;
			testRun = 1;
			fprintf (file, "\n\nStalk test\n");//print to file 
			processInput(3);
		}
		if(testRun == 1){
			sendPacket(8);//signal a rest
			printf("Please have a rest and press the horn to continue when you are ready\n");
			waitForInput();
			sendPacket(11);//blank the screen so they don't try and hold the button down
			sleep(2);
			for(temp = 0; temp < 1000; temp++)//clear the buffer
				res = read(fd,buf,255);	
			testRun = 0;
		}//end testRun if

	}

	sendPacket(1);
	sleep(9);//pause at the thankyou screen for for 15 seconds before quitting
	
/*	while (1){
	
		res = read(fd,buf,255);
		buf[res]=0;
		

		if(buf[0] == 'H' & res != 0 & res != -1){
			printf("H detected on input\n");	
			sendPacket(1);
		}
		if(buf[0] == 'L' & res != 0 & res != -1){
			printf("L detected on input\n");	
			sendPacket(2);
		}
		if(buf[0] == 'R' & res != 0 & res != -1){
			printf("R detected on input\n");	
			sendPacket(3);
		}
		if(buf[0] == 'l' & res != 0 & res != -1){
			printf("l detected on input\n");	
			sendPacket(4);
		}
		if(buf[0] == 'r' & res != 0 & res != -1){
			printf("r detected on input\n");	
			sendPacket(5);
		}
		
		//usleep(500);
		//file:///usr/share/ubuntu-artwork/home/index.html
		

	}// end while
*/

	closeAll();

    	return 0;
}

	void calcTime(int t1, int t2, int ut1, int ut2){
		//int timeSecs = 0;//time in seconds difference used to write to the file
		//int timeuSecs = 0;//time in u seconds difference used to write to the file
		
		if (t1 == t2 ){
			timeSecs = 0;
			timeuSecs = (ut2-ut1);
		}
		else{
			timeSecs = ((static_cast<int>(t2))-(static_cast<int>(t1)))-1;
			timeuSecs = (((1000000 - ut1) + ut2)) + (timeSecs * 1000000) ;			
		}
		
		
	}

void processInput(int category){
	//cat 1 is wheel
	//cat 2 is face
	//cat 3 is stalk
	
	printf("in processInput\n");

	if(category == 1)
		sendPacket(5);
	if(category == 2)
		sendPacket(7);
	if(category == 3)
		sendPacket(6);
	
	printf("please read these instructions on which set of controls to use, when you are ready press the horn button\n");
	waitForInput();
	sendPacket(11);//blank the display before the test starts
	sleep(3);//sleep for 3 secs then start test
	
	int left = 0;//rand 0
	int right = 0;//rand 1
	int horn = 0;//rand 2
	int rand = 0;
	
	int numberOfTests = 5;//the number of times to run reach test
	int i=0;
	
	//rand = 2;//test the horn
	
	for(i=0; i < (numberOfTests*3) ;){
		rand = getRand(3);
		//printf("inner loop test number = %i rand = %i\n" , i , rand);
//		if(rand == 0  && wheel != numberOfTests){
//			printf("processing wheel\n");
//			wheel ++;
//			numberOfTests++;
//			processInput(1);		
//		}
		usleep(500);//sleep for half a sec so that you don't go past the next screen if the button is help down
		
		int temp;//a small loop to clear the serial buffer incase the horn was held down
		for(temp = 0; temp < 1000; temp++)
			res = read(fd,buf,255);	

		
		if (rand == 0 && left != numberOfTests){//left
			sendPacket(2);
			left++;
			i++;
			getInput(0,category);
		}
	
		if (rand == 1  && right != numberOfTests){//right
			sendPacket(3);
			right++;
			i++;
			getInput(1,category);
		}
	
		if (rand == 2 && horn != numberOfTests){//horn
			sendPacket(4);
			horn++;
			i++;
			getInput(2,category);
		}
	}//end for

}



void getInput (int input, int cat){
	// left = 0
	// right = 1
	// horn =  2
	
	//cat 1 is wheel
	//cat 2 is face
	//cat 3 is stalk
	//int t2Secs, t1Secs, t2uSecs, t1uSecs;//these are now global
	
	//fprintf (file, "\n");//print stats to file
	

	//res = read(fd,buf,255);//read serial port//why was it being read here???
	//buf[res]=0;
		
	gettimeofday (&foo, &bar);//get current time for t1
	t1Secs = foo.tv_sec;
	t1uSecs = foo.tv_usec;


	while(cont){
		res = read(fd,buf,255);
		buf[res]=0;
		
		if(buf[0] == 'w' && buf[1] == 'h' && res != 0 & res != -1 && input == 2 && cat == 1){//this is for the horn on the wheel
			gettimeofday (&foo, &bar);//get current time for t2
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("H detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input wh \tuSecs= %i\n",timeuSecs);//print stats to file
			printf("input wh \t uSecs= %i\n",timeuSecs);
			cont = 0;
		}
		if(buf[0] == 'w' && buf[1] == 'h' && res != 0 & res != -1 && input == 2 && cat == 3){//stalk horn
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("H detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input wh s \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input wh \t uSecs= %i\n",timeuSecs);
			cont = 0;
		}
		if(buf[0] == 'w' && buf[1] == 'l'  && res != 0 && res != -1 && input == 0  && cat == 1){//wheel left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("L detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input wl \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input wl \t uSecs= %i\n",timeuSecs);
			cont = 0;
		}
		if(buf[0] == 'w' && buf[1] == 'r' && res != 0 && res != -1 && input == 1  && cat == 1){//right wheel
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("R detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input wr \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input wr \t uSecs= %i\n",timeuSecs);
			cont = 0;
		}
		if(buf[0] == 's' && buf[1] == 'l' && res != 0 && res != -1 && input == 0  && cat == 3){//stalk left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("l detected on input\n");	
			sendPacket(9);//send return stick message
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input sl \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input sl \t uSecs= %i\n",timeuSecs);
			cont = 0;
			//checkReturnInput();//see if stalk has been returned to the nuteral position.
			//sendPacket(9);//send a wait for next input packet
			sleep(3);
		}
		if(buf[0] == 's' && buf[1] == 'r' && res != 0 && res != -1 && input == 1  && cat == 3){//stalk left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("r detected on input\n");	
			sendPacket(9);//send return stick message
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input sr \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input sr \t uSecs= %i\n" ,timeuSecs);
			cont = 0;
			//checkReturnInput();//see if stalk has been returned to the nuteral position.
			//sendPacket(12);//send a wait for next input packet
			sleep(3);//wait for user to return stick
		}
		if(buf[0] == 'f' && buf[1] == 'l' && res != 0 && res != -1 && input == 0  && cat == 2){//stalk left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("r detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input fl \t uSecs= %i\n", timeuSecs);//print stats to file
			printf("input fl \t uSecs= %i\n" ,timeuSecs);
			cont = 0;
		}
		if(buf[0] == 'f' && buf[1] == 'r' && res != 0 && res != -1 && input == 1  && cat == 2){//stalk left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("r detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input fr \t uSecs= %i\n",timeuSecs);//print stats to file
			printf("input fr \t uSecs= %i\n" ,timeuSecs);
			cont = 0;
		}
		if(buf[0] == 'f' && buf[1] == 'h' && res != 0 && res != -1 && input == 2  && cat == 2){//stalk left
			gettimeofday (&foo, &bar);//get current time
			t2Secs = foo.tv_sec;
			t2uSecs = foo.tv_usec;
			printf("r detected on input\n");	
			sendPacket(9);//send a wait for next input packet
			printf("t1= %i \tt2= %i \tut1= %i \tut2= %i\n",t1Secs, t2Secs, t1uSecs, t2uSecs);
			calcTime(t1Secs, t2Secs, t1uSecs, t2uSecs);
			fprintf (file, "input fh \t uSecs= %i\n" ,timeuSecs);//print stats to file
			printf("input fh \t uSecs= %i\n" ,timeuSecs);
			cont = 0;
		}
		
	}//end while
	cont = 1;
	sleep(sleeptime);//sleep for 3 seconds until the next input is requested
	
	
	int temp;//a small loop to clear the serial buffer incase the horn was held down
	for(temp = 0; temp < 1000; temp++)
		res = read(fd,buf,255);	

}

	// left = 0
	// right = 1
	// horn =  2
	
	//cat 1 is wheel
	//cat 2 is face
	//cat 3 is stalk

//(t2Secs*t2uSecs)-(t1Secs*t1uSecs)
//int, int

void checkReturnInput(){
	
	int hasReturned = 0;
	
	while (hasReturned == 0){
		printf("in hasReturned\n");
		res = read(fd,buf,255);
		//buf[res]=0;
		if(buf[0] == 's' && buf[1] == 'r'){
			printf("in hasReturned == sr\n");
			hasReturned = 0;
		}
		else if(buf[0] == 's' && buf[1] == 'l'){
			printf("in hasReturned == sl\n");
			hasReturned = 0;
		}
		else{
			hasReturned = 1;//
		}
		usleep(1000000);
		//sleep(1);
	}
	return;
}

int waitForInput(){
	while (cont){//this block waits for an input from serial
		res = read(fd,buf,255);
		//buf[res]=0;
		if(buf[0] == 'w' && buf[1] == 'h' && res != 0 & res != -1){
			printf("H detected on input\n");	
			cont = 0;
		}
	}// end while
	cont = 1;
	return 0;
}


void init(){

 	printf("in init\n");

	fd = SERIALopen ("/dev/ttyS0", SERIAL_NONBLOCK);
	SERIALparams(fd, 9600, 7, SERIAL_LINE);

	srand( (unsigned)time( NULL ) );

	gettimeofday (&foo, &bar);

	file = fopen ("out.txt", "wt");//open a file called out.txt//wt is the properties of the time 
	fprintf (file, "This should be at the top of the file!\n");//print at the start of the file

}

void closeAll(){
	printf("trying to exit program and close all connections.\n");
	
	s.close();//close TCP connection

	fprintf(file, "\n this should be at the end of the file");
	fclose (file); //close the file
	
	SERIALclose (fd);//close the serial connection
}


int getRand (int range){

	int x;
	//int j;

	/* Set evil seed (initial seed) */
	//srand( (unsigned)time( NULL ) );

	//for (j = 0; j < 500; j++) {
	//	x = (int)rand()%10 ;//0 is included!        
	//	printf("%i\t", x);
    	//}
	x = (int)rand()%range ;
	//printf("in getRand returning =  %i range = %i\n", x,range);	

return(x);

}//end getRand


void sendPacket(int number){
	//printf("in sendpacket\n");


    	//TCPSocket s("10.0.0.5", 1234);

	//printf("after connection is made\n");
	//while (1){
    		//s.read((char *)buffer, 4096);
    		//printf("Client: Server Said: %s\n", buffer);
	//	}    
    	


/*
1= welcome
2= left
3= right
4= horn
5= wheel
6= stalk
7= face
8= rest
9= end
10=instr
11=blank
12=return stick
*/


	int i = number;
	//while(1){
		if (i == 1)
			s.write("one\n");
		if (i == 2)
			s.write("two\n");
		if (i == 3)
			s.write("three\n");
		if (i == 4)
			s.write("four\n");
		if (i == 5)
			s.write("five\n");
		if (i == 6)
			s.write("six\n");
		if (i == 7)
			s.write("seven\n");
		if (i == 8)
			s.write("eight\n");
		if (i == 9)
			s.write("nine\n");
		if (i == 10)
			s.write("ten\n");
		if (i == 11)
			s.write("eleven\n");//send a blank
		if (i == 12)
			s.write("twelve\n");//return stick
					
			//i = 0;
		

		printf("Just sent number = %i\n" , i);
		//sleep(1);
		
		i++;	
	//}//end while
	//s.close();

}

/*this is what the qtopia app uses to communicate with it's devices
	attr.c_cflag &= ~CBAUD;
	attr.c_cflag |= B19200 | CS8 | CLOCAL;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	attr.c_cflag &= ~CRTSCTS;
*/

