#!/bin/bash
make -s clean;
make -s mscopier;


# generate the source file to be copied
touch source.txt;
./generate_text.sh 1000 > source.txt;




# Run ms copier for 2 <= n <= 10
# Check if  file contents are same 
echo "Testing basic operation"
echo "......................."
echo ""

for i in {2..10}; do 
    echo "Testing for n value of $i";
    # Generate the destination file
    touch dest.txt; 

    # run command
    ./mscopier $i source.txt dest.txt
    
    
    if ! cmp -s source.txt dest.txt; then 
        echo "Test failed";
        # clean up if test failed
        make -s clean
        rm -r dest.txt
        rm -r source.txt
        exit;
    fi

    rm dest.txt;
done


echo "Basic operation is functional" 
echo ""
echo "Testing for memory leaks"
echo "......................."
echo ""


#valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./mscopier 10 source.txt dest.txt
for i in {2..10}; do 
    echo "Testing for n value of $i";
    touch dest.txt; 

    output=$(valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./mscopier $i source.txt dest.txt 2>&1)

     if ! echo "$output" | grep -q "All heap blocks were freed -- no leaks are possible"; then
        echo "Memory leak found for n=$i"
        # clean up if test failed
        make -s clean
        rm -r dest.txt
        rm -r source.txt
        exit 
    fi

    rm dest.txt;
done

echo "No memory leaks present in mscopier"
echo ""


make -s clean
rm source.txt