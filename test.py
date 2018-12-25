
import subprocess

da0 = 3
da1 = 4
duration = 20

print("./dac8532_output "+str(da0)+" "+str(da1)+" "+str(duration))

try:
    pro = subprocess.Popen("./dac8532_test "+str(da0)+" "+str(da1)+" "+str(duration)).wait()

except KeyboardInterrupt:
    print("finish!")
