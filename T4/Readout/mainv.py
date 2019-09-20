
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
histmode=True
if len(sys.argv)>3:
  histmode=(sys.argv[3]=="1")

a=[]
labels=np.arange(0,400,99)
for l in labels:
  b=[]
  for i in range(0,n):
    # ac=load("test8/file"+str(i)+".txt")
    # ac=load(folder+"/file"+str(i)+".txt")
    ac=load(folder+"/file"+str(l)+"_"+str(i)+".txt")
    # ac-=np.mean(ac)
    # ac/=np.std(ac)
    # for e in range(len(ac)):
      # if ac[e]<-10:
        # allt.append(e*0.002)
    if histmode:b.append(np.argmin(ac)*0.002*2)
    print("did",i)
  a.append(b)


# plt.plot(np.arange(len(q))*0.002,q,"o")

for i in range(len(labels)):
  if histmode:plt.hist(a[i],bins=50,alpha=0.2,label=labels[i])
plt.xlabel("mus")

plt.legend()
plt.show()  
