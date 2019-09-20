
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
os.system("git pull")



def load(filename):
  with open(filename) as f:
    q=[]
    for i in range(1000000000):
      ac=f.readline()[:-1]
      if len(ac)==0:break
      q.append(float(ac))


  q=np.array(q)
  return q
  
  
  
# q=np.zeros(40000)

allt=[]


folder="danode2/09"
if len(sys.argv)>1:
  folder=sys.argv[1]
n=100
if len(sys.argv)>2:
  n=int(sys.argv[2])
histmode=False
if len(sys.argv)>3:
  histmode=(sys.argv[3]=="1")
for i in range(0,n):
  # ac=load("test8/file"+str(i)+".txt")
  ac=load(folder+"/file"+str(i)+".txt")
  # ac=load(folder+"/file1_"+str(i)+".txt")
  # ac-=np.mean(ac)
  # ac/=np.std(ac)
  # for e in range(len(ac)):
    # if ac[e]<-10:
      # allt.append(e*0.002)
  if histmode:allt.append(np.argmin(ac)*0.002*2)
  if not histmode:plt.plot(np.arange(len(ac))*0.002,ac,"o",alpha=0.01)
  print("did",i)

# plt.plot(np.arange(len(q))*0.002,q,"o")

if histmode:plt.hist(allt,bins=50)
plt.xlabel("mus")

plt.show()  
