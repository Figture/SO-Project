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

# === CONFIGURATION ===
INDEX_LOOKUP=1000                      # Total number of lookup requests
output_file="../scripts/cacheTime.csv"
cache_sizes=($(seq 0 10 100))          # Change this range to explore more

# === SETUP ===
accesses=($(shuf -i 1-$INDEX_LOOKUP))  # Randomize lookup keys
echo "Testing FIFO Cache Efficiency"
echo "Cache Size,Total Time" > "$output_file"

# === MAIN LOOP ===
for cache_size in "${cache_sizes[@]}"; do
    echo "Starting test with cache size: $cache_size"

    # Start the server
    ./dserver "$PATH_DATA" "$cache_size" &
    server_pid=$!
    sleep 1  # Give the server a moment to initialize

    total_time=0.0

    # Perform all lookups
    for idx in "${accesses[@]}"; do
        temp_time_file=$(mktemp)
        { /usr/bin/time -f "%e" ./dclient -c "$idx" > /dev/null; } 2> "$temp_time_file"
        elapsed=$(<"$temp_time_file")
        rm -f "$temp_time_file"

        # Validate and sum
        if [[ ! "$elapsed" =~ ^[0-9.]+$ ]]; then elapsed=0.0; fi
        total_time=$(awk "BEGIN { printf \"%.3f\", $total_time + $elapsed }")
    done

    # Record result
    echo "$cache_size,$total_time" >> "$output_file"
    echo "Cache size $cache_size took $total_time seconds"

    # Shutdown the server
    ./dclient -f
    sleep 1
done

echo "Done. Results saved to $output_file"

cd ../scripts

