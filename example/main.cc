#include <iostream>
#include <thread>
#include <atomic>
#include <ggcapture.h>
#include <gginput.h>

using namespace std;
using namespace ggcapture;
using namespace gginput;

class CustomInput : public GGInput
{
	GGCapture* m_ggcapture;
public:
	CustomInput(GGCapture* ggcapture)
		: m_ggcapture(ggcapture)
	{
	}
	virtual void handleKeyDown(GGInputKey key) override
	{
		cerr << "key pressed: " << key.keyCode << endl;
		if (key.keyCode == 112) {
			GGInput::handleKeyDown(key);
			m_ggcapture->saveFrame();
		}
	}
};

class CustomCapture : public GGCapture
{
private:
	atomic<int> m_fps = 0;
	unique_ptr<thread> m_tick_interval;
protected:
	void newFrameArrived(shared_ptr<CImg<unsigned char>> frame) override
	{
		m_fps++;
		GGCapture::newFrameArrived(frame);
	}
public:
	CustomCapture() : GGCapture() { 
		m_tick_interval = make_unique<thread>([&]() {
			while(true) {
				sleep(1);
				cerr << m_fps << " fps" << endl;
				m_fps = 0;
			}
		});
	}
	void start(string title, GGCapture::CaptureMethod mode, int fps) {
		GGCapture::start(title, mode, fps);
		m_tick_interval->detach();
	}
};

int main()
{
	CustomCapture ggcapture;
	GGInput gginput;
#if WIN32
	ggcapture.start("task manager", GGCapture::DirectXDesktopDuplication, 10);
#elif APPLE 
	ggcapture.start("activity monitor", GGCapture::Window, 10);
#endif
	gginput.start();
}
