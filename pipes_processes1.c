// C program to demonstrate a two-way pipe between parent and child process
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org"; 
    char input_str1[100], input_str2[100]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of first pipe
        close(fd2[1]);  // Close writing end of second pipe
  
        printf("P1: Enter a string to concatenate with 'howard.edu': ");
        scanf("%s", input_str1);
  
        // Write input string to child (P2) through first pipe
        write(fd1[1], input_str1, strlen(input_str1)+1);
        close(fd1[1]); // Close writing end of first pipe
  
        // Wait for child process to finish
        wait(NULL);
  
        // Read string from child (P2) via second pipe
        read(fd2[0], input_str2, 100);
  
        // Concatenate fixed string with the received input from child
        int k = strlen(input_str2); 
        int i; 
        for (i=0; i<strlen(fixed_str2); i++) 
            input_str2[k++] = fixed_str2[i]; 
  
        input_str2[k] = '\0';   // Null-terminate the string
  
        printf("P1: Final concatenated string: %s\n", input_str2);
  
        close(fd2[0]); // Close reading end of second pipe
    } 
  
    // Child process (P2)
    else
    { 
        close(fd1[1]);  // Close writing end of first pipe
        close(fd2[0]);  // Close reading end of second pipe
  
        // Read the string from parent (P1) via first pipe
        char concat_str1[100]; 
        read(fd1[0], concat_str1, 100);
  
        // Concatenate "howard.edu" to the string
        int k = strlen(concat_str1); 
        int i; 
        for (i=0; i<strlen(fixed_str1); i++) 
            concat_str1[k++] = fixed_str1[i]; 
  
        concat_str1[k] = '\0';   // Null-terminate the string
  
        printf("P2: Concatenated string: %s\n", concat_str1);
        close(fd1[0]); // Close reading end of first pipe
  
        // Prompt the user for a new input string in P2
        printf("P2: Enter another string to concatenate with 'gobison.org': ");
        scanf("%s", input_str2);
  
        // Write the new input string back to parent (P1) via second pipe
        write(fd2[1], input_str2, strlen(input_str2)+1);
        close(fd2[1]); // Close writing end of second pipe
  
        exit(0);
    } 
} 