#include <stdio.h>
#include <time.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
 
#define PI 3.1416
#define WINSIZE 7            //optical flow를 수행할 윈도우의 사이즈.  여기서는 7*7
#define MAX_COUNT 100  //flow 할 피처 포인트 몇개나 사용하는가
 
void main()
{ 
 int pressed_key;
 int width, height;
 
 IplImage* src_color_R; 
 IplImage* src_grey_R;
 
 IplImage* pre_src_color_R;  //t-1의 이미지 프레임
 IplImage* pre_src_grey_R;  //t-1의 이미지 프레임
 
 IplImage* result_view_R;
 
 CvCapture* capture_R = cvCaptureFromCAM(0);  //웹캠 사용함
 cvNamedWindow( "Optical Flow", CV_WINDOW_AUTOSIZE ); 
 cvMoveWindow("Optical Flow",0,0);
 
 //첫 이미지 받아오기
 src_color_R = cvQueryFrame( capture_R );
 
 width  = src_color_R->width;    //소스 이미지 구조체에서 이미지 폭 불러오기
 height = src_color_R->height;   //소스 이미지 구조체에서 이미지 높이 불러오기
 
 printf("image width : %d    height : %d  \n",width,height);  //이미지의 크기를 출력하기
 
 src_grey_R        = cvCreateImage(cvSize(width,height), 8, 1);
 
 pre_src_color_R = cvCreateImage(cvSize(width,height), 8, 3);   
 pre_src_grey_R  = cvCreateImage(cvSize(width,height), 8, 1);
 
 result_view_R     = cvCreateImage(cvSize(width,height), 8, 3); 
 
 //goodfeature to track 
 //int MAX_COUNT = 100;  //flow 할 피처 포인트 몇개나 사용하는가
 int num_of_pts_R;
 int pre_num_of_pts_R;
 
 IplImage* eig_R  = cvCreateImage(cvSize(width,height), 32, 1 );
 IplImage* temp_R = cvCreateImage(cvSize(width,height), 32, 1 );
 
 //점들이 저장되는곳  그냥 간단히 매우 크게 지정  
 CvPoint2D32f feature_points_R[5000];
 CvPoint2D32f pre_feature_points_R[5000];
 
 //OpenCV 화면에 글자쓰기
 char s_output_result[50];
 CvFont font;
 
 //수행시간 측정용
 clock_t start, finish; //수행시간체크
 double duration;
 
 //Optical Flow
 char* status_R = 0;
 status_R = new char [MAX_COUNT];
 int flags_R=0;
 
 IplImage* pyramid_R        = cvCreateImage(cvSize(width,height), 8, 1);   
 IplImage* prev_pyramid_R = cvCreateImage(cvSize(width,height), 8, 1);  
 
 result_view_R->origin= 0;  //이미지 위아래 뒤집어 보이게 하기
 
 // initial work  첫 이미지는 전 이미지 정보가 없어서 해주는거
 
 //goodfeature to track
 double quality = 0.01;
 double min_distance = 10;
 num_of_pts_R = MAX_COUNT;
 
 src_color_R = cvQueryFrame( capture_R );
 cvCvtColor(src_color_R, src_grey_R, CV_BGR2GRAY);
 
 //tracking하기 좋은 점(corner)들을 찾음
 cvGoodFeaturesToTrack( src_grey_R, eig_R, temp_R, feature_points_R, &num_of_pts_R, quality, min_distance, 0, WINSIZE, 0, 0.04 );
 
 cvFindCornerSubPix(src_grey_R, feature_points_R, num_of_pts_R, cvSize(WINSIZE,WINSIZE), cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));  
 //점들을 draw
 for(int i=0; i<num_of_pts_R; ++i) 
  cvCircle( result_view_R, cvPointFrom32f(feature_points_R[i]), 1, CV_RGB(0,255,0), -1, 4,0);
 //다음 씬 처리 위해 지금 영상 이전 영상으로 저장
 cvCopy(src_grey_R, pre_src_grey_R);
 cvCopy(pyramid_R, prev_pyramid_R);
 
 //현재 찾은 점 좌표 저장
 for(int i=0; i<num_of_pts_R; ++i) 
  pre_feature_points_R[i] = feature_points_R[i];
 pre_num_of_pts_R = num_of_pts_R;
 
 while(1)
 {
  start=clock();  //연산시간측정
  src_color_R = cvQueryFrame( capture_R );
  cvCopy(src_color_R, result_view_R);
 
  cvCvtColor(src_color_R, src_grey_R, CV_BGR2GRAY);
 
  if(pre_num_of_pts_R>0)
  {
   printf("opflow start\n");
   cvCalcOpticalFlowPyrLK( pre_src_grey_R, src_grey_R, prev_pyramid_R, pyramid_R, pre_feature_points_R, feature_points_R, num_of_pts_R, cvSize(WINSIZE,WINSIZE), 3, status_R, 0,cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags_R );
   flags_R |= CV_LKFLOW_PYR_A_READY;
 
   int valid_pt_R=0;  //valid한 점의 개수 count
   for(int i=0; i<num_of_pts_R; ++i)  //그 point가 valid 한지
   {
    if(status_R[i]!=0)
     ++valid_pt_R;
   }
 
   printf("valid_pts_R:%d\n",valid_pt_R);
   sprintf(s_output_result,"valid points : %d",valid_pt_R );    //우선 sprintf로 문자열 생성
   cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.5, 0.5, 0, 1);  //이런 저런 설정.
   cvPutText(result_view_R, s_output_result ,cvPoint(15,height-20),&font,cvScalar(0,255,0));   //cvPoint로 글자 시작 위치 설정(uv)
 
   //draw result, 화살표 모양으로 그리고 실제 움직인 거리의 3배 길이로 화살표 그려줌
   for(int i=0; i<num_of_pts_R; i++)
   {     
    if ( status_R[i] == 0 )  //valid 한 점이 아니면 넘어감
     continue;
 
    int line_thickness = 1;
    CvScalar line_color = CV_RGB(255,0,0);
 
    CvPoint p,q;
    p.x = (int) pre_feature_points_R[i].x;
    p.y = (int) pre_feature_points_R[i].y;
    q.x = (int) feature_points_R[i].x;
    q.y = (int) feature_points_R[i].y;
 
    double angle; 
    angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
    double arrow_length; 
    arrow_length = sqrt( pow((float)(p.y - q.y),2) + pow((float)(p.x - q.x),2) );
    q.x = (int) (p.x - 3 * arrow_length * cos(angle));  //3배 길이로 화살표 그려줌
    q.y = (int) (p.y - 3 * arrow_length * sin(angle));
 
    //일정길이이하 화살표는 그리지 않는다.
    if((arrow_length<10)|(arrow_length>40)) //화살표 길이가 너무짧거나 길면 안그린다.
     continue;
 
    //draw arrow
    cvLine( result_view_R, p, q, line_color, line_thickness, CV_AA, 0 );
    p.x = (int) (q.x + 5 * cos(angle + PI / 4));
    if(p.x>=width)
     p.x=width-1;
    else if(p.x<0)
     p.x=0;
    p.y = (int) (q.y + 5 * sin(angle + PI / 4));
    if(p.y>=height)
     p.y=height-1;
    else if(p.y<0)
     p.y=0;
    cvLine( result_view_R, p, q, line_color, line_thickness, CV_AA, 0 );
    p.x = (int) (q.x + 5 * cos(angle - PI / 4));
    if(p.x>=width)
     p.x=width-1;
    else if(p.x<0)
     p.x=0;
    p.y = (int) (q.y + 5 * sin(angle - PI / 4));
    if(p.y>height)
     p.y=height-1;
    else if(p.y<0)
     p.y=0;
    cvLine( result_view_R, p, q, line_color, line_thickness, CV_AA, 0 );
   }   
  }
 
  //goodfeature to track
  num_of_pts_R = MAX_COUNT;
 
  cvGoodFeaturesToTrack( src_grey_R, eig_R, temp_R, feature_points_R, &num_of_pts_R, quality, min_distance, 0, WINSIZE, 0, 0.04 );
  cvFindCornerSubPix(src_grey_R, feature_points_R, num_of_pts_R, cvSize(WINSIZE,WINSIZE), cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));  
  printf("max_count_R : %d\n",num_of_pts_R);
  for(int i=0; i<num_of_pts_R; ++i)  
   cvCircle( result_view_R, cvPointFrom32f(feature_points_R[i]), 1, CV_RGB(0,255,0), -1, 4,0);
  //다음 씬 처리 위해 지금영상 저장
  cvCopy(src_grey_R, pre_src_grey_R);
  cvCopy(pyramid_R, prev_pyramid_R);
 
  //현재 찾은 점 좌표 저장
  for(int i=0; i<num_of_pts_R; ++i)
   pre_feature_points_R[i] = feature_points_R[i];
 
  pre_num_of_pts_R = num_of_pts_R;
 
  finish=clock();
 
  duration=(double)(finish-start)/CLOCKS_PER_SEC;
 
  sprintf(s_output_result,"time : %f",duration );    //우선 sprintf로 문자열 생성
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.5, 0.5, 0, 1);  //이런 저런 설정.
  cvPutText(result_view_R, s_output_result ,cvPoint(width-150,height-20),&font,cvScalar(0,255,0));   //cvPoint로 글자 시작 위치 설정(uv)
 
  cvShowImage( "Optical Flow", result_view_R );
 
  pressed_key = cvWaitKey(30);
  if(pressed_key == 'q')
   break;
 }
 
 cvDestroyAllWindows(); 
 
 cvReleaseCapture(&capture_R);
 
 if(src_grey_R)
  cvReleaseImage(&src_grey_R);
 if(pre_src_color_R)
  cvReleaseImage(&pre_src_color_R);
 if(result_view_R)
  cvReleaseImage(&result_view_R);
 
 //goodfeature to track 
 if(eig_R)
  cvReleaseImage( &eig_R );
 if(temp_R)
  cvReleaseImage( &temp_R );
 
 if(status_R)
  delete [] status_R;
 
}
