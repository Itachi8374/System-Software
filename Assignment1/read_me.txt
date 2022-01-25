Name: Tanay Maheshwari
Roll No: 190101092

Information/Assumptions for the given assignment:
1. Field length of each column (except operand) is assumed to be of fixed length(10), therefore input file should be formatted accordingly to get correct result.
2. Proper formatting instructions are as follows:
    a. Each field (except operand) should be of 10 length.
    b. No extra space or tab should be present after operand field.
    c. Tabs should not be used to separate fields. If any field is less than 10 then spaces should be used to fill up the rest of column.
    d. No extra line should be present after END directive.
3. Comments must have "." symbol in label field. 
4. If there is an error in first pass the program will terminate printing the error on terminal. If there is an error in second pass, the error will be printed but the program will continue execution.
5. If the program name exceeds 6 characters then only the first 6 characters will be taken.
6. There are no spaces before or after "," while using indirect addressing.
7. While using X'' format of BYTE directive, if the hexadecimal constant length is odd then one more byte will be reserved to make it even.
8. Unicode character code "\u2038" is used to print caret. This unicode work is tested to work properly on Mac and Linux.
9. Instructions to run the program:
    a. Make sure to have "input.txt" file in the same folder as that of 190101092_Assignment01.cpp
    b. Three files, intermediate.txt, listing.txt and object_code.o will be generated
    c. Sample input file with source code taken from book pg 49 is present in example folder. Output files generated on running the input file on my system are also attached. To test the program on sample input simply copy paste the "input.txt" file from example folder to same folder as that of 190101092_Assignment01.cpp
10. Run the following commands in order:
    a. g++ 190101092_Assignment01.cpp
    b. ./a.out