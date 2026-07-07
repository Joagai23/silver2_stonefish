#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Author: Giacomo Picardi (modified for cmd_vel integration)
# Maintainer: Jorge Aguirregomezcorta Aina (modified for Stonefish Simulator)

import os, sys, time, signal
import numpy as np
import csv
from datetime import datetime
import math

import rclpy
from rclpy.node import Node
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup

from std_msgs.msg import Float64MultiArray
from sensor_msgs.msg import JointState
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import robot

should_quit = False

def handler(signum, frame):
    global should_quit
    should_quit = True

class OmnidirectionalGaitController(Node):

    def __init__(self):
        super().__init__('omnidirectional_gait_controller')
        self.group = ReentrantCallbackGroup()

        # Setup Data Logging Infrastructure
        os.makedirs('data', exist_ok=True)
        timestamp_str = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        self.filename = f"data/motion_and_effort_{timestamp_str}.csv"
        self.csv_file = open(self.filename, mode='w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        self.csv_writer.writerow([
            'sec', 'nanosec', 'x', 'y', 'z', 'is_moving', 
            'coxa_mean_effort', 'femur_mean_effort', 'tibia_mean_effort'
        ])
        self.get_logger().info(f"Logging motion, pose & efforts to {self.filename}")

        # 20Hz Data Logging Timer
        self.timer = self.create_timer(0.05, self.log_synced_data)

        # Tracking Variables
        self.current_x = 0.0
        self.current_y = 0.0
        self.current_z = 0.0
        self.last_x = 0.0
        self.last_y = 0.0
        self.is_moving = False
        self.current_sec = 0
        self.current_nanosec = 0
        
        self.latest_coxa_mean = 0.0
        self.latest_femur_mean = 0.0
        self.latest_tibia_mean = 0.0

        # Fixed gait parameters
        self.gait_width = 40.0
        self.gait_height = 30.0
        self.step_length = 20.0
        self.duty_cycle = 0.6
        self.ground_clearance = 10.0
        self.phase_lag = np.array([0.0, 180.0, 0.0, 180.0, 0.0, 180.0])
        self.nstep = 30
        self.rotation = 0.0
        self.direction = 0.0
        self.period = 3.0

        # Latest cmd_vel
        self.latest_cmd = None
        self.last_cmd = None

        # Robot model and communication setup
        self.robot = robot.Robot()
        self.Q_current = robot.static_poses_pos['dragon']

        self.joint_order = [
            'silver2/Joint_L0_Coxa', 'silver2/Joint_L0_Femur', 'silver2/Joint_L0_Tibia',
            'silver2/Joint_L1_Coxa', 'silver2/Joint_L1_Femur', 'silver2/Joint_L1_Tibia',
            'silver2/Joint_L2_Coxa', 'silver2/Joint_L2_Femur', 'silver2/Joint_L2_Tibia',
            'silver2/Joint_L3_Coxa', 'silver2/Joint_L3_Femur', 'silver2/Joint_L3_Tibia',
            'silver2/Joint_L4_Coxa', 'silver2/Joint_L4_Femur', 'silver2/Joint_L4_Tibia',
            'silver2/Joint_L5_Coxa', 'silver2/Joint_L5_Femur', 'silver2/Joint_L5_Tibia',
        ]

        self.joint_state_subscriber = self.create_subscription(
            JointState, '/joint_states_stonefish', 
            self.joint_state_subscriber_callback, 10, 
            callback_group=self.group)
            
        self.pose_subscriber = self.create_subscription(
            Odometry, '/silver2/pose', 
            self.pose_callback, 10, 
            callback_group=self.group)

        self.create_subscription(Twist, '/cmd_vel', self.cmd_vel_callback, 10)
        self.pid_pos_publisher = self.create_publisher(JointState, '/joint_command_stonefish', 10)

    def pose_callback(self, msg):
        # Extract timestamp and global coordinates from Action Graph TransformStamped
        self.current_sec = msg.header.stamp.sec
        self.current_nanosec = msg.header.stamp.nanosec
        self.current_x = msg.pose.pose.position.x
        self.current_y = msg.pose.pose.position.y
        self.current_z = msg.pose.pose.position.z
        
        # Determine motion state (0.1mm displacement tolerance)
        if math.hypot(self.current_x - self.last_x, self.current_y - self.last_y) > 0.0001:
            self.is_moving = True
        else:
            self.is_moving = False
            
        self.last_x = self.current_x
        self.last_y = self.current_y

    def joint_state_subscriber_callback(self, msg):
        joint_position_dict = dict(zip(msg.name, msg.position))
        for i, joint_name in enumerate(self.joint_order):
            if joint_name in joint_position_dict:
                self.Q_current[i] = joint_position_dict[joint_name]
            else:
                self.get_logger().warn(f"Joint {joint_name} not found in message")

        # --- Effort Mean Calculations & Logging (Synchronized via loop) ---
        if msg.effort:
            coxa, femur, tibia = [], [], []
            for name, effort in zip(msg.name, msg.effort):
                name_lower = name.lower()
                if 'coxa' in name_lower: coxa.append(effort)
                elif 'femur' in name_lower: femur.append(effort)
                elif 'tibia' in name_lower: tibia.append(effort)
            
            self.latest_coxa_mean = sum(coxa) / len(coxa) if coxa else 0.0
            self.latest_femur_mean = sum(femur) / len(femur) if femur else 0.0
            self.latest_tibia_mean = sum(tibia) / len(tibia) if tibia else 0.0

    def log_synced_data(self):
        # The single authoritative point where data gets flushed to the CSV
        self.csv_writer.writerow([
            self.current_sec, self.current_nanosec,
            self.current_x, self.current_y, self.current_z,
            1 if self.is_moving else 0,
            self.latest_coxa_mean, self.latest_femur_mean, self.latest_tibia_mean
        ])
        self.csv_file.flush()

    def cmd_vel_callback(self, msg):
        self.latest_cmd = msg

    def change_configuration_loop(self, Q_target):
        Q_cc, _, Admiss_cc, nstep_cc, ctrl_timestep = self.robot.change_configuration(Q_target, self.Q_current)
        if not all(Admiss_cc):
            self.get_logger().warn("Configuration change outside workspace")
            return

        for i in range(nstep_cc):
            if should_quit:
                break
            msg = Float64MultiArray()
            msg.data = Q_cc[:, i].tolist()
            # Change from Array to Joint State goes here
            self.publish_joint_setpoint(msg.data, ctrl_timestep)

    def omni_loop(self):
        i = 0
        Q_omni = np.zeros((18, self.nstep))
        ctrl_timestep = self.period / self.nstep

        while rclpy.ok() and not should_quit:
            #rclpy.spin_once(self, timeout_sec=0.1)

            if self.latest_cmd is None:
                time.sleep(0.1)
                continue

            vx, vy, wz = self.latest_cmd.linear.x, self.latest_cmd.linear.y, self.latest_cmd.angular.z

            if np.isclose(vx, 0.0, atol=1e-3) and np.isclose(vy, 0.0, atol=1e-3) and np.isclose(wz, 0.0, atol=1e-3):
                time.sleep(0.1)
                continue

            cmd_changed = False
            if self.last_cmd is None:
                cmd_changed = True
            else:
                dvx = abs(vx - self.last_cmd.linear.x)
                dvy = abs(vy - self.last_cmd.linear.y)
                dwz = abs(wz - self.last_cmd.angular.z)
                if dvx > 1e-3 or dvy > 1e-3 or dwz > 1e-3:
                    cmd_changed = True

            if cmd_changed:
                self.last_cmd = self.latest_cmd

                if np.isclose(vx, 0.0, atol=1e-3) and np.isclose(vy, 0.0, atol=1e-3) and not np.isclose(wz, 0.0, atol=1e-3):
                    self.rotation = 1
                    self.direction = np.sign(wz) * 90.0  # +90 for clockwise, -90 for counter-clockwise
                    vel_mag = min(abs(wz), 1.0)
                else:
                    self.rotation = 0
                    self.direction = np.arctan2(vy, vx) * 180.0 / np.pi
                    vel_mag = np.clip(np.linalg.norm([vx, vy]), 0.0, 1.0)

                self.period = 5.0 - 4.0 * vel_mag  # range 5.0 (slow) to 1.0 (fast)
                ctrl_timestep = self.period / self.nstep

                Admiss_omni = [False] * 6
                for leg_id in range(6):
                    _, Q_omni[3*leg_id:3*leg_id+3, :], _, Admiss_omni[leg_id] = \
                        self.robot.trj_gen(self.gait_width, self.gait_height, self.direction, self.step_length,
                                           self.duty_cycle, self.ground_clearance, self.phase_lag[leg_id],
                                           self.nstep, self.period, leg_id, self.rotation)

                if not all(Admiss_omni):
                    self.get_logger().warn("Gait not feasible for current direction")
                    continue

                if i == 0:
                    self.change_configuration_loop(Q_omni[:, 0])

            for step in range(self.nstep):
                if should_quit:
                    break
                msg = Float64MultiArray()
                msg.data = Q_omni[:, step].tolist()
                # Change from Array to Joint State goes here
                self.publish_joint_setpoint(msg.data, ctrl_timestep)
                
            i += 1

    # Create ROS2 JointState Message using Position Array
    def publish_joint_setpoint(self, pos_array, timestep):
        # Safety check
        if len(pos_array) != len(self.joint_order):
            self.get_logger().error(
                f"Failed to convert message: "
                f"The number of joint names ({len(self.joint_order)}) does not match "
                f"the number of received positions ({len(pos_array)})."
            )
    
        joint_state_msg = JointState()
        joint_state_msg.name = self.joint_order
        joint_state_msg.position = pos_array
        joint_state_msg.velocity = []
        joint_state_msg.effort = []

        # Publish and Wait
        self.pid_pos_publisher.publish(joint_state_msg)
        
        # Non-blocking time step execution to prevent thread starvation
        start_time = self.get_clock().now().seconds_nanoseconds()
        start_sec = start_time[0] + start_time[1] * 1e-9
        
        while rclpy.ok() and not should_quit:
            rclpy.spin_once(self, timeout_sec=0.005) # 200Hz
            current_time = self.get_clock().now().seconds_nanoseconds()
            current_sec = current_time[0] + current_time[1] * 1e-9
            
            if (current_sec - start_sec) >= timestep:
                break

    def destroy_node(self):
        # Safely close your logger file descriptor upon node destruction
        if self.csv_file:
            self.csv_file.close()
            self.get_logger().info("Motion & Effort CSV data logger cleanly saved/closed.")
        super().destroy_node()

if __name__ == '__main__':
    rclpy.init()
    gait_controller = OmnidirectionalGaitController()
    
    # Use a MultiThreadedExecutor
    executor = MultiThreadedExecutor()
    executor.add_node(gait_controller)
    
    # Run the controller in a separate thread so it doesn't block callbacks
    import threading
    controller_thread = threading.Thread(target=gait_controller.omni_loop, daemon=True)
    controller_thread.start()
    
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        gait_controller.destroy_node()
        rclpy.shutdown()