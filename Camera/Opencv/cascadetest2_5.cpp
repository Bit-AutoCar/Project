#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <assert.h>
#include <wiringPi.h>
#include <wiringSerial.h>

int FORWARD = 0;
int BACK = 0;
int LEFT = 0;
int RIGHT = 0;
int STOP = 0;
int WAIT = 0;
int TRACKING = 0;
int Limit = 4;
int fd;

using namespace cv;
using namespace std;

CvHaarClassifierCascade *cascade;
CvMemStorage            *storage;

void detect(IplImage *img);

int main(int argc, char** argv)
{
	CvCapture *capture;
	IplImage  *frame;
	int input_resize_percent = 100;

	if(argc < 3)
	{
		std::cout << "Usage " << argv[0] << " finalcascade.xml video.avi" << std::endl;
	}

	if(argc == 4)
	{
		input_resize_percent = atoi(argv[3]);
		std::cout << "Resizing to: " << input_resize_percent << "%" << std::endl;
	}
	if((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		std::cout<<"No serial communication"<<endl;
		return -1 ;
	}

	cascade = (CvHaarClassifierCascade*) cvLoad(("newcascade.xml"), 0, 0, 0);
	storage = cvCreateMemStorage(0);
	capture = cvCaptureFromCAM(0);

	IplImage* frame1 = cvQueryFrame(capture);
	frame = cvCreateImage(cvSize((int)((frame1->width*input_resize_percent)/200) , (int)((frame1->height*input_resize_percent)/200)), frame1->depth, frame1->nChannels);
	const int KEY_SPACE  = 32;
	const int KEY_ESC    = 27;

	int key = 0;
	do
	{
		frame1 = cvQueryFrame(capture);
		cvFlip(frame, frame,0);
		cvFlip(frame1, frame1,0);
		cvFlip(frame, frame,1);
		cvFlip(frame1, frame1,1);
		if(!frame1)
			break;
		cvResize(frame1, frame);


		detect(frame);
		key = cvWaitKey(10);

		if(key == KEY_SPACE)
			key = cvWaitKey(0);

		if(key == KEY_ESC)
			break;

	}while(1);
	cvDestroyAllWindows();
	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);
	cvReleaseHaarClassifierCascade(&cascade);
	cvReleaseMemStorage(&storage);
	return 0;
}

