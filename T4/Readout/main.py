
import numpy as np
import matplotlib.pyplot as plt



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


for i in range(0,100):
  # ac=load("test8/file"+str(i)+".txt")
  ac=load("danode/16/file"+str(i)+".txt")
  # ac-=np.mean(ac)
  # ac/=np.std(ac)
  # for e in range(len(ac)):
    # if ac[e]<-10:
      # allt.append(e*0.002)
  allt.append(np.argmin(ac)*0.002*2)
  plt.plot(np.arange(len(ac))*0.002,ac,"o",alpha=0.01)
  print("did",i)

# plt.plot(np.arange(len(q))*0.002,q,"o")

# plt.hist(allt,bins=50)
plt.xlabel("mus")

plt.show()  
