#!/bin/bash

cd ../bin

NUMBER_OF_INDEXS=1000 

###################
#                 #
#   SEARCH TEST   #
#                 #
###################

CACHE_SIZE=30
PATH_DATA=/home/atavic/Documents/SO/Projeto-SO/Gdataset

./dserver $PATH_DATA $CACHE_SIZE & 

echo "Indexing $NUMBER_OF_INDEXS documents"
time {
    for i in $(seq 1 $((NUMBER_OF_INDEXS + 1))); do
        ./dclient -a "$i" "Author $i" 2025 "$i.txt"
    done
}

search_times=()
num_procs=()

output_file="../scripts/procTime.csv"
echo "Processes,Time" > "$output_file"

KEYWORD=geo
for i in $(seq 1 1 50); do
    echo "Searching word $KEYWORD with $i processes"
    real_time=$( (/usr/bin/time -f "%e"  ./dclient -s "$KEYWORD" $i > /dev/null) 2>&1 )
    search_times+=("$real_time")
    num_procs+=($i)
    echo "$i,$real_time" >> "$output_file"
done

sleep 1
./dclient -f

#######################
#                     #
#   CACHE SIZE TEST   #
#                     #
#######################

INDEX_LOOKUP=1000
cache_sizes=($(seq 1 20 101))


rlookup_times=()
rcache=()
accesses=($(shuf -i 1-$INDEX_LOOKUP))

echo "Random look up"
output_file="../scripts/cacheTime.csv"
echo "Cache size,Time" > "$output_file"

for i in "${cache_sizes[@]}"; do
    ./dserver "$PATH_DATA" "$i" &

    # Convert the accesses array into a space-separated string
    access_str="${accesses[*]}"

    # Use `bash -c` to expand the loop and pass access_str
    real_time=$( /usr/bin/time -f "%e" bash -c '
      for j in '"$access_str"'; do
        ./dclient -c "$j" > /dev/null
      done
    ' 2>&1 )

    rlookup_times+=("$real_time")
    rcache+=($i)

    echo "$i,$real_time" >> "$output_file"

    sleep 1
    ./dclient -f
done

cd ../scripts

echo "SEARCH KEYWORD TEST"
echo "PROCS       TIME"
echo "----------------"

for i in "${!num_procs[@]}"; do
    echo "${num_procs[$i]}       ${search_times[$i]}"
done

echo "CACHE SIZE TEST / Random"
echo "CACHE       TIMES"
echo "----------------"

for i in "${!rcache[@]}"; do
    echo "${rcache[$i]}       ${rlookup_times[$i]}"
done



