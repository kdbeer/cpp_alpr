//C++
#include <iostream>
#include <sstream>

//opencv
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;
using namespace std;

// Global variables
Mat frame, fgMaskMOG2, background, element; //fg mask fg mask generated by MOG2 method
Ptr<BackgroundSubtractor> pMOG2;            //MOG2 Background subtractor

Rect region_of_interest = Rect(400, 700, 1350, 300);

//string car_cascade_name = "cars.xml";
//CascadeClassifier car_cascade;

//Init interface
void processVideo(string videoFilename);
void processImages(char *firstFrameFilename);
void Morphology_Operations();
void Morphology_Operations_2(int, void *);
static void drawOptFlowMap(const Mat &flow, Mat &cflowmap, int step, double, const Scalar &color);

string cars_cascade_name = "cars.xml";
string videoFilename = "../test_videos/onsite vid.avi";
string window_name = "FG Mask MOG 2";
CascadeClassifier cars_cascade;
Mat hsv[3];

//Edge
int scale = 1;
int delta = 0;
int ddepth = CV_16S;
int keyboard; //input from keyboard
bool pause = false;

int main(int argc, char *argv[])
{
  //Load images
  background = imread("/home/kridsada/Documents/LicenPLateProject/cpp_alpr/build/background.jpg", 0);
  element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
  //Load cascade

  //create Background Subtractor objects
  pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
  processVideo(videoFilename);

  //destroy GUI windows
  destroyAllWindows();
  return EXIT_SUCCESS;
}

void processVideo(string videoFilename)
{
  //create the capture object
  VideoCapture capture(videoFilename);
  if (!capture.isOpened())
  {
    //error in opening the video input
    cerr << "Unable to open video file: " << videoFilename << endl;
    exit(EXIT_FAILURE);
  }
  //read input data. ESC or 'q' for quitting
  while ((char)keyboard != 'q' && (char)keyboard != 27)
  {
    // pause processing
    keyboard = waitKey(30);
    if ((char)keyboard == 'p')
    {
      cerr << "pause";
      pause = !pause;
      keyboard = 0;
    }
    else if ((char)keyboard == 's')
    {
      imwrite("background.jpg", fgMaskMOG2);
      cerr << "saved!!";
      keyboard = 0;
    }

    if (pause)
      continue;

    //read the current frame
    if (!capture.read(frame))
    {
      cerr << "Unable to read next frame." << endl;
      cerr << "Exiting..." << endl;
      exit(EXIT_FAILURE);
    }

    //Crop image
    frame = frame(region_of_interest);
    cvtColor(frame, fgMaskMOG2, CV_BGR2HSV);
    split(fgMaskMOG2, hsv);
    fgMaskMOG2 = hsv[2];
    // Backgound subtraction
    //subtract(fgMaskMOG2, background, fgMaskMOG2);
    //morphologyEx(fgMaskMOG2, fgMaskMOG2, MORPH_TOPHAT, element, Point(-1, -1));
    // equalizeHist(fgMaskMOG2, fgMaskMOG2);
    // GaussianBlur( fgMaskMOG2,fgMaskMOG2, Size( 11, 11 ), 0, 0 );
    imshow("Display frame", fgMaskMOG2);
  }
  //delete capture object
  capture.release();
}

/**
  * @function Morphology_Operations
  */
void Morphology_Operations()
{
  // Since MORPH_X : 2,3,4,5 and 6
  int operation = 1 + 2;

  Mat element = getStructuringElement(0, Size(2 * 3 + 1, 2 * 3 + 1), Point(5, 5));

  /// Apply the specified morphology operation
  morphologyEx(fgMaskMOG2, fgMaskMOG2, operation, element);
}

void Morphology_Operations_2(int, void *)
{
  // Since MORPH_X : 2,3,4,5 and 6
  int operation = 0 + 2;

  Mat element = getStructuringElement(1, Size(2 * 1 + 1, 2 * 1 + 1), Point(1, 1));

  /// Apply the specified morphology operation
  morphologyEx(fgMaskMOG2, fgMaskMOG2, operation, element);
}

static void drawOptFlowMap(const Mat &flow, Mat &cflowmap, int step,
                           double, const Scalar &color)
{
  for (int y = 0; y < cflowmap.rows; y += step)
    for (int x = 0; x < cflowmap.cols; x += step)
    {
      const Point2f &fxy = flow.at<Point2f>(y, x);
      line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
           color);
      circle(cflowmap, Point(x, y), 2, color, -1);
    }
}