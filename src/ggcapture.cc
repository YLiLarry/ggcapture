#include "ggcapture.h"
#include <iostream>

using namespace ggcapture;
using namespace std;
using namespace cimg_library;
using namespace filesystem;
using namespace SL;
using namespace SL::Screen_Capture;

void GGCapture::setCaptureMode(GGCapture::CaptureMethod mode)
{
	m_capture_mode = mode;
}

void GGCapture::setCaptureFps(int fps)
{
	m_capture_fps = fps;
}

void GGCapture::updateWindowPosition()
{
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

void GGCapture::initCaptureConfig()
{
	if (m_capture_mode == CaptureMethod::DirectXDesktopDuplication) {
		m_capture_config = SL::Screen_Capture::CreateCaptureConfiguration([]() {
			return SL::Screen_Capture::GetMonitors();
		})->onNewFrame([&](const SL::Screen_Capture::Image& img, auto window) {
			updateWindowPosition();
			int offset_x = Screen_Capture::OffsetX(m_window);
			int offset_y = Screen_Capture::OffsetY(m_window);
			int size_x = Screen_Capture::Width(m_window);
			int size_y = Screen_Capture::Height(m_window);
			int end_x = size_x + offset_x;
			int end_y = size_y + offset_y;
			int full_h = Screen_Capture::Height(img);
			int full_w = Screen_Capture::Width(img);
			offset_x = min(max(offset_x, 0), full_w);
			offset_y = min(max(offset_y, 0), full_h);
			end_x = min(max(end_x, 0), full_w);
			end_y = min(max(end_y, 0), full_h);
			shared_ptr<CImg<unsigned char>> frame;
			if (size_x > 0 && size_y > 0) {
				frame = make_shared<CImg<unsigned char>>(full_w, full_h, 1, 3, 0);
				CImg<unsigned char>& output = *frame;
				ImageBGRA const* imgsrc = StartSrc(img);
				for (int h = 0; h < full_h; h++) {
					ImageBGRA const* startimgsrc = imgsrc;
					for (int w = 0; w < full_w; w++) {
						output(w, h, 0, 0) = imgsrc->R;
						output(w, h, 0, 1) = imgsrc->G;
						output(w, h, 0, 2) = imgsrc->B;
						imgsrc++;
					}
					imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
				}
				frame->crop(offset_x, offset_y, end_x, end_y);
			} else {
				frame = make_shared<CImg<unsigned char>>();
			}			
			newFrameArrived(frame);
		});
		return;
	}
	assert("Not implemented" && false);
}

void GGCapture::newFrameArrived(shared_ptr<CImg<unsigned char>> frame)
{
	if (frame->is_empty()) {
		return;
	}
	if (m_show_frame_flag) {
		m_image_display.show();
		m_show_frame_flag = false;
	}
	m_image_display.resize(frame->width(), frame->height());
	m_image_display.display(*frame);
}

void GGCapture::setWindowTitle(string srchterm)
{
	m_window_title = srchterm;
	vector<Screen_Capture::Window> windows = SL::Screen_Capture::GetWindows();
	cout << "looking for window named: " << srchterm << endl;
	// convert to lower case for easier comparisons
	std::transform(srchterm.begin(), srchterm.end(), srchterm.begin(), [](char c) { return std::tolower(c); });
	for (auto& a : windows) {
		std::string name = a.Name;
		std::transform(name.begin(), name.end(), name.begin(), [](char c) {return std::tolower(c); });
		if (name.find(srchterm) != std::string::npos) {
			cout << "found: " << name << endl;
			m_window = a;
			m_window_title = name;
			break;
		}
	}
}

void GGCapture::start(string srchterm, CaptureMethod mode)
{
	setCaptureMode(mode);
	setWindowTitle(srchterm);
	initCaptureConfig();
	m_capture_manager = m_capture_config->start_capturing();
	m_capture_manager->setFrameChangeInterval(chrono::milliseconds(1000 / m_capture_fps));
	m_status = Status::Capturing;
}

void GGCapture::stop()
{
	m_capture_manager->pause();
	m_status = Status::Stopped;
}

void GGCapture::showFrame()
{
	m_show_frame_flag = true;
}

void GGCapture::setStoragePath(filesystem::path path)
{
	m_storage_path = path;
}
