#ifndef PARAMETER_H
#define PARAMETER_H
#include <iostream>
#include <opencv2/core.hpp>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>
namespace para {
    extern std::string CAMERA_PATH;
    extern cv::Mat intrMatrix;
    extern cv::Mat distCoeffs;
    extern std::vector<cv::Point3f> corners_at_box;
    extern Eigen::Vector3d t;
    extern double Q;
    extern double R;

    class Parameter {
        private:
        cv::FileStorage m_file;
        std::string m_path;
        public:
        Parameter();
        Parameter(const std::string para_file_path);
        ~Parameter();
        
        void open(const std::string para_file_path);
        void load();
    };
}
#endif
