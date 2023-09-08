import Queue_cpp as qu
import threading
import random
import array as arr

q=qu.Queue(qu.reliability(), qu.fixed_priority(), 3, 10)

def popper(id):
    print("yes")
    times = 20
    while times:
        p = q.pop()
        times = times - 1

def pusher(id):
    print("no")
    times = 20
    while times:
        push = random.random()%50
        level = random.random()%5
        q.push(push, level)
        times = times - 1
    print("no")

i=0
x=list()
for i in range (0, qu.threads()):
    if(i%2 ==0):
        thread = threading.Thread(target=popper,args=(i,))
    else:
        thread = threading.Thread(target=pusher,args=(i,))
    x.append(thread)
    thread.start()

for thread in x:
    thread.join()
