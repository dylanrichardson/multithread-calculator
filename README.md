This program evaluates equations from a configuration file using parallel processing. The configuration file consists of nodes. A node has an id, expression, time delay, and node dependencies. The program outputs the calculated value of each node and the sum of each nodes' value. There are two versions of the program. The first called *graph* uses semaphores and the second abstracts the specific functionality of the semaphores found in graph into a structure called an *nblock*. This was built for WPI CS 3013 Operating Systems.

#### Examples

```
$ cat config/2.txt
A 1 1
B 2 1 A
C 3 1 A
D 4 1 B C

$ graph/graph config/2.txt
Node A computed a value of 1 after 1 second.
Node B computed a value of 2 after 2 seconds.
Node C computed a value of 3 after 2 seconds.
Node D computed a value of 4 after 3 seconds.
Total computation resulted in a value of 10 after 3 seconds.
```

```
$ cat config/3.txt
A 1 1
B 0 1 A = 23 2 4 * +
C 0 1 A = V 2 %
D 0 1 B C = I V +

$ graph/graph config/3.txt
Node A computed a value of 1 after 1 second.
Node B computed a value of 31 after 2 seconds.
Node C computed a value of 0 after 2 seconds.
Node D computed a value of 35 after 3 seconds.
Total computation resulted in a value of 67 after 3 seconds.
```
