#include <string>
#include <ScreenCapture.h>
#include <CImg.h>
#include <filesystem>
#include <memory>
#include <atomic>

namespace ggcapture {

	using namespace std;
	using namespace cimg_library;
	using namespace SL::Screen_Capture;
	using namespace SL;

	class GGCapture {
	public:
		enum CaptureMethod {
			DirectXDesktopDuplication,
			Window
		};
		enum Status {
			Stopped,
			Capturing
		};

	private:
		CaptureMethod m_capture_mode = CaptureMethod::Window;
		string m_window_title;
		Status m_status = Stopped;
		filesystem::path m_storage_path = "./data";
		int m_capture_fps = 10;
		atomic<bool> m_show_frame_flag = false;
		CImgDisplay m_image_display;
		shared_ptr<ICaptureConfiguration<ScreenCaptureCallback>> m_capture_config;
		shared_ptr<IScreenCaptureManager> m_capture_manager;
		Screen_Capture::Window m_window;

		void setWindowTitle(string srchterm);
		void setCaptureMode(CaptureMethod mode);
		void initCaptureConfig();
		void setCaptureFps(int fps);
		void updateWindowPosition();
	protected:
		virtual void newFrameArrived(shared_ptr<CImg<unsigned char>> frame);
	public:
		GGCapture() = default;
		void setStoragePath(filesystem::path path);
		void start(string srchterm, CaptureMethod mode);
		void stop();
		void showFrame();
	};

}
