#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <ggcapture.h>
#include <gginput.h>

using namespace std;
using namespace ggcapture;
using namespace gginput;

class EInput : public GGInput
{
	GGCapture* m_ggcapture;
public:
	EInput(GGCapture* ggcapture)
		: m_ggcapture(ggcapture)
	{
	}
	virtual void handleKeyDown(GGInputKey key) override
	{
		cerr << key.keyCode << endl;
		if (key.keyCode == 112) {
			GGInput::handleKeyDown(key);
			m_ggcapture->saveFrame();
		}
	}
};

class ECapture : public GGCapture
{
	void newFrameArrived(shared_ptr<CImg<unsigned char>> frame) override
	{
		saveFrame();
		GGCapture::newFrameArrived(frame);
	}
};

int main()
{
	mutex cv_mutex;
	unique_lock<mutex> cv_lock(cv_mutex);
	condition_variable cv;
	ECapture ggcapture;
	EInput gginput(&ggcapture);
	ggcapture.start("dangerous", GGCapture::DirectXDesktopDuplication);
	gginput.start();
}
