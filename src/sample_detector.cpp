/*
 * @Descripttion: 
 * @version: 
 * @Author: qwq
 * @Date: 2021-10-15 15:24:46
 * @LastEditors: qwq
 * @LastEditTime: 2021-10-15 15:42:22
 */

#include "class_timer.hpp"
#include "class_detector.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "yolov5/Target.h"


#include <memory>
#include <thread>


int main(int argc,char *argv[])
{
	//ros node init
	ros::init(argc,argv,"yolov5");
	ros::NodeHandle n;

	//publisher
	ros::Publisher pub = n.advertise<yolov5::Target>("detector",1000);


	//添加配置文件和权重文件
	Config config_v5;
	config_v5.net_type = YOLOV5;
	config_v5.detect_thresh = 0.5;
	config_v5.file_model_cfg = "/home/kjzxcsq/dev/catkin_ws/src/yolov5-tensorrt-ros/configs/yolov5-5.0/yolov5s.cfg";
	config_v5.file_model_weights = "/home/kjzxcsq/dev/catkin_ws/src/yolov5-tensorrt-ros/configs/yolov5-5.0/best.weights";
	config_v5.inference_precison = FP16;

	std::unique_ptr<Detector> detector(new Detector());
	detector->init(config_v5);

	std::vector<BatchResult> batch_res;
	std::vector<cv::Mat> batch_img;
	Timer timer;
	yolov5::Target target;
	char key;
	double fps;
	double t;

	
    cv::Mat frame;
    cv::VideoCapture cap(0);
    if(!cap.isOpened())
    {
        std::cout<<"camrea 0 fail";
        return -1;
    }

	cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);

	while(ros::ok())
	{
		//prepare batch data
		cap>>frame;
		cv::Mat temp0 = frame.clone();
		batch_img.push_back(temp0);

		//detect
		timer.reset();
		detector->detect(batch_img, batch_res);
		fps = 1000.0/timer.elapsed();

		//disp
		if(!batch_res[0].empty())
		{
			for (const auto &r : batch_res[0])
			{
				//std::cout <<"batch "<<i<< " id:" << r.id << " prob:" << r.prob << " rect:" << r.rect << std::endl;
				cv::rectangle(batch_img[0], r.rect, cv::Scalar(255, 0, 0), 2);
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << "id:" << r.id << "  score:" << r.prob;
				cv::putText(batch_img[0], stream.str(), cv::Point(r.rect.x, r.rect.y - 5), 0, 0.5, cv::Scalar(0, 0, 255), 2);
					
				//detect publisher
				target.id = r.id;
				target.prob = r.prob;
				target.x = r.rect.x + r.rect.width/2;
				target.y = r.rect.y + r.rect.height/2;
				pub.publish(target);
				ROS_INFO("%d",r.id);
			}
		}
		else
		{
			target.id = -1;
			pub.publish(target);
			ROS_INFO("-1");
		}

		
		cv::putText(batch_img[0],"FPS: "+ std::to_string(fps),cv::Point(0,400), 0, 1, cv::Scalar(0, 255, 255), 2);
		cv::namedWindow("video", cv::WINDOW_NORMAL);
		cv::imshow("video", batch_img[0]);
		batch_img.pop_back();
		
		key = cv::waitKey(1);
        if(key == 27) 
            break;

		ros::spinOnce();

	}

}
