#ifdef SISU_OPENCV
#include "test.hpp"
#include "threadgears.hpp"
#include "sisumath.hpp"
#include "memblock.hpp"
#include "Packet.hpp"
#include "Stopwatch.hpp"


#include <concurrentqueue.h>
#include <iostream>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN 1
    #define NOMINMAX 1
    #include <windows.h>
#endif
#if defined(_WIN64)
    #include <windows.h>
#endif

#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <limits>

#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/core/opengl.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/highgui.hpp"

#include "gltools.hpp"

using namespace cv;
using namespace cv::cuda;
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

struct DrawData
{
    ogl::Arrays arr;
    ogl::Texture2D tex;
    ogl::Buffer indices;
};

#ifndef OPENGLES

void draw(void* userdata);

void draw(void* userdata)
{
    DrawData* data = static_cast<DrawData*>(userdata);

    glRotated(0.6, 0, 1, 0);

    ogl::render(data->arr, data->indices, ogl::TRIANGLES);
}
#endif

const int win_width = 800;
const int win_height = 640;

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

#if 0
#ifndef OPENGLES
TEST(OpenCV_UT, TestOpenGL)
{
    string filename = "resources\\test.jpg";

    Mat img = imread(filename);
    if (img.empty())
    {
        cerr << "Can't open image " << filename << endl;
	exit( -1 );
    }

    namedWindow("OpenGL", WINDOW_OPENGL);
    resizeWindow("OpenGL", win_width, win_height);

    Mat_<Vec2f> vertex(1, 4);
    vertex << Vec2f(-1, 1), Vec2f(-1, -1), Vec2f(1, -1), Vec2f(1, 1);

    Mat_<Vec2f> texCoords(1, 4);
    texCoords << Vec2f(0, 0), Vec2f(0, 1), Vec2f(1, 1), Vec2f(1, 0);

    Mat_<int> indices(1, 6);
    indices << 0, 1, 2, 2, 3, 0;

    DrawData data;

    data.arr.setVertexArray(vertex);
    data.arr.setTexCoordArray(texCoords);
    data.indices.copyFrom(indices);
    data.tex.copyFrom(img);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspectiveGL(45.0, (double)win_width / win_height, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lookAtGL( { 0, 0, 3}, { 0, 0, 0 }, { 0, 1, 0 } );

    glEnable(GL_TEXTURE_2D);
    data.tex.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_CULL_FACE);

    setOpenGlDrawCallback("OpenGL", draw, &data);

    for (;;)
    {
        updateWindow("OpenGL");
        int key = waitKey(40);
        if ((key & 0xff) == 27)
            break;
    }

    setOpenGlDrawCallback("OpenGL", 0, 0);
    destroyAllWindows();
}
#endif // OPENGLES
#endif // 0
#endif // SISU_OPENCV
