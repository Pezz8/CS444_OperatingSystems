Cristian Guarino
DATE = 11/26/2023
CS444 - proj3

In my proj3 I was able to finish all the required features requested by the assignment (described in Step 3). 
Unfortunalely, I was not able to work on the Extra Credit requirements to those have not been implemented in my version of bsh.c.

I started by working on the enviroment variables in my local system and experimenting with the what happened when getting data from envp. 
Initially I was having issues since apparently I currently have more than 64 variables on my PC. 
You can see in the loading function that I tried loading a set number of variables to test out the function instead of using all the ones present on my machine. 
Eventualy I realized that I would not encounter that issue since our Unix server does not have my same situation and most likely will not run in the same issue.

That being said in order to test it I implemented my code to stop reading variables once we reached 64 in order to not go out of bounds of the array we will be 
using to store our variables locally.

I added utility functions to my code in order to help me find the variables I needed in the array. 

You will see that I have two print functions to print our variable since I was not sure if we only needed to show the ones listed in the assignment or all the 
variables on our machine. Hope this clears the confusion.

I left and commented out the test cases I used in main to test the program before I started using the cmdArg.