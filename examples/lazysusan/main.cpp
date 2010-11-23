



#include <iostream>

#include <wcl/tracking/LazySusan.h>

using namespace std;
using namespace wcl;


int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		cout << "Usage: lazy_susan [device]" << endl;
		return 1;
	}

	cout << "Connecting to Lazy Susan... ";
	LazySusan* ls = new LazySusan(argv[1]);
	TrackedObject* to = ls->getObject("blah");
	cout << "Done!" << endl;

	while (true)
	{
		ls->update();
		//cout << to->getOrientation().toString() << endl;
		usleep(30000);
	}
	return 0;
}
