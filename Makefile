
ifndef NP
NP = 4
endif

ifndef S
S = 1000
endif

sequential:
	gcc -O3 sequential.c common.c -o main -lm

parallel1:
	smpicc -O4 1D_parallel_julia.c common.c -o 1D_parallel -lm

parallel2:
	smpicc -O4 2D_parallel_julia.c common.c -o 2D_parallel -lm

run1:
	smpirun -np $(NP) -hostfile ./hostfile -platform ./cluster_crossbar.xml --cfg=smpi/host-speed:1000000000 ./1D_parallel $(S)

run2:
	smpirun -np $(NP) -hostfile ./hostfile -platform ./cluster_crossbar.xml --cfg=smpi/host-speed:1000000000 ./2D_parallel $(S)
