#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <ggcapture.h>
#include <gginput.h>

using namespace std;
using namespace ggcapture;
using namespace gginput;
using namespace ggframe;

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
	void newFrameArrived(shared_ptr<ggframe::Frame> frame) override
	{
		m_fps++;
		cerr << "+";
		GGCapture::newFrameArrived(frame);
	}
public:
	CustomCapture() : GGCapture() { 
		m_tick_interval = make_unique<thread>([&]() {
			while(true) {
				this_thread::sleep_for(chrono::seconds(1));
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
	ggcapture.showFrame();
#if WIN32
	ggcapture.start("task manager", GGCapture::DirectXDesktopDuplication, 60);
#elif APPLE 
	ggcapture.start("activity monitor", GGCapture::Window, 30);
#endif
	cerr << "press any key to exit" << endl;
	string in;
	cin >> in; 
}
