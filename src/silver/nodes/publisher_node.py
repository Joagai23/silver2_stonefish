# my_basic_package/publisher_node.py
# This script creates a simple ROS2 node that publishes a "Hello, World!" message.

import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class BasicPublisher(Node):
    """
    A simple publisher node that publishes a string message every 2 seconds.
    """
    def __init__(self):
        super().__init__('basic_publisher')
        # Create a publisher on the 'chatter' topic with a String message type.
        self.publisher_ = self.create_publisher(String, 'chatter', 10)
        # Set a timer to call the timer_callback function every 2 seconds.
        self.timer = self.create_timer(2.0, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        """
        Callback function that is called by the timer.
        It creates a message, logs it, and publishes it.
        """
        msg = String()
        msg.data = f'Hello, ROS2 World: {self.i}'
        self.publisher_.publish(msg)
        self.get_logger().info(f'Publishing: "{msg.data}"')
        self.i += 1

def main(args=None):
    """
    Main function to initialize and run the ROS2 node.
    """
    rclpy.init(args=args)
    basic_publisher = BasicPublisher()
    try:
        rclpy.spin(basic_publisher)
    except KeyboardInterrupt:
        pass
    finally:
        # Destroy the node explicitly
        basic_publisher.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()