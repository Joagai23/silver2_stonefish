# my_basic_package/subscriber_node.py
# This script creates a simple ROS2 node that subscribes to messages.

import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class BasicSubscriber(Node):
    """
    A simple subscriber node that listens to the 'chatter' topic.
    """
    def __init__(self):
        super().__init__('basic_subscriber')
        # Create a subscriber to the 'chatter' topic.
        self.subscription = self.create_subscription(
            String,
            'chatter',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        """
        Callback function that is called whenever a message is received.
        """
        self.get_logger().info(f'I heard: "{msg.data}"')

def main(args=None):
    """
    Main function to initialize and run the ROS2 node.
    """
    rclpy.init(args=args)
    basic_subscriber = BasicSubscriber()
    try:
        rclpy.spin(basic_subscriber)
    except KeyboardInterrupt:
        pass
    finally:
        # Destroy the node explicitly
        basic_subscriber.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()