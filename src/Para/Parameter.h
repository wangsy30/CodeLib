#ifndef __PATAMETER_H
#define __PATAMETER_H

#pragma once
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>


namespace para {
  extern std::string CAMERA_PATH;
  extern std::string VIDEO_READ_PATH;
  extern std::string VIDEO_WRITE_PATH;
  extern std::string ANGLE_DATA_PATH;

  extern int  CAMERA_COUNT;
  extern int  CAMERA_INDEX;

  extern int  FRAME_HEIGHT;
  extern int  FRAME_WIDTH;

  extern int  FRAME_RESIZE_WIDTH;
  extern int  FRAME_RESIZE_HEIGHT;

  extern std::string UART_PATH;
  extern int  UART_BAUD;

  extern ushort GRAY_BIAS;

  extern int  MASK_RESIZE;
  extern int  MASK_WIDTH;
  extern int  MASK_LENGTH;
  extern int  MASK_GAUSS_KERNEL;
  extern double  MASK_THRESH;
  extern uchar  MASK_WEIGHT;
  extern uchar  HEAD_WEIGHT;
  extern uchar  SIDE_WEIGHT;

  extern int MID_SCORE;
  extern int MID_FEVER;

  extern int TEMP_PIX_LOW;
  extern int TEMP_PIX_HIGH;
  extern int TEMP_LOW;
  extern int TEMP_HIGH;

  extern float TEMP_BIAS;

  extern int COEFFICIENT;

  extern int COL_BEGIN;
  extern int COL_END;

  extern std::string NET_CONFIG_PATH;
  extern std::string NET_WEIGHTS_PATH;

  extern std::vector<float> angle_data;


  class Parameter {
  private:
    std::string m_file_path;
    cv::FileStorage m_file_storage;
    std::ifstream angle_data_in;
  public:
    Parameter();
    Parameter(const std::string parameter_file_path);
    ~Parameter();

    bool open(const std::string parameter_file_path);

    void load();
  };

}
#endif // __PARAMETER_H
