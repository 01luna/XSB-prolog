import time

def makelist(N):
    start = time.time()
    retlist = []
    for i in range(1,N):
        retlist.append(i)
    end = time.time()
    print("python_done: " + str(end-start) + " seconds")
    return retlist

class StressClass:
    def __init__(self):
        self.list = []

    def func0(self,N):
        start = time.time()
        for i in range(1,N):
            self.list.append(i)
        end = time.time()
        print("python_done: " + str(end-start) + " seconds")
        return self.list

