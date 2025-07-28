#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Author: Giacomo Picardi

from __future__ import division
import numpy as np

from robot_def import *	# Constant definitions
import time
import math
from scipy.spatial import Delaunay
from scipy.linalg import svd
import transforms3d.euler as eul

static_poses_vel = {"slow": 0.5*np.ones(18),
                    "medium": 3*np.ones(18),
                    "fast": 7.5*np.ones(18)}

static_poses_pos = {"zero": np.zeros(18),
                    "min": -90*np.ones(18),
                    "max": 90*np.ones(18),
                    "std": np.array([45,50,100,0,50,100,-45,50,100,-45,50,100,0,50,100,45,50,100]),
                    "low_torque": np.array([45,-75,0, 0,-75,0, -45,-75,0, -45,-75,0, 0,-75,0, 45,-75,0]),
                    "folded": np.array([0,90,160,0,90,160,0,90,160,0,90,160,0,90,160,0,90,160]),
                    #"dragon": np.array([45,70,150,0,70,150,-45,70,150,-45,70,150,0,70,150,45,70,150])
                    "dragon": np.array([45,30,130,0,30,130,-45,30,130,-45,30,130,0,30,130,45,30,130])*np.pi/180,
                    "idle": np.array([45,50,130,0,50,130,-45,50,130,-45,50,130,0,50,130,45,50,130]) # dragon position to idle
                   }

