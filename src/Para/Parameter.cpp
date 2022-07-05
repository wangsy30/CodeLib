#include "Parameter.h"


namespace para {
  std::string CAMERA_PATH;
  std::string VIDEO_READ_PATH;
  std::string VIDEO_WRITE_PATH;
  std::string ANGLE_DATA_PATH;

  int  CAMERA_COUNT;
  int  CAMERA_INDEX;

  int  FRAME_HEIGHT;
  int  FRAME_WIDTH;

  int  FRAME_RESIZE_WIDTH;
  int  FRAME_RESIZE_HEIGHT;


  std::string UART_PATH;
  int  UART_BAUD;

  ushort GRAY_BIAS;

  int  MASK_RESIZE;
  int  MASK_WIDTH;
  int  MASK_LENGTH;
  int  MASK_GAUSS_KERNEL;
  double  MASK_THRESH;

  uchar  MASK_WEIGHT;
  uchar  HEAD_WEIGHT;
  uchar  SIDE_WEIGHT;

  int MID_SCORE;
  int MID_FEVER;
  int TEMP_PIX_LOW;
  int TEMP_PIX_HIGH;
  int TEMP_LOW;
  int TEMP_HIGH;

  float TEMP_BIAS;

  int COEFFICIENT;

  int COL_BEGIN;
  int COL_END;

  std::string NET_CONFIG_PATH;
  std::string NET_WEIGHTS_PATH;


  std::vector<float> angle_data;

  Parameter::Parameter() {
    std::cout << "-- Parameter Destructor successfully!" << std::endl;
  }

  Parameter::Parameter(const std::string file_path) {
    m_file_path = file_path;
    m_file_storage.open(m_file_path, cv::FileStorage::READ);
    if (!m_file_storage.isOpened()) {
      std::cout << "-- parameter file open failed!" << std::endl;
    } else {
      std::cout << "-- parameter file open successfully!" << std::endl;
    }
  }

  Parameter::~Parameter() {
    m_file_storage.release();
  }

  bool Parameter::open(const std::string file_path) {
    m_file_path = file_path;
    m_file_storage.open(m_file_path, cv::FileStorage::READ);
    if (!m_file_storage.isOpened()) {
      std::cout << "-- parameter file open failed!" << std::endl;
    } else {
      std::cout << "-- parameter file open successfully!" << std::endl;
    }
    return true;
  }

  void Parameter::load() {
    std::string home = getenv("HOME");

    CAMERA_PATH = std::string(m_file_storage["CAMERA_PATH"]);
    VIDEO_READ_PATH = std::string(m_file_storage["VIDEO_READ_PATH"]);
    VIDEO_WRITE_PATH = std::string(m_file_storage["VIDEO_WRITE_PATH"]);
    ANGLE_DATA_PATH = std::string(m_file_storage["ANGLE_DATA_PATH"]);

    CAMERA_COUNT = (int) m_file_storage["CAMERA_COUNT"];
    CAMERA_INDEX = (int) m_file_storage["CAMERA_INDEX"];

    FRAME_HEIGHT = (int) m_file_storage["FRAME_HEIGHT"];
    FRAME_WIDTH  = (int) m_file_storage["FRAME_WIDTH"];

    FRAME_RESIZE_HEIGHT = (int) m_file_storage["FRAME_RESIZE_HEIGHT"];
    FRAME_RESIZE_WIDTH = (int) m_file_storage["FRAME_RESIZE_WIDTH"];

    UART_PATH = std::string(m_file_storage["UART_PATH"]);
    UART_BAUD = (int) m_file_storage["UART_BAUD"];

    GRAY_BIAS = (int) m_file_storage["GRAY_BIAS"];

    MASK_RESIZE = (int) m_file_storage["MASK_RESIZE"];
    MASK_WIDTH = (int) m_file_storage["MASK_WIDTH"];
    MASK_LENGTH = (int) m_file_storage["MASK_LENGTH"];
    MASK_GAUSS_KERNEL = (int) m_file_storage["MASK_GAUSS_KERNEL"];
    MASK_THRESH = (double) m_file_storage["MASK_THRESH"];
    MASK_WEIGHT = (int) m_file_storage["MASK_WEIGHT"];
    HEAD_WEIGHT = (int) m_file_storage["HEAD_WEIGHT"];
    SIDE_WEIGHT = (int) m_file_storage["SIDE_WEIGHT"];

    MID_SCORE = (int) m_file_storage["MID_SCORE"];
    MID_FEVER = (int) m_file_storage["MID_FEVER"];

    TEMP_PIX_LOW = (int) m_file_storage["TEMP_PIX_LOW"];
    TEMP_PIX_HIGH = (int) m_file_storage["TEMP_PIX_HIGH"];
    TEMP_LOW = (int) m_file_storage["TEMP_LOW"];
    TEMP_HIGH = (int) m_file_storage["TEMP_HIGH"];

    TEMP_BIAS = (float) m_file_storage["TEMP_BIAS"];

    COEFFICIENT = (int) m_file_storage["COEFFICIENT"];

    COL_BEGIN = (int) m_file_storage["COL_BEGIN"];
    COL_END = (int) m_file_storage["COL_END"];

    NET_CONFIG_PATH = std::string(m_file_storage["NET_CONFIG_PATH"]);
    NET_WEIGHTS_PATH = std::string(m_file_storage["NET_WEIGHTS_PATH"]);
    std::cout << ANGLE_DATA_PATH << std::endl;
    angle_data_in.open(ANGLE_DATA_PATH);

    std::string angle_data_str,part_of_str;

    std::getline(angle_data_in,angle_data_str);
    std::istringstream Readstr(angle_data_str);
    for(int i = 0;i < COL_END-COL_BEGIN+1;i++){
        std::getline(Readstr,part_of_str,',');
//        std::cout << atof(part_of_str.c_str()) << std::endl;
        angle_data.emplace_back(atof(part_of_str.c_str()));
    }
  }

} // namespace para end
