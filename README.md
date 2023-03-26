# Smallsh
## OSU Winter2023 | CS344- Operating Systems

#### Smallsh Goals and Requirements
In this assignment you will write smallsh your own shell in C. smallsh will implement a command line interface similar to well-known shells, such as bash. Your program will

- Print an interactive input prompt
- Parse command line input into semantic tokens
- Implement parameter expansion
- Shell special parameters $$, $?, and $!
- Tilde (~) expansion
- Implement two shell built-in commands: exit and cd
- Execute non-built-in commands using the the appropriate EXEC(3) function.
- Implement redirection operators ‘<’ and ‘>’
- Implement the ‘&’ operator to run commands in the background
- Implement custom behavior for SIGINT and SIGTSTP signals


#### How to Run and Exit
Make sure that you have an environment where you can run C program. There are one of two ways to run the project.
1. With the attached the makefile.
make
./smallsh
ctrl + c to exit

2. Compile files with gcc 99

gcc -std=c99 -o smallsh smallsh.c
./smallsh
ctrl + c to exit

#### Comments
This project is very clse to complete, but it doesn't implement the custom behavior for SIGINT and SIGSTP exactly up to specifications as well as the cd custom logic is a little bit off.
#### Background 
I completed this project as part of CS344-Operating Systems offered at Oregon State University. Before starting this project we built a BASE64 Encoder and a program that mimics the TREE command in LINUX. We learned a lot about C and linux in this course and how they relate to operating systems. Before this course(3 months ago) I had no C programming experience and had just started reading a book on C to make this course easier to handle.
