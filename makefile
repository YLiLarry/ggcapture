default:
	g++ -std=c++14 src/main.cc -lscreen_capture_lite -I/opt/X11/include -L/opt/X11/lib -lX11 -framework CoreFoundation -framework CoreGraphics -framework ImageIO -framework CoreMedia -framework CoreVideo -framework AVFoundation -framework AppKit

