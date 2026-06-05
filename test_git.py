import subprocess
print(subprocess.check_output(['git', 'log', '-n', '5', 'src/catch2/catch_approx.cpp']).decode('utf-8'))