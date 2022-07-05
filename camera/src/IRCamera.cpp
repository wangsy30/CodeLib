#include "IRCamera.h"
#include "define.h"
using namespace cv;
using namespace std;

IRcamera::~IRcamera(){
#if defined(_USE_VIDEO_)
    video.release();
#else
    /*  7.stop capturing  */
    ret = ioctl(fd_video,VIDIOC_STREAMOFF,&type);
    if (ret < 0){
       perror("stop fail\n");
    }
    /*  8.release yin she  */
    for (int i = 0; i< 2; i++){
       munmap(mptr[i],length[i]);
    }
    /*  close the device  */
    close(fd_video);
    writer.release();
    std::cout << "camera shut down" << std::endl;
#endif
}


int IRcamera::initCamera(int size_buffer){
#if defined(_USE_VIDEO_)
    video.open(read_path);
#if defined(_RESULT_RECORD_)
    writer.open(write_path, CV_FOURCC('X', 'V', 'I', 'D'), 20, cv::Size(capture_width, capture_height));
#endif
#else
   fd_video = open("/dev/video0", O_RDWR);
    buf_count = size_buffer;
    capture_width = capture_width;
    capture_height = capture_height;

    y = cv::Mat::zeros(cv::Size(capture_width,capture_height),CV_16UC1);
    y1 = cv::Mat::zeros(cv::Size(capture_width/2,capture_height),CV_16UC1);
    y2 = cv::Mat::zeros(cv::Size(capture_width/2,capture_height),CV_16UC1);
    u = cv::Mat::zeros(cv::Size(capture_width,capture_height),CV_16UC1);
    v = cv::Mat::zeros(cv::Size(capture_width,capture_height),CV_16UC1);

    frame_tmp = cv::Mat::zeros(cv::Size(capture_width,capture_height),CV_16UC1);

#if defined(_CAMERA_RECORD_)
    writer.open(write_path, CV_FOURCC('X', 'V', 'I', 'D'), 20, cv::Size(capture_width, capture_height));
#endif
    v4l2Init();
#endif
}

void IRcamera::v4l2Init(){
    /*  1.open the device  */
    if (fd_video < 0 ){
      perror("open fail\n");
    }
    /*  2.get format  */
    struct v4l2_fmtdesc v4fmt;
    v4fmt.index = 0;// ditterent from the video
    v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd_video, VIDIOC_ENUM_FMT,&v4fmt);// ioctl(fd , command , struct related to the command)
    if (ret < 0 ){
       perror("get fail\n");
    }
    printf("index=%d\n",v4fmt.index);
    printf("flags=%d\n",v4fmt.flags);
    printf("description=%s\n",v4fmt.description);
    unsigned char *p=(unsigned char *)&v4fmt.pixelformat;
    printf("pixelformat=%c%c%c%c\n",p[0],p[1],p[2],p[3]);
    printf("reserved=%d\n",v4fmt.reserved[0]);

    /*  3.set capturing format  */
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vfmt.fmt.pix.width = capture_width;
    vfmt.fmt.pix.height = capture_height;
    vfmt.fmt.pix.pixelformat = format;//set capturing format
    ret = ioctl(fd_video,VIDIOC_S_FMT,&vfmt);
    if (ret < 0){
      perror("set format fail\n");
    }
    memset(&vfmt,0,sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd_video,VIDIOC_G_FMT,&vfmt);
    if (ret < 0){
      perror("get format fail\n");
    }
    if (vfmt.fmt.pix.width == capture_width && vfmt.fmt.pix.height == capture_height && vfmt.fmt.pix.pixelformat == format){
      printf("set succeed\n");
    }else{
      printf("set failed\n");
    }

    /*  4.apply nei he kong jian  */
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.count = 2;//apply 4 huan chong qv
    reqbuffer.memory = V4L2_MEMORY_MMAP;//set yin she fang shi
    ret = ioctl(fd_video,VIDIOC_REQBUFS,&reqbuffer);
    if (ret < 0){
      perror("apply fail\n");
    }

    struct v4l2_buffer mapbuffer;
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(int i=0;i<2;i++){
      mapbuffer.index = i;
      ret = ioctl(fd_video,VIDIOC_QUERYBUF,&mapbuffer);//从内核空间中查询一个空间做映射
      if (ret < 0){
          perror("query fail\n");
      }
      mptr[i] = (unsigned char *)mmap(NULL,mapbuffer.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_video,mapbuffer.m.offset);
      length[i] = mapbuffer.length;
      //通知使用完毕，放回去
      ret = ioctl(fd_video,VIDIOC_QBUF,&mapbuffer);//不要mapbuffer了，还回去
      if (ret < 0){
          perror("return fail\n");
      }
    }

    /*  6.start capturing  */
    //VIDIOC_DQBUF：告诉内核我要某一个数据，内核不可以修改
    //VIDIOC_QBUF：告诉内核我已经使用完毕
    //VIDIOC_STREAMON：开始采集，数据到队列中
    //VIDIOC_STREAMOFF：停止采集，不再向队列中写数据
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd_video,VIDIOC_STREAMON,&type);
    if (ret < 0){
      perror("start fail\n");
    }
}

void IRcamera::ImDecode(std::vector<uchar>& data_buffer){
    for(int i = 0;i < capture_height;i++){
        for(int j = 0;j < capture_width/2;j++){
            y1.at<ushort>(i,j) = (ushort)data_buffer.at(capture_width*2*i+4*j);
            u.at<ushort>(i,2*j) = (ushort)data_buffer.at(capture_width*2*i+4*j+1);
            y2.at<ushort>(i,j) = (ushort)data_buffer.at(capture_width*2*i+4*j+2);
            v.at<ushort>(i,2*j+1) = (ushort)data_buffer.at(capture_width*2*i+4*j+3);
            }
        }
    for(int i = 0;i < capture_height;i++){
        for(int j = 0;j < capture_width/2;j++){
            y.at<ushort>(i,j*2) = y1.at<ushort>(i,j);
            y.at<ushort>(i,j*2+1) = y2.at<ushort>(i,j);
        }
    }
    frame_tmp = 256*u + 256*v + y - gray_bias;
    frame_tmp.convertTo(frame_tmp,CV_8UC1,0.498);
    cv::cvtColor(frame_tmp,frame,COLOR_GRAY2BGR);
}

void IRcamera::BuffToFrame(){
    struct v4l2_buffer readbuffer;
    readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    readbuffer.index = 6;//无效，下一行代码会设置index的值
    ret = ioctl(fd_video,VIDIOC_DQBUF,&readbuffer);
    if (ret < 0){
        perror("extract fail\n");
    }
    std::vector<uchar> vec_data(mptr[readbuffer.index],mptr[readbuffer.index]+readbuffer.length);
    ImDecode(vec_data);
    //    frame = imdecode(vec_data,IMREAD_COLOR);
//    std::cout << vec_data.size() << std::endl;
//    if(frame.empty()) printf("empty!\n");
    //通知内核已经使用完毕
    ret = ioctl(fd_video,VIDIOC_QBUF,&readbuffer);
    if (ret < 0){
        perror("return fail\n");
    }
}

void IRcamera::GetFrame(cv::Mat& dst){
#if defined(_USE_VIDEO_)
    video >> frame;
#else
    BuffToFrame();
#if defined(_CAMERA_RECORD_)
    writer << frame;
#endif
#endif
    dst = frame.clone();
}
