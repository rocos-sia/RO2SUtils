#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"
#include <iostream>
#include <thread>
#include <chrono>

class ParameterSetter : public rclcpp::Node
{
public:
    ParameterSetter() : Node("parameter_setter")
    {
        // 声明参数
        // this->declare_parameter("marker_size", 0.0);
    }

   void setMarkerSize(double size)
{
    // 只有在参数值实际变化时才进行设置
    rclcpp::Client<rcl_interfaces::srv::SetParameters>::SharedPtr parameter_client =
        this->create_client<rcl_interfaces::srv::SetParameters>("/aruco_single/set_parameters"); // 假设节点名称已知
    auto request = std::make_shared<rcl_interfaces::srv::SetParameters::Request>();

    // 将 rclcpp::Parameter 转换为 rcl_interfaces::msg::Parameter_
    rcl_interfaces::msg::Parameter parameter_msg;
    parameter_msg.name = "marker_size";
    parameter_msg.value.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
    parameter_msg.value.double_value = size;

    request->parameters.push_back(parameter_msg);
    auto result_future = parameter_client->async_send_request(request);
    RCLCPP_INFO(this->get_logger(), "请求设置远程节点上的参数 marker_size 为 %f。", size);
}

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto parameter_setter = std::make_shared<ParameterSetter>();

    while (rclcpp::ok())
    {
        double marker_size;
        std::cout << "Enter marker size: ";

        if (!(std::cin >> marker_size))
        {
            std::cerr << "Invalid input. Please enter a valid number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // 使用成员函数setMarkerSize来设置参数，它只在值变化时才进行设置
        parameter_setter->setMarkerSize(marker_size);

        // 添加短暂延迟以减少频繁的参数设置操作
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rclcpp::shutdown();
    return 0;
}