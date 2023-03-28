#!/usr/bin/env python3

import argparse
import subprocess
import sys
import re
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('type', choices=['patch', 'minor', 'major'])
args = parser.parse_args()

mydir = Path(__file__).parent

def updateVersion(major, minor, patch):
    if args.type == 'patch':
        patch = int(patch) + 1
    elif args.type == 'minor':
        minor = int(minor) + 1
        patch = 0
    else:
        major = int(major) + 1
        minor = 0
        patch = 0
    return (major, minor, patch)


res = subprocess.run(['git', 'describe', '--tags', '--match', 'v*', '--abbrev=0'], 
                     cwd=mydir,
                     stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE,
                     text=True)

if res.returncode == 0:
    output = res.stdout.strip()

    regex = r'''v(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)$'''
    m = re.match(regex, output)
    if m is None:
        print('invalid existing version: ' + output, file=sys.stderr)
        sys.exit(1)

    groups = m.groupdict()
    major = groups['major']
    minor = groups['minor']
    patch = groups['patch']
else:
    major = 0
    minor = 0
    patch = 0

major, minor, patch = updateVersion(major, minor, patch)
print(f'{major}.{minor}.{patch}')


