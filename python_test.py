
import subprocess

da0 = 3
da1 = 4
duration = 20


try:
    subprocess.call(["./dac8532_test",str(da0),str(da1),str(duration)])

except KeyboardInterrupt:
    print("finish!")
