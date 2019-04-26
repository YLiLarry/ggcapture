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
		atomic<CaptureMethod> m_capture_mode = CaptureMethod::Window;
		atomic<Status >m_status = Stopped;
		atomic<bool> m_show_frame_flag = false;
		atomic<bool> m_save_frame_flag = false;
		atomic<thread::id> m_capture_thread_id;
		string m_window_title; /* capture thread */
		filesystem::path m_storage_path = "./data"; /* capture thread */
		int m_capture_fps = 10; /* capture thread */
		CImgDisplay m_image_display; /* capture thread only */
		shared_ptr<ICaptureConfiguration<ScreenCaptureCallback>> m_capture_config; /* capture thread */
		shared_ptr<IScreenCaptureManager> m_capture_manager; /* capture thread */
		Screen_Capture::Window m_window;  /* capture thread only */

		void setWindowTitle(string srchterm); /* capture thread */
		void setCaptureMode(CaptureMethod mode); /* capture thread */
		void initCaptureConfig(); /* capture thread */
		void setCaptureFps(int fps); /* capture thread */
		void updateWindow(); /* capture thread only */
	protected:
		virtual void newFrameArrived(shared_ptr<CImg<unsigned char>> frame); /* capture thread only */
	public:
		GGCapture() = default;
		void setStoragePath(filesystem::path path); /* atomic */
		void start(string srchterm, CaptureMethod mode); /* atomic */
		void stop(); /* atomic */
		void showFrame(); /* atomic */
		void saveFrame(); /* atomic */
		Status status() const; /* atomic */
	};

}
