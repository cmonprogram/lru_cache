# lru_cache
### Build

```
prod
$ clang++ -O3 main.cpp -o lru_cache && ./lru_cache
```
```
test
$ clang++ -O0 -Wall -g -fsanitize=fuzzer,address,undefined,leak -DFUZZING -DNOPRINT main.cpp  -o lru_cache && ./lru_cache
```
### Output
```
size:1 [new] key1:999
size:2 [new] key2:888
size:3 [new] key3:777
size:4 [new] key4:666
size:5 [new] key5:555
size:6 [new] key6:444
size:5 [deleted] key2:888
before replace 999
size:6 [new] key1:1000
size:5 [deleted] key1:999
after replace 1000
size:4 [deleted] key3:777
size:3 [deleted] key4:666
size:2 [deleted] key5:555
size:1 [deleted] key6:444
size:0 [deleted] key1:1000
```
### Difficulty
```
Get O(1) average
Set O(1) with no reorder
```
