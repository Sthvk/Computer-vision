#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Images  //////////////////////

Mat imgOriginal, imgResized, imgGray, imgCanny, imgThreshold, imgBlur, imgDilation, imgErode, imgWarp;
vector<Point> initialDocPoints, docPoints;


float w = 420, h = 596;

Mat preprocessing(Mat img)
{
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDilation, kernel);
	//erode(imgDilation, imgErode, kernel);

	return imgDilation;
}

vector<Point> getContours(Mat imgDil) 
{
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conersPolygon(contours.size());
    vector<Rect> boundRect(contours.size());

	vector<Point> biggestContour;
	int maxArea = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);

        if (area > 1000) {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conersPolygon[i], 0.02 * peri, true);

			if (area > maxArea)
			{
				biggestContour = { conersPolygon[i][0], conersPolygon[i][1], conersPolygon[i][2], conersPolygon[i][3] };
				maxArea = area;
			}

            drawContours(imgOriginal, conersPolygon, i, Scalar(255, 0, 255), 2);
        }
    }
    return biggestContour;
}

void drawPoints(vector<Point> points, Scalar color)
{

	for (int i = 0; i < points.size(); i++) 
	{
		circle(imgResized, points[i], 10, color, FILLED);
		putText(imgResized, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}

vector<Point> reorder(vector<Point> points)
{
	vector<Point> docPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}

	docPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //0
	docPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //1
	docPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //2
	docPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //3
	return docPoints;
}

Mat getWarp(Mat img, vector<Point> points, float w, float h)
{
	Point2f src[4] = { points[0], points[1], points[2],points[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));

	return imgWarp;
}

void main() 
{
	string path = "Resources/paper.jpg";
	imgOriginal = imread(path);
	resize(imgOriginal, imgResized, Size(), 0.5, 0.5);

	//Preprocess image
	imgThreshold = preprocessing(imgResized);

	//Get contours - biggest
	initialDocPoints = getContours(imgThreshold);

	//detect corners
	//drawPoints(initialDocPoints, { 0,0,255 });
	docPoints = reorder(initialDocPoints);
	//drawPoints(docPoints, { 0,255,0 });

	//Warp
	imgWarp = getWarp(imgResized, docPoints, w, h);

	imshow("Image", imgResized);
	//imshow("Image Dilation", imgThreshold);
	imshow("Image Warp", imgWarp);
	waitKey(0);

}