void detect(IplImage *img)
{
	CvSize img_size = cvGetSize(img);
	CvSeq *object = cvHaarDetectObjects(img, cascade, storage, 1.1, 1, 0, cvSize(20, 20));

	if(object->total == 1){
		for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
		{
			CvRect *r = (CvRect*)cvGetSeqElem(object, i);
			int FB = (r->width*r->height);
			int LR = (r->x + (r->width)/2);

			cvRectangle(img,
					cvPoint(r->x, r->y),
					cvPoint(r->x + r->width, r->y + r->height),
					CV_RGB(255, 0, 0), 2, 8, 0);

			if(FB < 2500 && (LR >= 100 && LR <= 220)){
				FORWARD++;}
			else if((FB > 2500 && FB < 5000) && (LR >= 100 && LR <= 220)){
				TRACKING++;}
			else if(FB > 5000){
				BACK++;}
			else if(LR > 200){
				LEFT++;}
			else if(LR < 100){
				RIGHT++;}
		}

		if(FORWARD == 1){
			serialPutchar(fd, 49);
			std::cout << "GOGO" << std::endl;
			WAIT = 1,LEFT = 0,RIGHT =0,BACK = 0,STOP = 0;}
		else if(FORWARD == 3){
			serialPutchar(fd, 55);}
		else if(FORWARD == 4){
			serialPutchar(fd, 49);}

		if(BACK == 1){
			serialPutchar(fd, 50);
			WAIT = 5,FORWARD =0,RIGHT = 0,STOP = 0;
			serialPutchar(fd, 55);}	
		else if(BACK == 2){
			serialPutchar(fd, 55);}

		if(LEFT == 2){
			serialPutchar(fd, 54);
			std::cout << "LEFT" << std::endl;
			WAIT = 2,FORWARD =0,RIGHT = 0,STOP = 0;}
		else if(LEFT == 3){
			//serialPutchar(fd, 52);
			serialPutchar(fd, 55);}


		if(RIGHT == 2){
			serialPutchar(fd, 52);
			std::cout << "RIGHT" << std::endl;
			WAIT = 3,FORWARD = 0,LEFT = 0,STOP = 0;}
		else if(RIGHT == 3){
			//serialPutchar(fd, 54);
			serialPutchar(fd, 55);}

		if(TRACKING == 1){
			serialPutchar(fd, 55);
			std::cout << "TRACKING" << std::endl;
			WAIT = 4,FORWARD = 0,LEFT = 0,STOP = 0;}
		else if(TRACKING == 2){
			if(object->total == 0){
				serialPutchar(fd, 50);}
			else {
				serialPutchar(fd, 55);}
		}
		else if(TRACKING == 3){
			serialPutchar(fd, 55);}

	}else STOP++;

	if(STOP == 1){
		TRACKING = 0,FORWARD = 0,BACK = 0,RIGHT = 0,LEFT = 0;
		std::cout << "STOP" << std::endl;
		serialPutchar(fd, 55);}
	
	
	if((WAIT == 0) && (object->total == 0)){
		if(STOP == 10){
			serialPutchar(fd, 52);}
		else if(STOP == 12){
			serialPutchar(fd, 55);}
		else if(STOP == 13){
			serialPutchar(fd, 52);}
		else if(STOP == 14){
			serialPutchar(fd, 55);}
		else if(STOP == 17){
			serialPutchar(fd, 52);}
		else if(STOP == 18){
			serialPutchar(fd, 55);
			STOP = 0;}
	}

	else if((WAIT == 1) && (object->total == 0)){
		if(STOP == 10){
			serialPutchar(fd, 50);}
		else if(STOP == 11){
			serialPutchar(fd, 55);}
		else if(STOP == 15){
			serialPutchar(fd, 50);}
		else if(STOP == 16){
			serialPutchar(fd, 55);}
		else if(STOP == 19){
			serialPutchar(fd, 52);}
		else if(STOP == 20){
			serialPutchar(fd, 55);}
		else if(STOP == 23){
			serialPutchar(fd, 52);}
		else if(STOP == 24){
			serialPutchar(fd, 55);}
		else if(STOP == 26){
			serialPutchar(fd, 52);}
		else if(STOP == 27){
			serialPutchar(fd, 55);}
		else if(STOP == 29){
			serialPutchar(fd, 52);}
		else if(STOP == 31){
			serialPutchar(fd, 55);
			STOP = 0;}
	}
	else if((WAIT == 2) && (object->total == 0)){
		if(STOP == 10){
			serialPutchar(fd, 52);}
		else if(STOP == 11){
			serialPutchar(fd, 55);}
		else if(STOP == 15){
			serialPutchar(fd, 54);}
		else if(STOP == 17){
			serialPutchar(fd, 55);}
		else if(STOP == 19){
			serialPutchar(fd, 52);}
		else if(STOP == 22){
			serialPutchar(fd, 55);}
		else if(STOP == 24){
			serialPutchar(fd, 52);}
		else if(STOP == 25){
			serialPutchar(fd, 55);
			STOP = 0;}
	}
	else if((WAIT == 3) && (object->total == 0)){
		if(STOP == 10){
			serialPutchar(fd, 54);}
		else if(STOP == 11){
			serialPutchar(fd, 55);}
		else if(STOP == 17){
			serialPutchar(fd, 52);}
		else if(STOP == 18){
			serialPutchar(fd, 55);}
		else if(STOP == 23){
			serialPutchar(fd, 54);}
		else if(STOP == 24){
			serialPutchar(fd, 55);}
		else if(STOP == 25){
			serialPutchar(fd, 54);}
		else if(STOP == 26){
			serialPutchar(fd, 55);
			STOP = 0;}
	}
	else if((WAIT == 4) && (object->total == 0)){
		if(STOP == 10){
			serialPutchar(fd, 54);}
		else if(STOP == 12){
			serialPutchar(fd, 55);}
		else if(STOP == 14){
			serialPutchar(fd, 54);}
		else if(STOP == 15){
			serialPutchar(fd, 55);
			STOP = 0;}

	}
	else if(STOP == 50){
		serialPutchar(fd, 54);}
	else if(STOP == 52){
		serialPutchar(fd, 55);}
	else if(STOP == 54){
		serialPutchar(fd, 54);}
	else if(STOP == 56){
		serialPutchar(fd, 55);
		STOP = 0;}

	//	cvShowImage("video1", img);

}

