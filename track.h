#ifndef TRACKING_H
#define TRACKING_H
#include <QObject>
#include "vehicledetector.h"
#include <vector>
#include <string>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <unistd.h>.
#include <set>
#include "opencv2/video/tracking.hpp"
#include "Hungarian.h"
#include "KalmanTracker.h"



using namespace std;
using namespace cv;


#define StateType Rect_<float>



typedef struct TrackBox
{
  int frame;
  int id;
  Rect_<float> box;
}TrackBox;




class Track
{
public:
  /**
   * @brief Track
   */
  Track();

  /**
   * @brief IOU
   * @param box1
   * @param box2
   * @return
   */
  float IOU(Rect_<float> box1, Rect_<float> box2);
  /**
   * @brief GetIOU
   * @param box1
   * @param box2
   * @return
   */
  float GetIOU(Rect_<float> box1, Rect_<float> box2);
  /**
   * @brief Trackbox
   * @param bboxes
   * @return
   */
  vector<TrackBox>  Trackbox(vector<cv::Rect> bboxes);


private:

  vector<TrackBox> detData;
    int max_age = 10;
  int min_hits = 1;
  unsigned int trkNum = 0;
  unsigned int detNum = 0;
  double iouThreshold = 0.2; //0.3
  vector<KalmanTracker> trackers;
  KalmanTracker kf_count ; //0
  vector<Rect_<float>> predictedBoxes;
  vector<vector<double>> iouMatrix;
  vector<int> assignment;
  set<int> unmatchedDetections;
  set<int> unmatchedTrajectories;
  set<int> allItems;
  set<int> matchedItems;
  vector<cv::Point> matchedPairs;
  vector<TrackBox> frameTrackResult;
  vector<vector<TrackBox>> detFrameData;
  Rect_<float> box1, box2;

};

#endif // TRACKING_H
