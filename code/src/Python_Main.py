import Queue_cpp as qu
import threading
import random
import array as arr

q=qu.Queue(qu.reliability(), qu.fixed_priority(), 3, 20)

def popper(id):
    times = 50
    while times:
        p = q.pop()
        times = times - 1

    print("POPPER DONE")

def pusher(id):
    times = 50
    while 1:
        push = random.randint(1,50)
        level = random.randint(0,2)
        q.push(push, levels)
        times = times - 1

    print("PUSHER DONE")

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
