# __MapReduce__
This Readme gives a brief overview of the problem, the files included and instructions to run the program
Read resources/ProblemStatement.pdf for the detailed problem description.
* Author : Akash R Vasishta
* UFID   : 53955080

### __I. Problem Statement__
--------------------
Implement the Map-Reduce programming model for data processing. 
MapReduce is a framework to write applications to process huge amounts of data, in parallel, on large clusters of commodity hardware in a reliable manner. The algorithm contains two important tasks, namely Map and Reduce. Map takes a set of data and converts it into another set of data, where individual elements are broken down into tuples (key/value pairs). Secondly, reduce task, which takes the output from a map as an input and combines those data tuples into a smaller set of tuples. Combiner program makes use of fork, exec, pipe and dup2 system calls and combines the two Map-Reduce programs.

### __II. Files Included__
-----------------
* __mapperTrail2.c__ - Maps the topics from the input file with the rules and writes to the standard output as (userID,topic, score)
* __reducerPFinal1.c__ - Reduces the mapped tuples in the format (userID,topic, total) and prints on standard output
* __combinerP1.c__ - Uses various system calls like fork,exec,pipe and dup2 to implement Mapper-Reducer
* __input.txt__ - This file has a list of words and it is input to the Combiner program
* __Binaries__       - Has all the compiled binaries of the above programs

### __III. How to run the Combiner__
---------------------------

1. $ gcc mapperTrail2.c -o mapperTrail2.out
2. $ gcc reducerPFinal1.c -o reducerPFinal1.out
3. $ gcc combinerP1.c -o combinerP1.out
4. $ ./combinerP1.out ./input.txt

### __IV. Outputs:__
The Reduced tuples are displayed on the standard output.
