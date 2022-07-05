#include "ProcessBox.h"
#include "../Para/Parameter.h"
#include "../define.h"
using namespace cv;
using namespace std;
namespace box {

processbox::processbox(){
    mask = cv::Mat::ones(para::MASK_RESIZE,para::MASK_RESIZE,CV_8UC1);
    mask = mask * para::HEAD_WEIGHT;
    for(int row = para::MASK_RESIZE/2;row < para::MASK_RESIZE;row++){
        for(int col = 0;col < para::MASK_RESIZE;col++){
            if(row < para::MASK_RESIZE/2+para::MASK_LENGTH &&
                    col >= (para::MASK_RESIZE-para::MASK_WIDTH)/2 &&
                    col < (para::MASK_RESIZE+para::MASK_WIDTH)/2){
                mask.at<uchar>(row,col) = para::MASK_WEIGHT;
            }else{
                mask.at<uchar>(row,col) = para::SIDE_WEIGHT;
            }
        }
    }
#if defined(_FACE_DETECT_)
    config.net_type = YOLOV3;
    config.file_model_cfg = para::NET_CONFIG_PATH;
    config.file_model_weights = para::NET_WEIGHTS_PATH;
//    config.calibration_image_list_file_txt = "../configs/calibration_images2.txt";
    config.inference_precison =FP16;
    config.detect_thresh = 0.2;
    detector.init(config);
#endif
}


int processbox::JudgeFace(cv::Mat& src,float face_range,std::vector<int> face_location){
    cv::cvtColor(src,src_gray,COLOR_BGR2GRAY);

    Rect uphalf = Rect(0,0,src_gray.cols,(int)(src_gray.rows/2)+10);
    src_uphalf = src_gray(uphalf).clone();
    cv::threshold(src_uphalf,src_uphalf,para::MASK_THRESH,255,THRESH_BINARY);

    Mat element = getStructuringElement(MORPH_ELLIPSE,Size(20,10));
    cv::morphologyEx(src_uphalf,src_uphalf,MORPH_CLOSE,element);
//    cv::dilate(src_uphalf,src_uphalf,)
//    imshow("headHALF",src_uphalf);
    vector<vector<Point>> contours;
    vector<vector<Point>> contours_select;
    cv::findContours(src_uphalf,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    int index = 0;
    for(int i = 0;i < contours.size();i++){
//        cout << contours[i].size() << " ";
        if(contours[index].size() < contours[i].size())
            index = i;
    }
    contours_select.push_back(contours[index]);

//    cout << contours_select.size() << endl;
    if(contours_select.empty())
        return JUDGE_FAILED;
    halfhead = boundingRect(contours_select[0]);
    int height = (int)(1.2*halfhead.width);
    if(height + halfhead.y >= src_gray.rows){
        height = src_gray.rows - halfhead.y-1;
    }
    finalhead = Rect(halfhead.x,halfhead.y,halfhead.width,height);
    head = src_gray(finalhead).clone();

//    cout << contours_select.size() << endl;
//    cv::drawContours(src_uphalf,contours_select,0,Scalar(255),1);
//    imshow("src",src);
    //1.下采样
    cv::resize(head,head,Size(para::MASK_RESIZE,para::MASK_RESIZE));
    //2.模糊化
//    cv::GaussianBlur(src,src,Size(para::MASK_GAUSS_KERNEL,para::MASK_GAUSS_KERNEL),0,0);
    //3.阈值分割
    cv::threshold(head,head,para::MASK_THRESH,1,THRESH_BINARY);
    //4.计算得分
    cv::multiply(head,mask,dst_mask);
//    imshow("dst",dst_mask);
    int a = cv::sum(dst_mask)[0];
//    cout << "[SCORE]:" << a;
    if(a < para::MID_SCORE){
        cv::rectangle(src,finalhead,Scalar(0,255,0),2);
    }else{
        cv::rectangle(src,finalhead,Scalar(0,0,255),2);
    }

    /*---detect fever---*/
    //获取距离信息
//    int pix_center = src.;
//    float range = Pixel2Range(pix_center,angle_ranges,increment);
//    cout << "range:" << face_range << endl;
    //计算灰度值
    halfhead_mat = src_gray(halfhead);
    cv::minMaxLoc(halfhead_mat,&minval,&fever_val);
    fever_val = (float)((fever_val - para::TEMP_PIX_LOW)*(para::TEMP_HIGH-para::TEMP_LOW))/(para::TEMP_PIX_HIGH-para::TEMP_PIX_LOW)+para::TEMP_LOW;
    fever_val = fever_val + para::TEMP_BIAS;

    //数据可视化
    face_location[0] = 0;
    face_location[1] = 120;
    int bias = 10;
    cv::putText(_src,cv::format("DISTANCE:%.3f",face_range),Point(face_location[0]+bias,face_location[1]-60),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,255,0));
    float fever_val_raw = fever_val-(float)para::COEFFICIENT*(1-(float)1/face_range);
    if(a > para::MID_SCORE && fever_val > para::MID_FEVER){
        cv::putText(_src,cv::format("TEMPERATURE(MODIFIED):%.1f",fever_val),Point(face_location[0]+bias,face_location[1]),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,0,255));
        cv::putText(_src,cv::format("TEMPERATURE(RAW):%.1f",fever_val_raw),Point(face_location[0]+bias,face_location[1]-30),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,0,255));
        return FEVER_AND_MASKLESS;
    }else if(a > para::MID_SCORE && fever_val < para::MID_FEVER){
        cv::putText(_src,cv::format("TEMPERATURE(MODIFIED):%.1f",fever_val),Point(face_location[0]+bias,face_location[1]),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,255,0));
        cv::putText(_src,cv::format("TEMPERATURE(RAW):%.1f",fever_val_raw),Point(face_location[0]+bias,face_location[1]-30),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,255,0));
        return MASKLESS_ONLY;
    }else if(a < para::MID_SCORE && fever_val > para::MID_FEVER){
        cv::putText(_src,cv::format("TEMPERATURE(MODIFIED):%.1f",fever_val),Point(face_location[0]+bias,face_location[1]),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,0,255));
        cv::putText(_src,cv::format("TEMPERATURE(RAW):%.1f",fever_val_raw),Point(face_location[0]+bias,face_location[1]-30),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,0,255));
        return FEVER_ONLY;
    }else if(a < para::MID_SCORE && fever_val < para::MID_FEVER){
        cv::putText(_src,cv::format("TEMPERATURE(MODIFIED):%.1f",fever_val),Point(face_location[0]+bias,face_location[1]),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,255,0));
        cv::putText(_src,cv::format("TEMPERATURE(RAW):%.1f",fever_val_raw),Point(face_location[0]+bias,face_location[1]-30),cv::FONT_HERSHEY_PLAIN,2,Scalar(0,255,0));
        return NEITHER;
    }
}


