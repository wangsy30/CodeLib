#include "Converter.h"
#include <math.h>
ImageConverter::ImageConverter()
  : it_(nh_)
{
  // Subscrive to input video feed and publish output video feed
  image_sub_ = it_.subscribe("IRImage_raw", 10,
    &ImageConverter::imageCb, this);
   
  pub=nh_.advertise<std_msgs::String>("warning",10);
  sub=nh_.subscribe("scan",10,&ImageConverter::LaserScancallback,this);
#if defined(_RESULT_RECORD_)
  writer.open(para::VIDEO_WRITE_PATH,CV_FOURCC('X', 'V', 'I', 'D'), 20, cv::Size(para::FRAME_WIDTH,para::FRAME_HEIGHT));
#endif
  cv::namedWindow(OPENCV_WINDOW);
}

ImageConverter::~ImageConverter()
{
  writer.release();
  cv::destroyWindow(OPENCV_WINDOW);
}


void ImageConverter::LaserScancallback(const sensor_msgs::LaserScan& scan)
{
    increment = scan.angle_increment;
//    angle_ranges = scan.ranges;
    std::vector<float> angle_ranges_tmp;
    std::vector<float> angle_right;
    int num = (int) (0.25*opt_pi/increment);
    int length = scan.ranges.size();
    float tmp1,tmp2;
    for(int i = 0;i < num;i++){
        tmp1 = scan.ranges.at(i);
        tmp2 = scan.ranges.at(length-num+i);
        if(!isnan(tmp1)){
            angle_right.emplace_back(tmp1);
        }else{
            angle_right.emplace_back(13.0);
        }
        if(!isnan(tmp2)){
            angle_ranges_tmp.emplace_back(tmp2);
        }else{
            angle_ranges_tmp.emplace_back(13.0);
        }
    }
    angle_ranges_tmp.insert(angle_ranges_tmp.end(),angle_right.begin(),angle_right.end());
//    float tmp = angle_ranges.at(angle_ranges.size()-1);
//    ROS_INFO("num:%d, range size: %d, increment:%f",num,angle_ranges.size(),tmp);
    angle_ranges.swap(angle_ranges_tmp);
//    ROS_INFO("%d",angle_ranges.size());
}

void ImageConverter::imageCb(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  try
  {
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    src = cv_ptr->image;

  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }


  //our part
  if(!angle_ranges.empty())
      flag = pb.Process(src,angle_ranges,increment);

  //send message
  switch(flag){
  case box::BOTH_MODE:
      send_msg.data = 1;
      pub.publish(send_msg);
      break;
  case box::FEVER_MODE:
      send_msg.data = 2;
      pub.publish(send_msg);
      break;
  case box::MASKLESS_MODE:
      send_msg.data = 3;
      pub.publish(send_msg);
      break;
  case box::NONE_MODE:
      send_msg.data = 4;
      pub.publish(send_msg);
      break;
  case -1:
      ROS_INFO("no face detected");
      break;
  case -2:
      ROS_INFO("process error");
      break;
  }
  // Update GUI Window

//  cv::line(src,cv::Point(src.cols/2,0),cv::Point(src.cols/2,src.rows-1),cv::Scalar(0,255,0),1);
//  if(cv::waitKey(3)=='s'){
//      sprintf(picture_save_path,"/home/w/mycode/opt22_code/ros_opencv/src/data_process/configs/%f.jpg",target_angle);
//      cv::imwrite(picture_save_path,src);
//  }
  cv::imshow(OPENCV_WINDOW, src);
#if defined(_RESULT_RECORD_)
  writer << src;
#endif
  if(cv::waitKey(3)=='l')
      writer.release();
  // Output modified video stream
  // image_pub_.publish(cv_ptr->toImageMsg());
}
