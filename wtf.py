import random

random.seed(42)

N = 4
for i in range(50000):
  a1 = random.randrange(N)
  a2 = random.randrange(N-1)
  if a2 >= a1:
    a2 += 1
  print("{}".format(a2))
  #print("{}".format(sorted([a1, a2])))
