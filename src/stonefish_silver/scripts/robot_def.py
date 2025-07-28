# Author: Giacomo Picardi
# Relevant motor combination
import numpy as np

R1 = np.array([1,2,3])
R2 = np.array([4,5,6])
R3 = np.array([7,8,9])
L1 = np.array([10,11,12])
L2 = np.array([13,14,15])
L3 = np.array([16,17,18])
LEG1 = R1
LEG2 = R2
LEG3 = R3
LEG4 = L1
LEG5 = L2
LEG6 = L3
ALL = np.array([1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18])

POS_UNIT                    = 0.088             # (DEG) A rotation of 1 (motor command) is 0.088 deg
VEL_UNIT                    = 0.229             # (RPM) A command of 1 in profile velocity is 0.229 rpm