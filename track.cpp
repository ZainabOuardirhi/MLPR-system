#include "track.h"

Track::Track()
{




}


float Track::IOU(Rect_<float> box1, Rect_<float> box2)
{
  float in = (box2 & box1).area();
  float un = box2.area() + box1.area() - in;

  if (un < DBL_EPSILON)
    return 0;

  return (float)(in / un);
}


float Track::GetIOU(Rect_<float> box1, Rect_<float> box2)
{
  float minx1 = box1.x;
  float maxx1 = box1.x + box1.width;
  float miny1 = box1.y;
  float maxy1 = box1.y+ box1.height;

  float minx2 = box2.x;
  float maxx2 = box2.x + box2.width;
  float miny2 = box2.y;
  float maxy2 = box2.y + box2.height;

  if (minx1 > maxx2 || maxx1 < minx2 || miny1 > maxy2 || maxy1 < miny2)
    return 0.0f;
  else
    {
      float dx = std::min(maxx2, maxx1) - std::max(minx2, minx1);
      float dy = std::min(maxy2, maxy1) - std::max(miny2, miny1);
      float area1 = (maxx1 - minx1)*(maxy1 - miny1);
      float area2 = (maxx2 - minx2)*(maxy2 - miny2);
      float inter = dx*dy; // Intersection
      float uni = area1 + area2 - inter; // Union
      float IoU = inter / uni;
      return IoU;
    }
  //	return 0.0f;
}

vector<TrackBox> Track::Trackbox(vector<cv::Rect> bboxes)
{
  detData.clear();
  detFrameData.clear();
  frameTrackResult.clear();
  predictedBoxes.clear();

  for(int i=0;i<bboxes.size();i++)
    {
      TrackBox tb;

      tb.box = Rect_<float>(Point_<float>(bboxes.at(i).x, bboxes.at(i).y), Point_<float>(bboxes.at(i).x + bboxes.at(i).width, bboxes.at(i).y + bboxes.at(i).height));
      detData.push_back(tb);
      detFrameData.push_back(detData);
    }

  if (trackers.size() == 0) // the first frame met
    {
      // initialize kalman trackers using first detections.
      for (unsigned int i = 0; i < detData.size(); i++)
        {
          KalmanTracker trk = KalmanTracker(detData.at(i).box);
          trackers.push_back(trk);
        }

      //continue;
    }



  for (auto it = trackers.begin(); it != trackers.end();)
    {
      Rect_<float> pBox = (*it).predict();
      if (pBox.x >= 0 && pBox.y >= 0)
        {
          predictedBoxes.push_back(pBox);
          it++;
        }
      else
        {
          it = trackers.erase(it);

        }
    }


  trkNum = predictedBoxes.size();
  detNum =detData.size();

  iouMatrix.clear();
  iouMatrix.resize(trkNum, vector<double>(detNum, 0));

  for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
    {
      for (unsigned int j = 0; j < detNum; j++)
        {
          iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detData.at(j).box);

        }
    }

  HungarianAlgorithm HungAlgo;
  assignment.clear();
  HungAlgo.Solve(iouMatrix, assignment);
  unmatchedTrajectories.clear();
  unmatchedDetections.clear();
  allItems.clear();
  matchedItems.clear();

  if (detNum > trkNum)
    {
      for (unsigned int n = 0; n < detNum; n++)
        allItems.insert(n);

      for (unsigned int i = 0; i < trkNum; ++i)
        matchedItems.insert(assignment[i]);

      set_difference(allItems.begin(), allItems.end(),
                     matchedItems.begin(), matchedItems.end(),
                     insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
    }
  else if (detNum < trkNum)
    {
      for (unsigned int i = 0; i < trkNum; ++i)
        if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
          unmatchedTrajectories.insert(i);
    }
  else
    ;

  // filter out matched with low IOU
  matchedPairs.clear();
  for (unsigned int i = 0; i < trkNum; ++i)
    {
      if (assignment[i] == -1) // pass over invalid values
        continue;
      if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
        {
          unmatchedTrajectories.insert(i);
          unmatchedDetections.insert(assignment[i]);
        }
      else
        matchedPairs.push_back(cv::Point(i, assignment[i]));
    }

  int detIdx, trkIdx;
  for (unsigned int i = 0; i < matchedPairs.size(); i++)
    {
      trkIdx = matchedPairs[i].x;
      detIdx = matchedPairs[i].y;
      trackers[trkIdx].update(detData.at(detIdx).box);
    }

  for (auto umd : unmatchedDetections)
    {
      KalmanTracker tracker = KalmanTracker(detData.at(umd).box);
      trackers.push_back(tracker);
    }

  // get trackers' output
  frameTrackResult.clear();
  for (auto it = trackers.begin(); it != trackers.end();)
    {
      if (((*it).m_time_since_update < 1) &&
          ((*it).m_hit_streak >= min_hits ))
        {
          TrackBox res;
          res.box = (*it).get_state();
          res.id = (*it).m_id + 1;
          frameTrackResult.push_back(res);
          it++;
        }
      else
        it++;

      // remove dead tracklet
      if (it != trackers.end() && (*it).m_time_since_update > max_age)
        it = trackers.erase(it);
    }

  return frameTrackResult;

}
