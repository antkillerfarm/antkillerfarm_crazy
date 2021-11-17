#!/usr/bin/env python3
import sys
import os
import logging
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))

logging.basicConfig(format="%(asctime)s %(name)s [%(levelname)s] - %(message)s", level=logging.DEBUG)
log = logging.getLogger("Build")

def run_subprocess(args, cwd=None, capture=False, shell=False, env={}):
    log.debug("Running subprocess in '{0}'\n{1}".format(cwd or os.getcwd(), args))
    my_env = os.environ.copy()

    stdout, stderr = (subprocess.PIPE, subprocess.STDOUT) if capture else (None, None)
    my_env.update(env)

    completed_process = subprocess.run(args, cwd=cwd, check=True,
        stdout=stdout, stderr=stderr, env=my_env, shell=shell)
    log.debug("Subprocess completed. Return code=" + str(completed_process.returncode))
    return completed_process

def main():
    # args = ["ls -al ."]
    # run_subprocess(args, cwd=script_dir, shell=True)

    # args = ["ls", "-al", "."]
    # run_subprocess(args, cwd=script_dir, shell=False)

    # args = ["ls -al . && ls"]
    # run_subprocess(args, cwd=script_dir, shell=True)

    args = ["ls -al .>1.txt"]
    subprocess.call(args, cwd=script_dir, shell=True)

if __name__ == "__main__":
    sys.exit(main())
