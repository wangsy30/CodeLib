#include "ProcessBox.h"
#include<opencv2/core/eigen.hpp>

using namespace cv;
using namespace std;
using namespace ly;
using namespace ml;

ProcessBox::ProcessBox() {
    SVM_params = cv::ml::SVM::load((string)getenv("HOME")+"/AutoAim_VREP/config/number_HOG.xml");
}
ProcessBox::~ProcessBox() {
    
}

void ProcessBox::feature(Mat image,Mat& data_mat,const int& yangben_data_position){
    Mat trainImg = Mat(Size(128, 128), CV_8UC1); 
	resize(image, trainImg, trainImg.size());
	HOGDescriptor *hog = new HOGDescriptor(Size(128, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	vector<float>descriptors;//存放结果    为HOG描述子向量    
	hog->compute(trainImg, descriptors, Size(1, 1), Size(0, 0)); //Hog特征计算，检测窗口移动步长(1,1)     
	// cout << "HOG描述子向量维数    : " << descriptors.size() << endl; 
	for (vector<float>::size_type j = 0; j < descriptors.size(); j++) {
		data_mat.at<float>(yangben_data_position, j) = descriptors[j];
	}
}

bool ProcessBox::FigureProcess(Mat& src,vector<Point2f>& corners,Mat& src_roi) {
        Mat src_temp,mask;
        Point2f pot[4];
        int length_Gauss = 5;
        GaussianBlur(src,src_temp,Size(2*length_Gauss+1,2*length_Gauss+1),0,0);
        cvtColor(src_temp,src_temp,CV_BGR2HSV);
        inRange(src_temp,Scalar(78,43,46),Scalar(124,255,255),mask);
        Mat kernel = getStructuringElement(MORPH_RECT,Size(2,2));
        morphologyEx(mask,mask,MORPH_OPEN,kernel);

        vector<vector<Point>> contours;
        vector<vector<Point2f>> contours_select;
        findContours(mask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
        // src_contours = src.clone();
        // drawContours(src_contours,contours,-1,Scalar(0,255,0));
        // imshow("src_contours",src_contours);
        
        if(contours.size() < 2)
            return false;

        // 筛选符合要求的轮廓，保存其四个角点和中心点
        for (size_t i = 0; i < contours.size(); i++) {
            // cout << contours.at(i).size() << endl;
            if(contours.at(i).size() < 100) {
                RotatedRect tmp = minAreaRect(contours[i]);
                if(tmp.size.area() > 1000 || tmp.size.area() < 100) 
                    continue;
                Point2f front_left,front_right,real_right,real_left;
                vector<Point2f> rectangles;
                tmp.points(pot);

                front_left = pot[0];
                front_right = pot[0];
                real_right = pot[0];
                real_left = pot[0];

                if(tmp.angle == 0 || tmp.angle == 90 || tmp.angle == 180 || tmp.angle == 270){
                    for(size_t j=0;j<4;j++) {
                        if(pot[j].y >= real_right.y && pot[j].x >= real_right.x) real_right = pot[j];
                        if(pot[j].y <= front_left.y && pot[j].x <= front_left.x) front_left = pot[j];
                        if(pot[j].y >= real_left.y && pot[j].x <= real_left.x) real_left = pot[j];
                        if(pot[j].y <= front_right.y && pot[j].x >= front_right.x) front_right = pot[j];
                    }  
                }else{
                    Point2f max_y,min_y,max_x,min_x;
                    max_y = pot[0];
                    min_y = pot[0];
                    max_x = pot[0];
                    min_x = pot[0];
                    for(size_t j=0;j<4;j++) {
                        if(pot[j].y > max_y.y) max_y = pot[j];
                        if(pot[j].x > max_x.x) max_x = pot[j];
                        if(pot[j].y < min_y.y) min_y = pot[j];
                        if(pot[j].x < min_x.x) min_x = pot[j];
                    }
                    if(max_x.y <= min_x.y) {
                        front_left = min_y;
                        front_right = max_x;
                        real_right = max_y;
                        real_left = min_x;   
                    }else{
                        front_left = min_x;
                        front_right = min_y;
                        real_right = max_x;
                        real_left = max_y;
                    }
                }
                rectangles.push_back(front_left);
                rectangles.push_back(front_right);
                rectangles.push_back(real_right);
                rectangles.push_back(real_left);
                rectangles.push_back(tmp.center);
                contours_select.push_back(rectangles);
            }
        }
        if(contours_select.size() <= 1)
            return false;
        for (size_t i = 0; i < contours_select.size()-1; i++) {
            
            for (size_t j = i+1; j < contours_select.size(); j++){
                // cout << contours_select.at(i).at(4).y << "-" << contours_select.at(j).at(4).y << endl;
                // cout << contours_select.at(i).at(4).x << "-" << contours_select.at(j).at(4).x << endl;                
                if(    abs(contours_select.at(i).at(4).y-contours_select.at(j).at(4).y) < 20 
                    && abs(contours_select.at(i).at(4).x-contours_select.at(j).at(4).x) < 200){
                    vector<Point2f> Left_,Right_;
                    Left_ = (contours_select.at(i).at(4).x < contours_select.at(j).at(4).x) ? contours_select.at(i):contours_select.at(j);
                    Right_ = (contours_select.at(i).at(4).x < contours_select.at(j).at(4).x) ? contours_select.at(j):contours_select.at(i);
                    Point2f Left_up((Left_.at(0).x + Left_.at(1).x)/2,(Left_.at(0).y + Left_.at(1).y)/2),
                            Left_down((Left_.at(2).x + Left_.at(3).x)/2,(Left_.at(2).y + Left_.at(3).y)/2),
                            Right_up((Right_.at(0).x + Right_.at(1).x)/2,(Right_.at(0).y + Right_.at(1).y)/2),
                            Right_down((Right_.at(2).x + Right_.at(3).x)/2,(Right_.at(2).y + Right_.at(3).y)/2);
                    if(Left_up.x == 1 || Left_up.y == 1
                    || Left_down.x == 1 || Left_down.y == src.rows
                    || Right_up.x == src.cols || Right_up.y == 1
                    || Right_down.x == src.cols || Right_down.y == src.rows)
                        continue;
                    float Left_k = (Left_up.x - Left_down.x) / (Left_up.y - Left_down.y);
                    float Right_k = (Right_up.x - Right_down.x) / (Right_up.y - Right_down.y);
                    float width = sqrt(pow(Left_up.x - Left_down.x,2) + pow(Left_up.y - Left_down.y,2));
                    float length = sqrt(pow(Left_up.x - Right_up.x,2) + pow(Left_up.y - Right_up.y,2));

                    if(abs(Left_k - Right_k) > 0.2 
                    || (length/width > 3 || length/width < 1))
                        continue;
                    corners.push_back(Left_up);
                    corners.push_back(Right_up);
                    corners.push_back(Right_down);
                    corners.push_back(Left_down);

                    //detect num                   
                    float center_x = (Left_.at(4).x + Right_.at(4).x)/2;
                    float center_y = (Left_.at(4).y + Right_.at(4).y)/2;
                    int Rect_x = ((int)(center_x - 0.3*length) > 1) ? (int)(center_x - 0.3*length) : 1;
                    int Rect_y = ((int)(center_y - 0.7*width) > 1) ? (int)(center_y - 0.7*width) : 1;
                    int Rect_width = ((Rect_x + (int)(0.6*length)) < src.cols) ? (int)(0.6*length) : (src.cols-Rect_x);
                    int Rect_length = ((Rect_y + (int)(1.4*width)) < src.rows) ? (int)(1.4*width) : (src.rows-Rect_y);
                    src_roi = src(Rect(Rect_x,Rect_y,Rect_width,Rect_length)).clone();
                    // imshow("roi",src_roi);
                    // char s[10];
                    // sprintf(s,"%d",cnt);
                    // imwrite("./num/"+(string)s+".jpg",src_roi);
                    // cnt++;
                    // cout << cnt << endl;
                    for (char k = 0; k < 4; k++){
                        // /* code */
                        char num[2];
                        sprintf(num,"%d",k+1);
                        circle(src,(Point)corners.at(k),2,Scalar(0,255,0));
                        putText(src,num,(Point)corners.at(k),FONT_HERSHEY_PLAIN,1,Scalar(0,0,255),1,8);
                    }
                    return true;
                }               
            }
        }
        return false;
        // for (char i = 0; i < 4; i++){
        //     // /* code */
        //     char num[2];
        //     sprintf(num,"%d",i+1);
        //     circle(src,(Point)corners.at(i),2,Scalar(0,255,0));
        //     putText(src,num,(Point)corners.at(i),FONT_HERSHEY_PLAIN,1,Scalar(0,0,255),1,8);
        // }
        
        // src_contours_select = src.clone();
        // drawContours(src_contours_select,contours_select,-1,Scalar(0,255,0));
        // imshow("contours_select",src);
}

int ProcessBox::DetectNumber(cv::Mat& src_roi){
    cvtColor(src_roi, src_roi, COLOR_BGR2GRAY);
    threshold(src_roi, src_roi, 0, 255, CV_THRESH_OTSU);
    Mat input;
    Mat SVMtrainMat = Mat(1, 8100, CV_32FC1);
    feature(src_roi,SVMtrainMat,0);

    SVMtrainMat.convertTo(SVMtrainMat, CV_32FC1);//更改图片数据的类型，必要，不然会出错
    return (int)SVM_params->predict(SVMtrainMat);//检测结果
}


void ProcessBox::CalTargetPose(const std::vector<cv::Point2f>&corners,const int& yaw,const int& pitch){
    cv::solvePnP(para::corners_at_box,corners,para::intrMatrix,para::distCoeffs,rvec,tvec,false,SOLVEPNP_IPPE); 
    cv::cv2eigen(tvec,Tvec_cam);

    /*--check whether pnp solves right--*/
    // cv::cv2eigen(para::intrMatrix,intrMatrix_eigen);
    // Eigen::Vector3d uv1;
    // uv1 = intrMatrix_eigen * Tvec_cam / Tvec_cam(2);
    // cv::circle(frame,Point((int)uv1(0),(int)uv1(1)),1,Scalar(0,0,255),2);

    theta_x = pitch*M_PI/180;
    theta_y = yaw*M_PI/180;
    R_y <<  cos(theta_y),0,sin(theta_y),
                0,      1,      0     ,
            -sin(theta_y),0,cos(theta_y);
    R_x <<  1      ,0     ,      0     ,
            0,cos(theta_x),-sin(theta_x),
            0,sin(theta_x), cos(theta_x);               
    RotMatrix =  R_y * R_x;
    Tvec_base = RotMatrix * (Tvec_cam + para::t);
}

void ProcessBox::GetOrder(const std::vector<cv::Point2f>&corners,Mat& frame,const int& yaw,const int& pitch,ly::sendData& send_data){
    CalTargetPose(corners,yaw,pitch);
    putText(frame,format("camera frame :  X : %f  Y : %f  Z : %f",Tvec_cam(0),Tvec_cam(1),Tvec_cam(2)),cv::Point(20, 40), cv::FONT_HERSHEY_PLAIN, 1.25, cv::Scalar(23, 243, 2), 2);
    putText(frame,format("base frame :  X : %f  Y : %f  Z : %f",Tvec_base(0),Tvec_base(1),Tvec_base(2)),cv::Point(20, 60), cv::FONT_HERSHEY_PLAIN, 1.25, cv::Scalar(23, 243, 2), 2);
#if defined(_USE_KALMAN_)
    KalmanPredict();
    send_data.yaw = -(pose_correct.at<float>(0) + 10*dt * pose_correct.at<float>(2))*180/M_PI;
    send_data.pitch = -(pose_correct.at<float>(1) + 10*dt * pose_correct.at<float>(3))*180/M_PI;
    cout << "[optimized]: yaw  " << -send_data.yaw << "   pitch  " << -send_data.pitch << endl;
#else
    float _yaw = atan2(Tvec_base(0),Tvec_base(2));
    float _pitch = atan2(-Tvec_base(1),sqrt(pow(Tvec_base(0),2)+pow(Tvec_base(2),2)));;
    send_data.yaw = -_yaw*180/M_PI;
    send_data.pitch = -_pitch*180/M_PI;   
#endif
}

void ProcessBox::KalmanPredict(){
    //get current pose of center
    yaw_m[1] = atan2(Tvec_base(0),Tvec_base(2));
    pitch_m[1] = atan2(-Tvec_base(1),sqrt(pow(Tvec_base(0),2)+pow(Tvec_base(2),2)));
    if(first_time == 1){
        // cout << "kalman init start" << endl;
        first_time = 0;
        v_yaw_m = 0;
        v_pitch_m = 0;
        //initialize the filter
        const int state_num = 4;//x 4x1
        const int measure_num = 4;//z 4x1
        kalman.init(state_num,measure_num,0);
        measurement = Mat::zeros(4, 1, CV_32F);
        kalman.transitionMatrix = (Mat_<float>(4, 4) << 1 ,0 ,dt,0 ,
                                                        0 ,1 ,0 ,dt,
                                                        0 ,0 ,1 ,0 ,
                                                        0 ,0 ,0 ,1 );
        setIdentity(kalman.measurementMatrix);//set H
        setIdentity(kalman.measurementNoiseCov,cv::Scalar::all(para::R));//set R
        setIdentity(kalman.processNoiseCov,cv::Scalar::all(para::Q));//set Q

        //state initialize x0 P0
        setIdentity(kalman.errorCovPost);//set P0
        kalman.statePost.at<float>(0) = yaw_m[1];
        kalman.statePost.at<float>(1) = pitch_m[1];
        kalman.statePost.at<float>(2) = 0;
        kalman.statePost.at<float>(3) = 0;
        cout << "kalman init done" << endl;
    }else{
        v_yaw_m = (yaw_m[1] - yaw_m[0])/dt;
        v_pitch_m = (pitch_m[1] - pitch_m[0])/dt;
        // cout << v_yaw_m << " " << v_pitch_m << endl;
    }
    yaw_m[0] = yaw_m[1];
    pitch_m[0] = pitch_m[1];
    
    //update measurement z
    
    measurement.at<float>(0) = yaw_m[1];
    measurement.at<float>(1) = pitch_m[1];
    measurement.at<float>(2) = v_yaw_m;
    measurement.at<float>(3) = v_pitch_m;
    kalman.predict();//predict
    pose_correct = kalman.correct(measurement);//update
}