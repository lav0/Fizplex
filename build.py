from subprocess import call
from time import time
from termcolor import colored

print colored("\nClang-Format ...\n", 'blue')
call('clang-format-4.0 -i src/*.cc src/*.h', shell=True)

print colored("CppCheck ...\n", 'blue')
call([  'cppcheck',
        '--enable=all', '--inconclusive',
        '--suppressions-list=cpp_check_suppressions',
        '--quiet', '--template={file}:{line}: {message} [{id}]',
        'src'])

print colored("Make ...\n", 'blue')
call(['make', '-C', 'build/', '-j6'])

print colored("Run tests ...\n", 'blue')
call(['./build/tests'])
