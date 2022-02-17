#include "Parameter.h"
namespace para {

    
    std::string CAMERA_PATH;
    cv::Mat intrMatrix;
    cv::Mat distCoeffs;
    double Q;
    double R;
    std::vector<cv::Point3f> corners_at_box = {cv::Point3f(-60,30,0),
                                               cv::Point3f(60,30,0),
                                               cv::Point3f(60,-30,0),
                                               cv::Point3f(-60,-30,0)};//corner at box's frame
    Eigen::Vector3d t(0,-29.79,100.228);

    Parameter::Parameter() {
        // std::cout << "class Parameter build !" << std::endl;
    }
    Parameter::Parameter(const std::string path) {
        m_path = path;
        m_file.open(m_path,cv::FileStorage::READ);
        if(!m_file.isOpened()) {
            std::cout << "para open failed !" << std::endl;
        } else {
            load();
            std::cout << "para open successfully !" << std::endl;
        }
    }

    Parameter::~Parameter() {
        m_file.release();
    }


    void Parameter::open(const std::string path) {
        m_path = path;
        m_file.open(m_path,cv::FileStorage::READ);
        if(!m_file.isOpened()) {
            std::cout << "para open failed !" << std::endl;
        } else {
            load();
            std::cout << "para open successfully !" << std::endl;
        }
    }

    void Parameter::load() {
        // m_file["CameraPath"] >> CAMERA_PATH;
        m_file["intrMatrix"] >> intrMatrix;
        m_file["distCoeffs"] >> distCoeffs;
        m_file["Q"] >> Q;
        m_file["R"] >> R;
    }
}