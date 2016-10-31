import argparse
import os
from glob import glob
import urllib.request
import bz2
import tarfile
import shutil

parser = argparse.ArgumentParser(description='Automate project stuff')
parser.add_argument('-v',  help='verbose output')
parser.add_argument('--clean', help='Delete pdb, lib, exp, and exe', action='store_true')
parser.add_argument('--setup', help='setup project and dependencies', action='store_true')

args = parser.parse_args()
submodules = ['irsdk', 'sqlite3', 'libmicrohttpd']
cef_url = 'http://opensource.spotify.com/cefbuilds/cef_binary_3.2704.1414.g185cd6c_windows64.tar.bz2'
cef_dist_name = 'cef_binary_3.2704.1414.g185cd6c_windows64.tar.bz2'

def clean():
    paths = ['build/bin/Debug', 'build/bin/Release']
    for path in paths:
        for file in glob(path.append('/*.lib')):
            os.remove(file)
        for file in glob(path.append('/*.pdb')):
            os.remove(file)
        for file in glob(path.append('/*.exp')):
            os.remove(file)
        try:
            os.remove(path.append('/ECU.exe'))
        except FileNotFoundError:
            print("bin dirs already clean!")
                
def setup():
    for lib in submodules:
        if not os.path.exists('libs/' + lib):
            print('Creating directory {lib}'.format(lib=lib))
            os.makedirs('libs/' + lib)
            os.system('git submodule init')
            os.system('git submodule update')
        else:
            # Check if library git hash is up to date
            print('Library {lib} OK'.format(lib=lib))

    if not os.path.exists('libs/' + cef_dist_name[:-8]):
        print('CEF lib not found. Downloading...')
        urllib.request.urlretrieve(cef_url, 'libs/{dist}'.format(dist=cef_dist_name))
        cef_compressed = bz2.BZ2File('libs/{dist}'.format(dist=cef_dist_name))
        data = cef_compressed.read()
        cef_compressed.close()
        new_file_path = 'libs/{dist}'.format(dist=cef_dist_name)[:-4]
        with open(new_file_path, 'wb') as file:
            file.write(data)

        print('Untar cef')
        print('Opening {file}'.format(file=new_file_path))
        tar = tarfile.open(new_file_path)
        tar.extractall('libs/')
        tar.close()
        
        print('Removing {file}'.format(file=cef_dist_name))
        os.remove('libs/{file}'.format(file=cef_dist_name))
        print('Removing {file}'.format(file=cef_dist_name[:-4]))
        os.remove('libs/{file}'.format(file=cef_dist_name[:-4]))
    else:
        print("Library cef OK")
            

def main():
    if args.clean:
        clean()

    if args.setup:
        setup()


if __name__ == '__main__':
    main()
