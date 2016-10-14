#!/usr/bin/python3

import os
import subprocess
import shutil
import argparse
import paramiko


executable_name = 'Hexapod'
pi_address = '192.168.0.128'
pi_user = 'pi'
pi_password = 'raspberry'


def clean(build_dir):
    # use os and get rid of shutil
    shutil.rmtree(build_dir)
    os.mkdir(build_dir)


def run_cmake(build_dir):
    subprocess.check_call(['cmake',
                           '-DCMAKE_TOOLCHAIN_FILE=../rpi2-toolchain.cmake',
                           '..'], cwd=build_dir)


def build(build_dir):
    subprocess.check_call(['make'], cwd=build_dir)


def deploy(build_dir):
    subprocess.check_call(['scp', build_dir + '/' + executable_name,
                          pi_user + '@' + pi_address + ':/home/pi'])


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--clean', help='Clean build directory.',
                        action="store_true", default=False)
    parser.add_argument('-m', '--cmake', help='Run cmake.',
                        action="store_true", default=False)
    parser.add_argument('-d', '--debug', help='Build with debug symbols.',
                        action="store_true", default=False)
    parser.add_argument('-e', '--deploy', help='Deploy executable on target.',
                        action="store_true", default=False)
    parser.add_argument('-r', '--run', help='Run the executable on target.',
                        action="store_true", default=False)
    args = parser.parse_args()
    return args


def make_debug_build():
    # todo implement
    pass


def run():
    ssh = paramiko.SSHClient()
    ssh.load_system_host_keys()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(pi_address, username=pi_user, password=pi_password)
    run_hexapod_cmd = '/home/pi/Hexapod'
    ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(run_hexapod_cmd)
    print(ssh_stdout.channel.recv_exit_status())


def main():
    args = parse_arguments()
    build_dir = os.path.dirname(os.path.realpath(__file__)) + '/' + 'build'

    if args.clean:
        clean(build_dir)
    if args.debug:
        make_debug_build(build_dir)
    if args.cmake:
        run_cmake(build_dir)
    build(build_dir)
    if args.deploy:
        deploy(build_dir)
    if args.run:
        run()

if __name__ == "__main__":
    main()
