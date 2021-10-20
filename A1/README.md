
A1
</br>C files compatible with linux
</br>
</br>


</br> to compile Sequential
</br> mpicc Sequencial.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./sequencial
</br>to run
</br>./sequencial

</br>
</br>

</br>paraller
</br>mpicc Parallel.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./parallel
</br>
</br>mpirun -np 1 ./parallel

</br>
</br>


</br>parallel spawn child master
</br>mpicc Parallel_spawn_master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master
</br>mpicc Parallel_spawn_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Parallel_spawn_child
</br>
</br>mpirun -np 1 ./Master 3

</br>
</br>


</br>dynamic Parallel
</br>mpicc Dynamic_child.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./dynamicChil
</br>mpicc Dynamic_Master.c ../Monte_Carlo/Monte_Carlo_Approx_PI.c -o ./Master
</br>
</br>mpirun -np 1 ./Master 3
