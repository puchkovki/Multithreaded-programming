# Realisation

List's node has two fields: `data` and atomic pointer to the next element `next`.

List always next fields:
 - fake head node `head_`, which refers to the first informative element
 - last informative element `last_`
 - fake tail node `tail_`, to which refers `last_`
 - number of the concurrent threads `n_threads_`
 - size of the local hazard pointers `n_hazard_ptr_`
 - map of hazard nodes for each proccess `hazard_ptrs_`
 - map of nodes want to delete `retired_nodes_`
 - mutex to make maps protected`threadMutex`

Stack is implemented with two-way adding methods `push_front()` and `push_back()`
and front deleting `pop_front()`. Due to the `Iterator` definition we could make
range based loop for our list or output it with `output` function.

To check [ABA problem](https://en.wikipedia.org/wiki/ABA_problem) was implemented
swap methods with sleep and yield realisations `swapSleep` and `swapYield` respectively
```
swap()
pop() // Delete A
pop() // Delete B
push(A) // Add A
```

Deleting is implemented with [Hazard pointers](https://habr.com/ru/post/202190/).
These approach required methods for retiring nodes `retire_` and then scanning all 
hazard pointers' arrays for save removal.

The moment our nodes could become "hazard" occurs while swap operation, so we mark
nodes as HP by `addToHp` method and then make them normal again by `deleteFromHp` method.




# Compile

To
make --always-make -s ARGS="$1" 