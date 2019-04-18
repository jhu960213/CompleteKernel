Things you should know about my OS:

OS Needed: linux.cs.mcgill.ca

1) All assignment 1,2,3 commands work as well as 4's 

2) Again, I've mentioned in previous assignments that the executable sometimes seg faults when running on linux or mimi but this doesn't happen on my MAC OS. In fact it never seg faults on my mac. 

3) For the partition command, the inputed block size and total blocks gets stored into the partition file fine but its not the number you would expect to see had you inputed both numbers above 9. for ex: if you inputed 10 -> : you will see a colon stored in the partition file. but when you mount the partition and load all those info into part and FAT structures, the printed out value of block size and total blocks would match what you inputed originally

4) Read command will return error message if the file its trying to read does not exist

5) Write command will either append to an existing file or write to a new file from the beginning

6) Mount command will need the last two fields (block size and total blocks) if its the first time you're creating a new partition, otherwise you only need Mount <partitionName> to mount an existing partition 

7) My program runs with only 1 partition but it's capable of creating many different partitions

8) Thank you for grading the assignments! It was a lot of fun coding them! :) 