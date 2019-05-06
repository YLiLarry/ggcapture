#include <string>
#include <ScreenCapture.h>
#include <memory>
#include <atomic>
#include <ggframe.h>
#if APPLE
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif

namespace ggcapture {

	using namespace std;
	using namespace cimg_library;
	using namespace SL::Screen_Capture;
	using namespace SL;

#if APPLE
	using boost::filesystem::path;
	using std::shared_ptr;
#else
	using std::filesystem::path;
#endif

	class GGCapture {
	public:
		enum CaptureMethod {
#if WIN32
			DirectXDesktopDuplication,
#endif
			Window
		};
		enum CaptureStatus {
			Stopped,
			Capturing
		};

	private:
		atomic<CaptureMethod> m_capture_mode = CaptureMethod::Window;
		atomic<CaptureStatus >m_status = Stopped;
		atomic<bool> m_show_frame_flag = false;
		atomic<bool> m_save_frame_flag = false;
		atomic<thread::id> m_capture_thread_id;
		string m_window_title; /* capture thread */
		path m_storage_path = "./data"; /* capture thread */
		int m_capture_fps; /* capture thread */
		shared_ptr<ICaptureConfiguration<ScreenCaptureCallback>> m_screen_capture_config; /* capture thread */
		shared_ptr<ICaptureConfiguration<WindowCaptureCallback>> m_window_capture_config; /* capture thread */
		shared_ptr<IScreenCaptureManager> m_capture_manager; /* capture thread */
		Screen_Capture::Window m_window;  /* capture thread only */

		void setWindowTitle(string srchterm); /* capture thread */
		void setCaptureMode(CaptureMethod mode); /* capture thread */
		void initCaptureConfig(); /* capture thread */
		void setCaptureFps(int fps); /* capture thread */
#if WIN32
		void updateWindowForDirectXDesktopDuplicationMode(); /* capture thread only */
#endif
	protected:
		virtual void newFrameArrived(shared_ptr<ggframe::Frame> frame); /* capture thread only */
	public:
		GGCapture() = default;
		void setStoragePath(path path); /* atomic */
		void start(string srchterm, CaptureMethod mode, int fps); /* atomic */
		void stop(); /* atomic */
		void showFrame(); /* atomic */
		void saveFrame(); /* atomic */
		CaptureStatus status() const; /* atomic */
	};

}
