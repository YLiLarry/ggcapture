#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <ggcapture.h>

using namespace std;
using namespace ggcapture;
int main()
{
	mutex cv_mutex;
	unique_lock<mutex> cv_lock(cv_mutex);
	condition_variable cv;
	GGCapture gg;
	gg.start("elite - dangerous", GGCapture::DirectXDesktopDuplication);
	gg.showFrame();
	cv.wait(cv_lock);
}
