# A 1
C files compatible with linux, and both parallel and sequencial executed locally therefore this report(performance) is based on that

### Table of Contents
1. [Contributors](#Contributor)
2. [Guide](#Guide)
3. [Question a](#Question-a)
4. [Question b](#Question-b)
5. [Question c](#Question-c)
6. [Question d](#Question-d)
</br>
</br>

## Contributor

@Amir-h055

</br>

##  Guide

[Monte_Carlo](Monte_Carlo)
</br>
</br>
[Sequential](Sequential)
</br> to compile Sequential
</br>```bash mpicc Sequencial.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./sequencial```
</br>to run
</br>```bash ./sequencial```

</br>
</br>

</br>paraller
[Parallel](Parallel)
</br>```bash mpicc Parallel.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./parallel ```
</br>
</br>```bash mpirun -np 1 ./parallel ```

</br>
</br>

[Parrallel_Master_Children](Parrallel_Master_Children)
</br>parallel spawn child master
</br>```bash mpicc Parallel_spawn_master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master ```
</br>```bash mpicc Parallel_spawn_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Parallel_spawn_child ```
</br>
</br>```mpirun -np 1 ./Master 3 ```

</br>
</br>

[Parallel_Dynamic](Parallel_Dynamic)
</br>dynamic Parallel
</br>```bash mpicc Dynamic_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./dynamicChil ```
</br>```bash mpicc Dynamic_Master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master ```
</br>
</br>```bash mpirun -np 1 ./Master 3 ```




## Question a
[Sequential](/Sequential)

 .| **time** | 
:---: | ---
 .| 0.205251


## Question b
- normal Paraller

**number of workers** | **time**
:---: | ---
2 | 0.233696
4 | 0.288562
6 | 0.307980
8 | 0.480024


- in Parallel_Master_Children

**number of workers** | **time**
:---: | ---
2 | 0.131800
4 | 0.105447
6 | 0.116287
8 | 0.393699

## Question c
Granularity is dedined as the size and number of task we are deviding our problem into.
But the granularity is basically the `Computation time over the comunation time` so when we increase the workers it would be mainly comunication effect that determins granularity and when we have less workers it is the computation

## Question d

**number of workers** | **time**
:---: | ---
2 | 0.151970
4 | 0.040547
6 | 0.059450
8 | 0.329488

## Question e
Speed up is sequential time over paraller time
parrale usually have good speed up if we are doing a task that can be done independent from one another . in this case we didnt get good result most likely becase the problem is not still that huge or becase of the metods being used for parralel version. So in this case the communication actually is more and as the graph shows after using more than 6 workers the overhead is considerably huge

```python

import numpy as np
import matplotlib.pyplot as plt
import sklearn as sk

num_workers = [2, 4, 6, 8]
seq_time = 0.205251
parallel_master_children_time = np.array([0.131800 , 0.105447 ,0.116287 ,0.393699])
normal_parallel_time = np.array( [0.233696, 0.288562, 0.307980, 0.480024])
dynamic_parallel_time = np.array( [0.151970, 0.040547, 0.059450, 0.329488])

x = parallel_master_children_time/seq_time
y = num_workers
plt.plot()
plt.ylabel('Speedup')
plt.xlabel('Number of workers')
plt.plot(y,x, label="parent-ch")
plt.plot(y,(normal_parallel_time/seq_time), label="parral")
plt.plot(y,(dynamic_parallel_time/seq_time), label="dynamic")
plt.plot(y,(seq_time*np.ones(4)), label="seq")
plt.legend()
plt.suptitle("Speedup and workers")
plt.savefig("Graph.png", format="jpg")
plt.show()


```



**Thank you**



