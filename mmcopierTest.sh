#!/bin/bash
make -s clean
make -s mmcopier

# Create dest dir if it does not already exist
mkdir -p dest_dir


# Run mm copier for 2 <= n <= 10
# Check if files have been copied from source_dir to dest_dir and file contents are same 
echo "Testing basic operation"
echo "......................."
echo ""

for i in {2..10}; do 
    echo "Testing for n value of $i";

    # run command
    ./mmcopier $i source_dir dest_dir

    # ensure files have been moved
    cd dest_dir
    for (( j = 1 ; j <= $i ; j++ ))
    do 

        if ! cmp -s source$j.txt ../source_dir/source$j.txt; then 
            echo "Test failed";
            # clean up if test failed
            make -s clean
            rm -r dest_dir
            exit;
        fi
    done

    # clean up before moving onto next n value
    rm *;  # removing all files in dest dir
    cd ../; # going back to main directory
done


echo "Basic operation is functional" 
echo ""
echo "Testing for memory leaks"
echo "......................."
echo ""

rm -r dest_dir
mkdir dest_dir

#valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./mmcopier 10 source_dir/ dest_dir/
for i in {2..10}; do 
    echo "Testing for n value of $i";
    output=$(valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./mmcopier $i source_dir/ dest_dir/ 2>&1)


     if ! echo "$output" | grep -q "All heap blocks were freed -- no leaks are possible"; then
        echo "Memory leak found for n=$i"
        # clean up if test failed
        make -s clean
        rm -r dest_dir
        exit 
    fi
done

echo "No memory leaks present in mmcopier"

# Todo: add thread safety tests
# valgrind --tool=helgrind ./mmcopier 10 source_dir/ dest_dir/

make -s clean
rm -r dest_dir


