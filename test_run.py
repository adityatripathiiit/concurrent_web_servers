import subprocess
NUMBER_OF_CONCURRENT_USERS = 100

command = "./wclient localhost 5000 1"
for i in range(NUMBER_OF_CONCURRENT_USERS):
    command += " /benchmarking/load/"+str(i+1) + ".html "
    print(command)
    process = subprocess.Popen(command, shell=True)
    process.wait()
