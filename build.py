#!/usr/bin/env python3

import argparse
import os
import json


CONFIG_FILE_PATH = 'config.json'

def parse_args():
    parser = argparse.ArgumentParser(description='This script builds and '
                                     'deploys hexapod robot project.')
    parser.add_argument('-d', '--debug', action = 'store_true', default=False,
                        help = 'Create a debug build.',  required=False)
    parser.add_argument('-i', '--ip', help='Raspberry Pi IP address in a local'
                        ' network', required=False)
    parser.add_argument('-c', '--clean', action = 'store_true',
                        help='Determines if build directory should be cleaned'
                        'before the build.', required=False,
                        default=False)

    return parser.parse_args()


def prepare_build_dir(should_clean):
    if not os.path.isdir('build'):
        os.makedirs('build')
    if should_clean:
        subdirectories = os.listdir('build/')
        for entry in subdirectories:
            os.remove(entry)


def prepare_rootfs(rootfs_path, ip):
    print('')

def parse_config():
    with open(CONFIG_FILE_PATH) as config_file:
        data = json.load(config_file)
    return data



def main():
    args = parse_args()
    config = parse_config()
    toolchain_path = config["toolchain"]
    rootf_path = config["rootfs"]
    prepare_build_dir(args.clean)
    prepare_rootfs(rootfs_path, args.ip)


if __name__ == '__main__':
    main()