int processbox::DetectFace(std::vector<float>& angle_ranges){
    std::vector<cv::Mat> batch_img;
    std::vector<int> location;

    face_roi_list.clear();
    face_ranges.clear();
    face_locations.clear();
    std::vector<BatchResult> batch_res;
    src_bgr = _src;

    batch_img.push_back(src_bgr);
    detector.detect(batch_img,batch_res);
//    cout << batch_res.size() << endl;
    if(batch_res[0].empty())
        return -1;
    for (const auto &r : batch_res[0]){
        face_roi_list.push_back(_src(r.rect));
        location.emplace_back(r.rect.x + r.rect.width/2);
        location.emplace_back(r.rect.y + r.rect.height/2);
        face_locations.emplace_back(location);
        face_ranges.emplace_back(Pixel2Range(location[0],angle_ranges));
//        cv::putText(src,cv::format("distance:%f",distance),Point(x_center+50,y_center-40),cv::FONT_HERSHEY_PLAIN,1,Scalar(0,255,0));
//        std::cout <<"batch "<<0<< " id:" << r.id << " prob:" << r.prob << " rect:" << r.rect << std::endl;
//        cv::rectangle(src, r.rect, cv::Scalar(255, 0, 0), 2);
//        std::stringstream stream;
//        stream << std::fixed << std::setprecision(2) << "id:" << r.id << "  score:" << r.prob;
//        cv::putText(batch_img[0], stream.str(), cv::Point(r.rect.x, r.rect.y - 5), 0, 0.5, cv::Scalar(0, 0, 255), 2);
    }
//    for(int i = 0;i<face_roi_list.size();i++){
//        cv::imshow("image"+std::to_string(i),face_roi_list[i] );
//    }
//    cv::namedWindow("image" + std::to_string(0), cv::WINDOW_NWORMAL);
//    cv::imshow("image"+std::to_string(0), batch_img[0]);
    return 1;
}

