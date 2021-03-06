#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <ggcapture.h>

using namespace std;
using namespace ggcapture;
using namespace ggframe;

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
#if WIN32
    ggcapture.start("task manager", GGCapture::DirectXDesktopDuplication, 60);
#elif APPLE 
    ggcapture.start("activity monitor", GGCapture::Window, 30);
#endif
    ggcapture.showFrame();
    cerr << "press any key to exit" << endl;
    string in;
    cin >> in; 
}
