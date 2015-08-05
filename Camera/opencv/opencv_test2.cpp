#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

int main()
{
	CvCapture* capture;
	IplImage* image;
	const char * ss ="http://192.168.1.69:9090/?action=stream?dummy=param.mjpg";


	int i, j;
	int index, RnCount,GnCount;
	int RDrawX, RDrawY;
	int GDrawX, GDrawY;


	capture = cvCaptureFromFile(ss);
	cvNamedWindow("color", 0);
	cvResizeWindow("color", 320, 240);

	if (capture) {
		while(1) {
			cvGrabFrame(capture);
			image = cvQueryFrame(capture);

			RnCount = 1;
			GnCount = 1;
			RDrawX = 0; RDrawY = 0;
			GDrawX = 0; GDrawY = 0;

			for (i=0; i<image->height; i++) {
				for (j=0; j<image->widthStep; j+=image->nChannels) {
					index = i * image->widthStep + j;
					-----------------------------------------------------------------

				}
				cvShowImage("color", image);
				cvSaveImage("/srv/ww.jpg",image);

			}
		}

		cvReleaseCapture(&capture);
		cvDestroyWindow("color");

		return 0;
	}