class Robot:
        def __init__(self):
                #Constructor v1 gets only number of legs leg_num and number of joints per leg leg_joints as arguments
                # and applies a calibration procedure to set zeros and limits.
                #Constructor v2 also gets three arrays of size leg_num*leg_joints to specifify zero and limits without
                # doing the calibration procedure
                #Both initialize the motorHandler object, define relevant robot poses and set initial pose

                #Structural characteristics of the robot.
                self.leg_num = 6			#Number of legs
                self.leg_joints = 3	#Number of joints in a leg
                self.joint_num = self.leg_num*self.leg_joints  #Number of joints in the robot
                self.l0 = 8 					#Lengths of leg segments in cm
                self.l1 = 7
                self.l2 = 30
                self.l3 = 32.3 					#26 without foot
                self.body_length = 40			#Dimensions of body in cm
                self.body_width = 55
                #[min,max] range of joints positioning in rad.
                self.q_min = np.array([-np.pi/2, -np.pi/2, -np.pi/4])
                self.q_max = np.array([np.pi/2, np.pi/2, 3/4*np.pi])

                #Position of leg frames within the base frame.
                #self.leg_frame_b = np.zeros([self.leg_num,3])
                #legs frame created indepently from the number of legs selected
                self.leg_frame_b = np.zeros([6,3])
                self.leg_frame_b[0,:] = np.array([self.body_length/2, self.body_width/2, 0])
                self.leg_frame_b[1,:] = np.array([self.body_length/2, 0,0])
                self.leg_frame_b[2,:] = np.array([self.body_length/2, -self.body_width/2,0])
                self.leg_frame_b[3,:] = np.array([-self.body_length/2, self.body_width/2,0])
                self.leg_frame_b[4,:] = np.array([-self.body_length/2, 0,0])
                self.leg_frame_b[5,:] = np.array([-self.body_length/2, -self.body_width/2,0])
                #Leg frames and joints management.
                self.mir = np.array([1, 1, -1, -1, -1, 1])
                self.l_pos = np.array([1, 1, -1, 1, 1, -1])
                self.zeros = np.zeros(self.leg_joints*self.leg_num+1, dtype='int')
                self.sign = np.array([-1, -1, 1, -1, -1, 1, -1, 1, -1, -1, 1, -1, -1, 1, -1, -1, -1, 1])
                #coxa joint rest position in deg.
                #self.rest_coxa_angle = np.zeros(6) #mrudul coxa 0, erase after experiments
                self.rest_coxa_angle = np.array([45, 0, -45, -45, 0, 45])*np.pi/180
                self.rest_coxa_angle_rot = np.array([60, 0, -60, -60, 0, 60])*np.pi/180

                #Create point cloud of leg workspace for admissibility test.
                nsample = 11
                self.cloud = np.zeros([nsample**3,3])
                c = 0
                q1 = np.linspace(self.q_min[0], self.q_max[0], nsample)
                q2 = np.linspace(self.q_min[1], self.q_max[1], nsample)
                q3 = np.linspace(self.q_min[2], self.q_max[2], nsample)
                for i in range(0,nsample):
                        for j in range(0,nsample):
                                for k in range(0,nsample):
                                        self.cloud[c,:] = self.kine([q1[i], q2[j], q3[k]],1) #patch rapida. Checko le gambe 3,4,5 sul workspace delle 1,2,6
                                        c = c+1

                self.motor_ids = np.arange(1,self.leg_joints*self.leg_num+1)

                #Generate a joint matrix: 0 corresponds to motor disable, 1 to motor enable.
                self.joints_status = np.zeros(self.leg_joints*self.leg_num)
                self.Q_cur = np.zeros(self.leg_joints*self.leg_num)
                time.sleep(1)

                #First pose should be set in locomotion.py
                print("Successfully created a robot object")


        def __del__(self):
                #Last pose should be set in locomotion.py
                #input("Press ENTER to terminate")
                print("Successfully deleted a robot object")

        def kine_coxa_plane(self, q):
                #q = [q1, q2]: joint coords of femur  and  tibia  joints in [rad]
                #p = [x_p, z_p]: foot tip position (2D) in the coxa plane  [cm]
                q2 = q[0]
                q3 = q[1]

                x_p = self.l0 + self.l2*np.cos(q2) + self.l3*np.cos(q2-q3)
                z_p = -self.l2*np.sin(q2) - self.l3*np.sin(q2-q3)

                return np.array([x_p, z_p])

        def kine(self, q, leg_id):  #updated 21/5/2020 to discriminate kinematics between legs 1,2,6 and 3,4,5
                q1 = q[0]
                q2 = q[1]
                q3 = q[2]

                x = (self.l0 + self.l2*np.cos(q2) + self.l3*np.cos(q2-q3))*np.cos(q1) - self.mir[leg_id]*self.l1*np.sin(q1)
                y = (self.l0 + self.l2*np.cos(q2) + self.l3*np.cos(q2-q3))*np.sin(q1) + self.mir[leg_id]*self.l1*np.cos(q1)
                z = self.l2*np.sin(q2) + self.l3*np.sin(q2-q3)

                return np.array([x, y, z])

        def leg_inv_kine_coxa_plane(self, p):
                #q = [q1, q2]: joint coords of tibia and femur joints
                #p = [x_p, z_p]: foot tip position (2D) in the coxa plane
                x_p = p[0]-self.l0 #tibia joint is shifted by l0 along x-axis in the coxa plane
                z_p = -p[1] #coxa plane has flipped z axis

                a = math.sqrt(x_p**2+z_p**2)
                q2 = math.acos((self.l2**2+a**2-self.l3**2)/(2*self.l2*a)) + math.atan2(z_p,x_p)
                q3 = math.pi - math.acos((self.l2**2+self.l3**2-a**2)/(2*self.l2*self.l3))
                return np.array([q2, q3])


        def leg_inv_kine(self, tip_pos, leg_id):
                x = tip_pos[0]
                y = tip_pos[1]
                z = tip_pos[2]

                L = math.sqrt(x**2+y**2)
                beta = math.atan2(y,x)
                gamma = math.atan2(self.l1,math.sqrt(L**2-self.l1**2))
                q1 = beta - self.mir[leg_id]*gamma
                # x3,y3 are leg tip coords in coxa frame
                x3 = x*math.cos(q1)+y*math.sin(q1)-self.l0
                y3 = z
                a = math.sqrt(x3**2+y3**2)
                q2 = math.acos((self.l2**2+a**2-self.l3**2)/(2*self.l2*a)) + math.atan2(y3,x3)
                q3 = math.pi - math.acos((self.l2**2+self.l3**2-a**2)/(2*self.l2*self.l3))
                q = np.array([q1, q2, q3])
                return q

        def admissible(self, T):
                """
                Test if points in `T` are in `hull`

                `T` should be a `NxK` coordinates of `N` points in `K` dimensions
                `hull` is either a scipy.spatial.Delaunay object or the `MxK` array of the
                coordinates of `M` points in `K`dimensions for which Delaunay triangulation
                will be computed
                """
                if not isinstance(self.cloud,Delaunay):
                        ws = Delaunay(self.cloud)
                        test = ws.find_simplex(T)>=0
                if np.size(test) == 1: #generalize to scalar case
                    return test
                else:
                    return all(test)

        def trj_gen(self,w,h,alpha,s,beta,delta_z,phi,n_step,gait_t,leg_id, rotation):
                """Gait parameters
                w = 40 #gait width [cm] ---> [x_c, y_c] for each leg
                h = 25 #gait height [cm]
                alpha = 90*np.pi/180 #direction [deg]
                s = 10 #step length [cm]
                beta = 2/3 #duty cycle
                delta_z = 10 #ground clearance [cm]
                phi = 0 #phase lag [deg]
                n_step = 120
                gait_t = 5 #period [s]
                leg_id = 0"""
                # check on parameter alpha
                if rotation == 0:
                    alpha = alpha*np.pi/180 # convert alpha to radians!!!!
                else:
                    if alpha == 0:
                        alpha = 1
                    alpha = alpha/np.abs(alpha) #when rotation is 1, alpha must be 1 or -1
                    s = s*np.pi/180

            	# find [x_c, y_c] for leg leg_id and gait_width w
                r = self.body_length/2 + w #radius of the circle centered in the middle of the body frame on which foot tips lie in rest postion
                d_leg = np.linalg.norm(self.leg_frame_b[leg_id,:]) #distance between leg_frame_b(leg_id) and body frame centre
                tip_coxa = np.array([(r - d_leg), self.mir[leg_id]*self.l1]) # (x,y) coords of foot tip in coxa frame
                R_coxa_leg = np.array([[np.cos(self.rest_coxa_angle[leg_id]), -np.sin(self.rest_coxa_angle[leg_id])],
        	    		       [np.sin(self.rest_coxa_angle[leg_id]), np.cos(self.rest_coxa_angle[leg_id])]])
                p_c = np.dot(R_coxa_leg,tip_coxa)
                x_c = p_c[0]
                y_c = p_c[1]
                n_s = int(n_step*beta)
                n_f = n_step-n_s

                if rotation == 0:
                    # stance phase trajectory
                    x_s = np.linspace(x_c + s/2*np.cos(alpha), x_c - s/2*np.cos(alpha), n_s)
                    y_s = np.linspace(y_c + s/2*np.sin(alpha), y_c - s/2*np.sin(alpha), n_s)
                    z_s = -h*np.ones([n_s])
                    # flying phase trajectory
                    t_f = np.linspace(0,1,n_f); #free parameter for trajectory definition
                    x_f = np.linspace(x_c - s/2*np.cos(alpha), x_c + s/2*np.cos(alpha), n_f)
                    y_f = np.linspace(y_c - s/2*np.sin(alpha), y_c + s/2*np.sin(alpha), n_f)
                    z_f = -(h - delta_z*np.sin(np.pi*t_f))
                else:
                    if leg_id < 3:
                        sign = -1
                    else:
                        sign = 1
                    # stance phase trajectory
                    t_s = alpha*np.linspace(-0.5,0.5,n_s)
                    x_s = r*np.cos((s*t_s+self.rest_coxa_angle_rot[leg_id]))+sign*self.leg_frame_b[leg_id,0]
                    y_s = r*np.sin((s*t_s+self.rest_coxa_angle_rot[leg_id]))+sign*self.leg_frame_b[leg_id,1]
                    z_s = -h*np.ones([n_s])
                    # flying phase trajectory
                    t_f = alpha*np.linspace(-0.5,0.5,n_f) #free parameter for trajectory definition
                    x_f = r*np.cos((-s*t_f+self.rest_coxa_angle_rot[leg_id]))+sign*self.leg_frame_b[leg_id,0]
                    y_f = r*np.sin((-s*t_f+self.rest_coxa_angle_rot[leg_id]))+sign*self.leg_frame_b[leg_id,1]
                    z_f = -(h - delta_z*np.cos(np.pi*t_f))

                if rotation == 0 and leg_id > 2:
                    x_s = np.flipud(x_s)
                    y_s = np.flipud(y_s)
                    x_f = np.flipud(x_f)
                    y_f = np.flipud(y_f)

                # merge phases
                x = np.hstack((x_s,x_f))
                y = np.hstack((y_s,y_f))
                z = np.hstack((z_s,z_f))
                T = np.vstack((x,y,z))
                # check admissibility and compute inverse kinematics and joint velocities
                Q = np.zeros([3, n_step])
                Qdot = np.zeros([3, n_step])
                admiss = self.admissible(np.transpose(T))
                if admiss:
                    for i in range(0,n_step):
                        Q[:,i] = self.leg_inv_kine(T[:,i], leg_id)
                    delta_t = gait_t/n_step
                    Qdot[:,0] = Q[:,n_step-1]-Q[:,0]
                    Qdot[:,1:n_step] = np.abs(np.diff(Q[:,0:n_step])/delta_t)
                else:
                    print('non admissible trj for leg_id: '+ str(leg_id))

        	# apply phase lag
                index = int(n_step*(phi/360))
                Q_tmp = np.zeros(Q.shape)
                Qdot_tmp = np.zeros(Qdot.shape)
                T_tmp = np.zeros(T.shape)
                Q_tmp[:, 0:index] = Q[:,n_step-index:n_step]
                Q_tmp[:,index:n_step] = Q[:,0:n_step-index]
                Qdot_tmp[:, 0:index] = Qdot[:,n_step-index:n_step]
                Qdot_tmp[:,index:n_step] = Qdot[:,0:n_step-index]
                T_tmp[:, 0:index] = T[:,n_step-index:n_step]
                T_tmp[:,index:n_step] = T[:,0:n_step-index]

                return T_tmp, Q_tmp, Qdot_tmp, admiss
        
        def change_configuration(self, q_des, q_cur):
                gait_t = 3 #[s]
                #assumption: configuration with neutral asset <--> all legs have the same height when they touch the ground
                new_leg_heights = np.zeros(6)
                old_leg_heights = np.zeros(6)
                for i in np.arange(0,6):
                        np_i = self.kine(q_des[3*i:3*i+3],i) #possibile fonte di errore
                        op_i = self.kine(q_cur[3*i:3*i+3],i)
                        new_leg_heights[i] = np_i[2]
                        old_leg_heights[i] = op_i[2]
                nh = min(new_leg_heights)
                oh = min(old_leg_heights)

                if np.allclose(q_des, q_cur):
                        print("same pose as before")
                        return True
                elif np.abs(nh-oh)<0.1:
                        n1 = 0
                        n2 = 30
                else:
                        n1 = 60
                        n2 = 30
                n = n1 + 2*n2
                t = np.linspace(0,1,n2)
                delta_t = gait_t/n
                Qq = np.zeros([18,n])
                Qdot = np.zeros([18,n])
                admiss = np.ones(6, dtype=bool)

                for i in np.arange(0,6):
                        np_i = self.kine(q_des[3*i:3*i+3],i) #new foot tip position
                        op_i = self.kine(q_cur[3*i:3*i+3],i) #old foot tip position
                        #FIRST PHASE--> All legs go to new height with linear trajectory
                        xi_1 = np.ones(n1)*op_i[0]
                        yi_1 = np.ones(n1)*op_i[1]
                        zi_1 = np.linspace(op_i[2], np_i[2], n1)
                        #SECOND PHASE-->Tripod1 goes to new feet position with circumference arcs, Tripod2 stays
                        if i==0 or i==2 or i==4:
                                xi_2 = np.linspace(op_i[0], np_i[0], n2)
                                yi_2 = np.linspace(op_i[1], np_i[1], n2)
                                zi_2 = np_i[2] + 5*np.sin(np.pi*t)
                        else:
                                xi_2 = np.ones(n2)*op_i[0]
                                yi_2 = np.ones(n2)*op_i[1]
                                zi_2 = np.ones(n2)*np_i[2]
                        #THIRD PHASE --> Tripod1 stays, Tripod2 goes to new feet position with circumference arcs
                        if i==0 or i==2 or i==4:
                                xi_3 = np.ones(n2)*np_i[0]
                                yi_3 = np.ones(n2)*np_i[1]
                                zi_3 = np.ones(n2)*np_i[2]
                        else:
                                xi_3 = np.linspace(op_i[0], np_i[0], n2)
                                yi_3 = np.linspace(op_i[1], np_i[1], n2)
                                zi_3 = np_i[2] + 5*np.sin(np.pi*t)
                        # merge phases
                        x_i = np.hstack((xi_1,xi_2,xi_3))
                        y_i = np.hstack((yi_1,yi_2,yi_3))
                        z_i = np.hstack((zi_1,zi_2,zi_3))
                        T_i = np.vstack((x_i,y_i,z_i))
                        # check admissibility and compute inverse kinematics and joint velocities
                        admiss[i] = self.admissible(np.transpose(T_i))
                        if admiss[i]:
                                for j in range(0,n):
                                        #print(i,j)
                                        #print(T_i[:,j])
                                        Qq[3*i:3*i+3,j] = self.leg_inv_kine(T_i[:,j], i) # leg_id=1...se metto i non funziona. C'è qualche incongruenza di segno sulle cinematiche
                                Qdot[3*i:3*i+3,0] = Qq[3*i:3*i+3,n-1]-Qq[3*i:3*i+3,0]
                                Qdot[3*i:3*i+3,1:n] = np.abs(np.diff(Qq[3*i:3*i+3,0:n])/delta_t)
                        else:
                                print('non admissible trj for leg_id: '+ str(i))

                # returns Qq trajectory, Qdot derivative of trajectory, Admiss if trajectory is feasible, n number of steps
                return Qq, Qdot, admiss, n, delta_t


        def get_leg_slice(self, data, leg_id):
                #Data: array of size leg_joints*joint_num
                #leg_id: id of a leg
                #return: a slice of data corresponding to the joints in leg with id leg_id
                #leg_id=1--->[data[1], data[2], data[3]]
                return data[self.leg_joints*(leg_id-1):self.leg_joints*(leg_id-1)+self.leg_joints]

        def d_from_q(self, motor_ids, q):
                #q: joint coordinate in deg
                #d: converts q to goal position command of dynamixel
                #it requires the zero calibrated (or passed) in the contructor and motor id
                try: #handles case of single motor
                        d = np.zeros(len(motor_ids), dtype = 'int')
                except:
                        d = self.sign[motor_ids-1]*q/POS_UNIT+self.zeros[motor_ids-1]
                        return d
                current = 0
                for i in motor_ids:
                        d[current] = self.sign[i-1]*q[current]/POS_UNIT+self.zeros[i-1]
                        current = current+1
                return d

        def q_from_d(self, motor_ids, d):
                #inverse of d_from_q
                q = np.zeros(len(motor_ids))
                current = 0
                for i in motor_ids:
                        q[current] = self.sign[i-1]*(d[current] - self.zeros[i-1])*POS_UNIT
                        current = current+1
                return q

        def profvel_from_qdot(self, qdot):
                #converts joint velocity qdot in deg/s to profvel motor command
                profvel =  qdot*6/VEL_UNIT
                return profvel.astype(int)

        def qdot_from_profvel(self, qdot):
                #inverse of profvel_from_qdot
                return qdot*VEL_UNIT/6