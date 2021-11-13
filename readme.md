# yolov5-tensorrt-ros

本项目是移植大佬[enazoe](https://github.com/enazoe)的[yolo-tensorrt](https://github.com/enazoe/yolo-tensorrt)的项目，移植之后支持在Nvidia Jeston Xavier平台运行yolov5视频检测，并支持作为ros节点发送检测到的目标信息。

##  1.环境配置

+ Jetpack  4.4
+ Cuda 10.2
+ Cudnn
+ TensorRT

## 2.部署过程

#### 2.1克隆仓库

```bash
//自己移植好的仓库,将其放到配置好的ros工作空间下
git clone https://gitee.com/wqque/yolov5-tensorrt-ros.git
//yolo官方仓库，现在直接克隆的是v6.0,但这个项目要求使用v6.0的仓库，v5.0的yaml配置文件，放哪都行
git clone https://github.com/ultralytics/yolov5.git
```

#### 2.2 安装依赖

```bash
//yolov5按照官方教程安装依赖，官方要求python版本3.8
cd yolov5
pip install -r requirements.txt
//ros仓库的编译，进到工作空间目录下，执行catkin_make
```

#### 2.3训练自己的模型

##### 2.3.1 转换.pt文件

将yolov5-tensorrt-ros/scripts下的yaml2cfg文件复制到yolov5官方的文件夹下，然后将自己训练好的权重文件best.pt也放到yolov5主目录下，修改modules/yolov5s.yaml文件下nc的数目（注意yaml文件一定要是yolov5 v5.0的配置文件)，修改为自己训练模型的目标数，然后执行

```bash
python yaml2cfg.py --weights best.pt
```

执行完成之后会生成yolov5s.cfg和best.weights,将其复制到yolov5-tensorrt-ros/configs/yolov5-5.0下。

#### 2.3.2 生成.engine文件

修改src目录下sampledetector.cpp里.cfg和.weights文件的路径，然后执行

```bash
rosrun yolov5 yolov5
```

第一次执行这个节点时要编译生成.engine文件，需要耐心等待一段时间，编译完成后自动开始执行。后面再执行这个节点时，程序只需要加载这个文件就行了。



## 4.注意事项

1.每次转换自己训练的.pt文件时，注意要保证文件名与之前的文件名不同，否则程序在编译时会直接加载之前文件生成的.engine文件，程序不会报错，但应该无法正常检测。

2.注意要转换的是config文件，就是只需要在官方库的基础上更改nc数目的那个yaml文件，而不是自己数据集的data文件。yolov5现在在官方库直接克隆下来应该是v6.0，而yaml文件一定要使用v5.0的文件，否则会报段错误。

## 4. 参考资料

+ [yolov5官方](https://github.com/ultralytics/yolov5.git)

+ [https://github.com/enazoe/yolo-tensorrt.git](https://github.com/enazoe/yolo-tensorrt.git)

  
