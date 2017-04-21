# Assignment2 CS530 using C++

## What currently works/TODO...
+ We need the parseFlags method to return an array that represents XPBE so we can determine if the instruction is Format 4
+ The Text Record while loop is basically set up. We now need to touch it up and write the methods for Format 3 and Format 4
+ Don't really worry about the software design portion, I can do that fairly quickly

## Format Three
1. Parse the Opcode for the instruction (Check for -1, the #. If not this then do -3 for the opcode)
2. Do the necessary subtraction for the relative address and then check the symbol hashmap for the correct value
3. Form the source code as a string (See the header record method for an example on that)

## Format Four
+ Format Four should go the same as the format 3 method, but instead of doing any subtraction just do a direct lookup in the symtab hasmap
+ Don't forget to add a "+" when formatting the source code string.



## Getting Started
+ First you'll need to pull to repo, do `git clone http_link_in_repo folder_to_save_repo`
+ An example of the above would be `git clone https://github.com/username/assignment assignment2`

## Compling The C++ Code
There are many different C++ compilers, to make sure we are all using the same one use this line to complie your code
+ `g++ -std=c++11 file_name.cpp -o output_file`

## Branches And Github Tips
+ Everyone should work on their own branch and towards the due date we can merge everything together
+ To create your own branch to work on using the terminal do `git checkout -b branch_name`
+ To see what branch you are working on do `git branch`

## Updating Your Branch
Follow the steps below to update your branch
+ `git add .` (The "." indicates to add everything in the folder to the repo)
+ `git commit -m "message goes here"` (This will add a message of what was changed. Make this a little descriptive so everyone knows what's going on)
+ `git push origin branch_name` (Pushes changes to your branch)
+ `git status` (This command will let you know if your branch is up to date or not)
