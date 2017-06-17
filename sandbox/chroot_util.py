#!/bin/python2
import subprocess,sys,os
from shutil import copy 

binaries = ['/usr/bin/bash','/usr/bin/touch','/usr/bin/ls',
            '/usr/bin/gcc','/usr/bin/vim']

def get_dependencies(executable):
    lddout = subprocess.check_output(['ldd', executable])
    lddout = [i for i in lddout.split('\n')]
    result = []
    for i in lddout:
        for j in i.split():
            if '/' in j:
                result.append(j)
    return result


def copy_dependencies(deps,dest):
    for lib in deps:
        #print lib
        new_dir = dest+lib
        #directory may already exist
        try:
            os.makedirs(new_dir[:new_dir.rfind('/')])
        except OSError, e:
            if e.errno != 17:
                raise
            pass
        try:
            copy(lib,new_dir[:new_dir.rfind('/')])
        except:
            pass


def copy_binaries():
    new_dir = dest_dir + '/bin'
    try:
        os.makedirs(new_dir)
    except OSError, e:
            if e.errno != 17:
                raise
            pass
    for i in binaries:
        copy(i,new_dir)


if len(sys.argv) != 3:
    print 'usage: ./chroot_util.py executable abs_dest_dir_path'
    sys.exit(0) 

[temp1,executable,dest_dir] = sys.argv
copy_dependencies(get_dependencies(executable),dest_dir)
copy_binaries()

#copy dependencies so that we can run all binaries from chroot
for i in binaries:
    copy_dependencies(get_dependencies(i),dest_dir)

print 'copied dependencies'
print 'chroot '+ dest_dir +' /bin/bash'
