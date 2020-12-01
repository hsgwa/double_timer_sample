#include <memory>
#include <chrono>
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class DoubleTimer : public rclcpp::Node {
public: 
	DoubleTimer(): 
	Node("double_timer_node"), timer1_exec_once_(false), timer2_exec_once_(false)
	{
		timer1_ = create_wall_timer(1s, [this](){
			RCLCPP_INFO(this->get_logger(), "timer1 callback");

			timer1_exec_once_ = true;
			if (finished()) {
				rclcpp::shutdown();
			}
			this->timer1_->cancel();
		});
		timer2_ = create_wall_timer(1s, [this](){
			RCLCPP_INFO(this->get_logger(), "timer2 callback");
			timer2_exec_once_ = true;
			if (finished()) {
				rclcpp::shutdown();
			}
			this->timer1_->cancel();
		});
		auto callable = std::bind(&DoubleTimer::timer3_callback, this);
		timer3_ = create_wall_timer(1s, callable);

	}
	void timer3_callback() {
		RCLCPP_INFO(this->get_logger(), "timer3 callback");
		timer3_exec_once_ = true;
		if (finished()) {
			rclcpp::shutdown();
		}
		this->timer3_->cancel();
	}
	bool finished() {
		return timer1_exec_once_ && timer2_exec_once_ && timer3_exec_once_;
	}
private:
	
	rclcpp::TimerBase::SharedPtr timer1_;
	rclcpp::TimerBase::SharedPtr timer2_;
	rclcpp::TimerBase::SharedPtr timer3_;
	bool timer1_exec_once_;
	bool timer2_exec_once_;
	bool timer3_exec_once_;
};

int main(int argc, char *argv[]) {
	rclcpp::init(argc, argv);

	rclcpp::executors::SingleThreadedExecutor exec;
	auto node = std::make_shared<DoubleTimer>();
	exec.add_node(node);

	exec.spin();

	rclcpp::shutdown();
	return 0;
}