int processbox::Process(cv::Mat& src,std::vector<float>& angle_ranges,float increment){
    _src = src;
    _increment = increment;
    /*--angle visualize--*/
//    int angle_length = angle_ranges.size()-1;
////    float dy = ((float)(para::COL_END-para::COL_BEGIN) / angle_length);
////    for(int i = 0;i < angle_length-1 ;i++){
////        cv::circle(src,cv::Point2f(para::COL_BEGIN+i*dy,angle_ranges.at(i)*40),1,cv::Scalar(0,255,0),2);
////    }
////    printf("dy:%f   increment:%f   length:%d\n",dy,increment,angle_length);
//    std::vector<float>::iterator min_range = std::min_element(std::begin(angle_ranges), std::end(angle_ranges));
//    float angle = Index2Angle(std::distance(std::begin(angle_ranges),min_range),angle_length);
////    cout << "range_min_index: " << std::distance(std::begin(angle_ranges),min_range) << " "
////         << "range_min: " << angle_ranges[std::distance(std::begin(angle_ranges),min_range)] << " "
////         << "length: " << angle_length <<endl;
//    cout << "angle : " << angle << endl;
//    *target_angle = angle;

    /*--获取温度信息--*/
    /*--开始检测--*/
    judgeflag.clear();
    fever_num = 0;
    maskless_num = 0;
#if defined(_FACE_DETECT_)
    //detect face in the raw figure
    if(DetectFace(angle_ranges) == -1)
        return -1;
    for(int i = 0;i<face_roi_list.size();i++){
        if(face_roi_list[i].rows > 20 && face_roi_list[i].cols > 20){
            //for each face roi,detect the fever and mask-wearing situation
            judge_tmp = JudgeFace(face_roi_list[i],face_ranges[i],face_locations[i]);
            switch (judge_tmp)
            {
            case FEVER_AND_MASKLESS:
                fever_num++;
                maskless_num++;
                break;
            case MASKLESS_ONLY:
                maskless_num++;
                break;
            case FEVER_ONLY:
                fever_num++;
                break;   
            case JUDGE_FAILED:
                return -2;
            default:
                break;
            }
            judgeflag.push_back(judge_tmp);
        }
    }
//    cout << "fever num: " << fever_num << " | "
//         << "maskless num: " << maskless_num << endl;
    if(fever_num != 0 || maskless_num != 0){
        if(maskless_num == 0){
//            cout << "fever detected" << endl;
            return FEVER_MODE;
        }else if(fever_num == 0){
//            cout << "maskless detected" << endl;
            return MASKLESS_MODE;
        }else{
//            cout << "both detected" << endl;
            return BOTH_MODE;
        }
    }
#endif
    return NONE_MODE;
}

float processbox::Index2Angle(int index,int length){
    int zero_index = length/2;
    int dindex = zero_index - index;
    return _increment*dindex;
}

float processbox::Pixel2Range(int pixel,std::vector<float>& angle_ranges){
    int pixel_bias = pixel - para::FRAME_WIDTH/2;
    if(pixel_bias < para::COL_BEGIN){
        pixel_bias = para::COL_BEGIN;
    }else if(pixel_bias > para::COL_END){
        pixel_bias = para::COL_END;
    }
    float center_angle = para::angle_data[pixel_bias - para::COL_BEGIN];

    int index = angle_ranges.size()/2 - (int)(center_angle/_increment);

    float range = angle_ranges[index-2];
    for(int i = -20;i < 21;i++){
        if(angle_ranges[index+i] < range)
            range = angle_ranges[index+i];
    }
    return range;

}
}

