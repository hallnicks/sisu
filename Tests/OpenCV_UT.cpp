#include "test.hpp"
#include "threadgears.hpp"
#include "sisumath.hpp"
#include "memblock.hpp"
#include "Packet.hpp"
#include "Stopwatch.hpp"


#include <concurrentqueue.h>
#include <iostream>

#include <math.h>
#include <limits>

#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace cv;
using namespace std;
using namespace sisu;

namespace {

class OpenCV_UT : public context
{

	public:
		OpenCV_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace


TEST(OpenCV_UT, TestLinkerAndSDKIntegration)
{
    std::string in = "resources\\test.jpg";
    Mat image = imread(in, IMREAD_GRAYSCALE);
    // Create and LSD detector with standard or no refinement.
    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);
    double start = double(getTickCount());
    vector<Vec4f> lines_std;
    // Detect the lines
    ls->detect(image, lines_std);
    double duration_ms = (double(getTickCount()) - start) * 1000 / getTickFrequency();
    std::cout << "It took " << duration_ms << " ms." << std::endl;
    // Show found lines
    Mat drawnLines(image);
    ls->drawSegments(drawnLines, lines_std);
    imshow("Standard refinement", drawnLines);
    waitKey(0);
}

TEST(OpenCV_UT, TestWebcam)
{
  VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if( !cap.open( 0 ) )
    {
		std::cerr << "Failed to open webcam." << std::endl;
		exit( -1 );
    }

    for(;;)
    {
          Mat frame;
          cap >> frame;
          if( frame.empty() ) break; // end of video stream
          imshow("this is you, smile! :)", frame);
          if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
    }
    // the camera will be closed automatically upon exit
    // cap.close();
}
