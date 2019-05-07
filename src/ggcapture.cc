#include <ggcapture.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <queue>

#if WIN32
#include <Windows.h>
#endif

using namespace ggcapture;
using namespace ggframe;
using namespace std;
using namespace SL;
using namespace SL::Screen_Capture;
using namespace chrono;
using ggframe::Frame;	

#if APPLE
	using boost::filesystem::path;
	using boost::filesystem::create_directory;
	using std::shared_ptr;
#else
	using std::filesystem::path;
	using std::filesystem::create_directory;
#endif

void GGCapture::setCaptureMode(GGCapture::CaptureMethod mode)
{
	m_capture_mode = mode;
}

void GGCapture::setCaptureFps(int fps)
{
	m_capture_fps = fps;
}

#if WIN32
void GGCapture::updateWindowForDirectXDesktopDuplicationMode()
{
	assert(this_thread::get_id() == m_capture_thread_id);
	RECT rect = { 0 };
	bool succeed = GetWindowRect(reinterpret_cast<HWND>(m_window.Handle), &rect);
	if (! succeed) {
		// window does not exist
		return;
	}
	m_window.Position.x = rect.left;
	m_window.Position.y = rect.top;
	m_window.Size.x = rect.right - rect.left;
	m_window.Size.y = rect.bottom - rect.top;
}
#endif

void GGCapture::initCaptureConfig()
{
#if WIN32
	if (m_capture_mode == CaptureMethod::DirectXDesktopDuplication) {
		m_screen_capture_config = SL::Screen_Capture::CreateCaptureConfiguration([]() {
			vector<Screen_Capture::Monitor> rtv;
			rtv.push_back(SL::Screen_Capture::GetMonitors()[0]);
			return rtv;
		})->onNewFrame([&](const SL::Screen_Capture::Image& img, auto window) {
			m_capture_thread_id = this_thread::get_id();
			updateWindowForDirectXDesktopDuplicationMode();
			int left = Screen_Capture::OffsetX(m_window);
			int top = Screen_Capture::OffsetY(m_window);
			int width = Screen_Capture::Width(m_window);
			int height = Screen_Capture::Height(m_window);
			int full_h = Screen_Capture::Height(img);
			int full_w = Screen_Capture::Width(img);
			shared_ptr<Frame> frame;
			if (width > 0 && height > 0) {
				frame = make_shared<Frame>(full_h, full_w);
				size_t size = sizeof(CV_8UC4) * frame->nCols() * frame->nRows();
				Screen_Capture::Extract(img, frame->data(), size);
				ggframe::Rec rec(top, left, width, height);
				frame->crop(rec);
			} else {
				frame = make_shared<Frame>();
			}			
			newFrameArrived(frame);
		});
		return;
	}
#endif
	if (m_capture_mode == CaptureMethod::Window) {
		m_window_capture_config = SL::Screen_Capture::CreateCaptureConfiguration([&]() {
			std::vector<SL::Screen_Capture::Window> rtv = { m_window };
			return rtv;
		})->onNewFrame([&](const SL::Screen_Capture::Image& img, auto window) {
			m_capture_thread_id = this_thread::get_id();
			int full_w = Screen_Capture::Width(img);
			int full_h = Screen_Capture::Height(img);
			shared_ptr<Frame> frame;
			if (full_w > 0 && full_h > 0) {
				frame = make_shared<Frame>(full_h, full_w);
				size_t size = sizeof(CV_8UC4) * frame->nCols() * frame->nRows();
				Screen_Capture::Extract(img, frame->data(), size);
			} else {
				frame = make_shared<Frame>();
			}	
			newFrameArrived(frame);
		});
		return;
	} 
	assert("invaid capture method" && false);
}

void GGCapture::newFrameArrived(shared_ptr<Frame> frame)
{
	/* on capturing thread */
	if (frame->empty()) {
		return;
	}
	if (m_show_frame_flag) {
		pushFrameQueue(frame);
	}
	if (m_save_frame_flag) {
		char s[30];
		time_t now = std::time(nullptr);
		std::strftime(s, sizeof(s), "%Y-%m-%d-%H-%M-%S", std::localtime(&now));
		create_directory(m_storage_path);
		path filepath = m_storage_path;
		filepath /= string(s) + ".bmp";
		frame->save(filepath.string().c_str());
		m_save_frame_flag = false;
	}
}

void GGCapture::setWindowTitle(string srchterm)
{
	m_window_title = srchterm;
	vector<Screen_Capture::Window> windows = SL::Screen_Capture::GetWindows();
	cerr << "looking for window named: " << srchterm << endl;
	// convert to lower case for easier comparisons
	std::transform(srchterm.begin(), srchterm.end(), srchterm.begin(), [](char c) { return std::tolower(c); });
	for (auto& a : windows) {
		std::string name = a.Name;
		std::transform(name.begin(), name.end(), name.begin(), [](char c) {return std::tolower(c); });
		if (name.find(srchterm) != std::string::npos) {
			cerr << "found: " << name << endl;
			m_window = a;
			m_window_title = name;
			return;
		}
	}
	cerr << "window not found, existing windows are:" << endl;
	for (auto& a : windows) {
		cerr << a.Name << endl;
	}
}

void GGCapture::start(string srchterm, CaptureMethod mode, int fps)
{
	setCaptureFps(fps);
	setCaptureMode(mode);
	setWindowTitle(srchterm);
	initCaptureConfig();
#if WIN32
	if (mode == CaptureMethod::DirectXDesktopDuplication) {
		m_capture_manager = m_screen_capture_config->start_capturing();
	} 
#endif
	if (mode == CaptureMethod::Window) {
		m_capture_manager = m_window_capture_config->start_capturing();
	}
	m_capture_manager->setFrameChangeInterval(chrono::milliseconds(1000 / m_capture_fps));
	m_status = CaptureStatus::Capturing;
}

void GGCapture::stop()
{
	m_capture_manager->pause();
	m_status = CaptureStatus::Stopped;
}

void GGCapture::showFrame()
{
	m_show_frame_flag = true;
	while(true) {
		shared_ptr<Frame> frame = removeFrameQueue();
		if (! frame->empty()) {
			frame->display();
		}
	}
}

void GGCapture::pushFrameQueue(shared_ptr<Frame> frame)
{
	m_frame_queue_mutex.lock();
	m_frame_queue.push(frame);
	m_frame_queue_mutex.unlock();
}

shared_ptr<Frame> GGCapture::removeFrameQueue()
{
	shared_ptr<Frame> frame;
	m_frame_queue_mutex.lock();
	if (m_frame_queue.empty()) {
		frame = make_shared<Frame>();
	} else {
		frame = m_frame_queue.front();
		m_frame_queue.pop();
	}
	m_frame_queue_mutex.unlock();
	return frame;
}

void GGCapture::setStoragePath(path path)
{
	m_storage_path = path;
}

void GGCapture::saveFrame()
{
	m_save_frame_flag = true;
}

GGCapture::CaptureStatus GGCapture::status() const
{
	return m_status;
}

void GGCapture::setPixelDensity(float density)
{
	m_pixel_density = density;
}

float GGCapture::pixelDensity() const
{
	return m_pixel_density;
}
