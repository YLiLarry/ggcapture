#include <ScreenCapture.h>
#include <iostream>
#include <CImg.h>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace SL::Screen_Capture;
using namespace cimg_library;

int main() 
{//Setup Screen Capture for all monitors
    int i = 0;
    unsigned char* dst;
    CImg<unsigned char>* output;
    condition_variable cv;
    mutex cv_mutex;
    unique_lock<mutex> cv_lock(cv_mutex);

auto framgrabber =  SL::Screen_Capture::CreateCaptureConfiguration([]() {
    //add your own custom filtering here if you want to capture only some monitors
    auto windows = SL::Screen_Capture::GetWindows();
    std::string srchterm = "~/lab/ggcapture — -bash";
    // convert to lower case for easier comparisons
    std::transform(srchterm.begin(), srchterm.end(), srchterm.begin(), [](char c) { return std::tolower(c, std::locale());});
    decltype(windows) filtereditems;
    for(auto& a : windows) {
        std::string name = a.Name;
        cerr << name << endl;
        std::transform(name.begin(), name.end(), name.begin(), [](char c) {return std::tolower(c, std::locale()); });
        if(name.find(srchterm) != std::string::npos) {
        filtereditems.push_back(a);
        }
    }
    return filtereditems;
  })->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Window& window) {
    int h = Height(img);
    int w = Width(img);
    output = new CImg<unsigned char>(w,h,1,3,0);
    auto imgsrc = StartSrc(img);
    for (auto h = 0; h < Height(img); h++) {
        auto startimgsrc = imgsrc;
        for (auto w = 0; w < Width(img); w++) {
            *output->data(w,h,0,0) = (unsigned char)imgsrc->R;
            *output->data(w,h,0,1) = (unsigned char)imgsrc->G;
            *output->data(w,h,0,2) = (unsigned char)imgsrc->B;
            imgsrc++;
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
    // output->save("output.bmp");
    delete output;
    cerr << "new frame" << endl;
  })->start_capturing();

framgrabber->setFrameChangeInterval(std::chrono::milliseconds(100));//100 ms
framgrabber->setMouseChangeInterval(std::chrono::milliseconds(100));//100 ms

cv.wait(cv_lock);


}
