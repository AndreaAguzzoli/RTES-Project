import Queue_cpp_double as qu
import threading
import random

q=qu.Queue(levels=2, dim=5)

def popper(id):
    times = 27
    while times:
        p = q.pop()
        times = times - 1

    print("POPPER DONE")

def pusher(id):
    times = 27
    while times:
        push = random.uniform(0.0,1.0)
        level = random.randint(0,q.getLevels()-1)
        q.push(push, level)
        times = times - 1

    print("PUSHER DONE")

i=0
x=list()
for i in range (0, qu.threads()):
    if(i%2 == 0):
        thread = threading.Thread(target=popper,args=(i,))
    else:
        thread = threading.Thread(target=pusher,args=(i,))
    x.append(thread)
    thread.start()

for thread in x:
    thread.join()
