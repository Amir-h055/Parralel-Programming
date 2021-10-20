
A1
C files compatible with linux

to compile Sequential
mpicc Sequencial.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./sequencial
to run
./sequencial



paraller
mpicc Parallel.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./parallel

mpirun -np 1 ./parallel




parallel spawn child master
mpicc Parallel_spawn_master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master
mpicc Parallel_spawn_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Parallel_spawn_child

mpirun -np 1 ./Master 3





dynamic Parallel
mpicc Dynamic_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./dynamicChil
mpicc Dynamic_Master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master

mpirun -np 1 ./Master 3
