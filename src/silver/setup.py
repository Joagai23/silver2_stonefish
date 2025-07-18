import os
from glob import glob
from setuptools import setup

package_name = 'my_basic_package'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # This line finds and installs all .launch.py files in the 'launch' directory
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='user@example.com',
    description='A basic ROS2 publisher and subscriber package.',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'publisher = my_basic_package.publisher_node:main',
            'subscriber = my_basic_package.subscriber_node:main',
        ],
    },
)