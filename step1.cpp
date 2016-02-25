#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

RNG rng(12345);

float euclideanDist(Point& p, Point& q) {
    Point diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

Mat redDetect(Mat img)
{

  int m, n;
  Mat img2(img.rows, img.cols, CV_8UC1);
  for (m = 0; m < img.rows; m++)
  {
    for (n = 0; n < img.cols; n++)
    {
      if (img.at<Vec3b>(m, n)[2] > 180 && img.at<Vec3b>(m, n)[2] > img.at<Vec3b>(m, n)[1]+ 20 &&  img.at<Vec3b>(m, n)[2] > img.at<Vec3b>(m, n)[0]+ 20)
        img2.at<uchar>(m, n) = 255;
      else
        img2.at<uchar>(m, n) = 0;
    }
  }
  return img2;
}

int main(int argc, char** argv)
{
const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";
Mat src = imread(filename, 0);
Mat srcclr = imread(filename, CV_LOAD_IMAGE_COLOR);

 if(src.empty())
 {
     cout << "can not open " << filename << endl;
     return -1;
 }

 Mat dst, cdst,srchsv,srcred,img_final;
 //cvtColor(srcclr, srchsv, CV_BGR2HSV);
 srcred=redDetect(srcclr);
 vector<vector<Point> > contours;
 vector<Vec4i> hierarchy;
 Canny(srcred, dst, 50, 200, 3);
 cvtColor(dst, cdst, CV_GRAY2BGR);
 findContours( srcred, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
 vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
 Mat drawing = Mat::zeros( dst.size(), CV_8UC3 );

vector<vector<Point> > newcontoursvector;

Point prev;
prev.x = 0;
prev.y = 0;
vector<Point > newcontours,draw,anothervar,largestContour;
Point2f outputQuad[4],inputQuad[4];
int maxlength=0;
for (auto iter : contours)
   {
    if(contourArea(iter)>maxlength)
    {
      maxlength=contourArea(iter);
      largestContour=iter;
    }
  }


    for (auto v : largestContour)
      {
        anothervar.push_back(v);
        if (v.x==prev.x || v.y==prev.y)
        {
           //cout<<"Duplicate"<<endl;s
        }
        else
        {
          newcontours.push_back(v);
          //cout << v << endl;

        }
        prev=v;
    }
   // cout<<"COUNTOUR COMPLETE"<<endl;

Mat newdst=dst;
approxPolyDP(newcontours, draw, arcLength(newcontours,1)*0.02, 1);
int minx=32767,miny=32767,maxx=0,maxy=0,it=0;
for (auto iterator : draw)
{
  Vec3b mycolor(0,0,100);
  cout<<iterator<<endl;
  //dst.at<Vec3b>(iterator.x,iterator.y)=mycolor;
  circle(newdst,iterator,20,CV_RGB(255,255,255),-1,8,0);
  if(iterator.x>maxx)
  maxx=iterator.x;
  if(iterator.y>maxy)
  maxy=iterator.y;
  if(iterator.x<minx)
  minx=iterator.x;
  if(iterator.y<miny)
  miny=iterator.y;
  inputQuad[it]=iterator;
  it++;
}
cout<<"Minx: "<<minx<<"Miny: "<<miny<<"Maxx: "<<maxx<<"Maxy: "<<maxy<<endl;
outputQuad[0]=Point(minx,miny);
outputQuad[1]=Point(minx,maxy);
outputQuad[3]=Point(maxx,miny);
outputQuad[2]=Point(maxx,maxy);
Rect myROI(minx,miny,maxx-minx,maxy-miny);

Mat pers;

  /*for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }*/
for (auto contourpt : largestContour){
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    circle(drawing, contourpt, 2, color, 1, 8, 0);
}
Mat lambda( 2, 4, CV_32FC1 );
lambda = Mat::zeros( src.rows, src.cols, src.type() );
lambda = getPerspectiveTransform(inputQuad, outputQuad);
warpPerspective(src,pers,lambda,src.size() );


Mat croppedImage = pers(myROI);
 //imshow("detected lines", cdst);
 //imshow("corners",newdst);
 //imshow("canny", dst);
imshow("contours",drawing);
 imshow("Original",src);
imshow("Cropped and Rotated",croppedImage);
 //imshow("contours",contours);


 waitKey();

 return 0;
}
