/*

Name: Muhammad Niazi
Project 1


Compile command: gcc message.c -o message.exe
Run command: ./message.exe [Buffer Size] [aurguments] [input file]
Run command with no aurguments: ./message.exe [Buffer Size] [input file]
Run command with no aurguments and no input file: ./message.exe [Buffer Size]

*/


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>

// Initialize the variables for different parameters
int ARGS_l = 0;
int ARGS_w = 0;
int ARGS_m = 0;
int ARGS_L = 0;
int BUFFER_SIZE = 0;
int FILE_COUNTER = 0;
char **FILES;

// function to print number of lines
void print_num(int n)
{
    if (n == 0)
        return;
    print_num(n / 10);
    write(1, &"0123456789"[n % 10], 1);
}

// function to print string in file
void print_string(char *s)
{
    while (*s)
        write(1, s++, 1);
}

// check string length
int str_len(char *s)
{
    int length = 0;
    while (s[length++] != '\0');
    return length;
}


// main funtion that checks for buffer size and aurguments 
void parse_arguments(int argc, char *argv[])
{
    // check for valid arguments
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            // check for which aurugment is passed
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                ARGS_l = argv[i][j] == 'l' ? 1 : ARGS_l;
                ARGS_w = argv[i][j] == 'w' ? 1 : ARGS_w;
                ARGS_m = argv[i][j] == 'm' ? 1 : ARGS_m;
                ARGS_L = argv[i][j] == 'L' ? 1 : ARGS_L;
            }
        }
        
        // check for buffer size
        else if (BUFFER_SIZE == 0)
        {
            BUFFER_SIZE = atoi(argv[i]);
        }

        // check if buffer size is valid
        else if (BUFFER_SIZE < 128 || BUFFER_SIZE > 256)
        {
            write(2, "Invalid buffer size. Please enter a buffer size between 128 and 256.\n", 70);
            exit(1);
        }
      

        else 
        {
            // check for valid file
            FILE_COUNTER++;
            FILES = (char**)realloc(FILES, FILE_COUNTER * sizeof(char*));
            int length = 0;
            while (argv[i][length] != '\0')
            {
                length++;
            }
            // check for valid end of file depending on buffer size
            FILES[FILE_COUNTER - 1] = (char*)malloc(length * sizeof(char));
            FILES[FILE_COUNTER - 1] = argv[i];
        }
        
    }
    // check for valid aurguments entered
    if (ARGS_l == 0 && ARGS_w == 0 && ARGS_m == 0 && ARGS_L == 0)
    {
        ARGS_l = 1;
        ARGS_w = 1;
        ARGS_m = 1;
    }
    if (FILE_COUNTER == 0)
    {
        // allocate memory for file
        FILE_COUNTER++;
        FILES = (char **)malloc(sizeof(char *));

        // if no file is entered default file is entered named "myprj1inp.txt"
        FILES[0] = "myprj1inp.txt\0";
    }
}



// print os info
void os_info()
{   
    struct utsname uts;
    uname(&uts);
    write(1, "OS name: ", 10);
    write(1, uts.sysname, str_len(uts.sysname));
    write(1, "\n", 1);
    write(1, "OS release: ", 13);
    write(1, uts.release, str_len(uts.release));
    write(1, "\n", 1);
    write(1, "OS version: ", 13);
    write(1, uts.version, str_len(uts.version));
    write(1, "\n", 1);
}

int main(int argc, char*argv[])
{


    os_info(); // print os info
    parse_arguments(argc, argv);    // parse aurguments

    // open the file
    int pipe1[2];
    int pipe2[2];

/* //sample code for pipe from lecture
    int n, fd[2]

    pid_t pid;
    
    char line[MAXLINE];
// The pipe is created and its file descriptors returned.
        if ( pipe(fd) < 0) {
            fprintf(stderr, "Pipe failed”);
            return 1;
        }
// Parent is producer and child is consumer.
        pid = fork();
            if (pid < 0) {
                    fprintf(stderr, "Fork Failed");
                    return 2;
                }   
        else if (pid == 0) { // Child reads from pipe
        close (fd[1]); // write end of pipe
        n = read (fd[0], line, MAXLINE);
        write (STDOUT_FILENO, line, n);
        }
        else { // Parent writes to pipe
        close (fd[0]); // read end of pipe
        write (fd[1], “hello world\n”, 12);
        }
        return 0;
        } 
*/

    // create pipe
    if (pipe(pipe1) == -1)
    {
        write(2, "Error creating pipe 1", 22);
        exit(1);
    }
    if (pipe(pipe2) == -1)
    {
        write(2, "Error creating pipe 2", 22);
        exit(1);
    }

    // create child process
    
    //pid_t pid = fork();


    pid_t pid;

    pid = fork();
    
    char *buffer = (char*)malloc(BUFFER_SIZE * sizeof(char)); // buffer for reading file

    if (pid == 0)
    {
        // read from pipe 
        int bytes = read(pipe1[0], buffer, BUFFER_SIZE);

        write(1, "Child info: ", 13);
        write(1, "\n", 1);
        write(1, "Process ID: ", 13);
        print_num(getpid());
        write(1, "\n", 1);


        write(1, "Parent ID: ", 12);
        print_num(getppid());
        write(1, "\n", 1);



        write(1, "Process current working directory: ", 36);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        write(1, cwd, str_len(cwd));
        write(1, "\n", 1);


        write(1, "Hostname: ", 11);
        char hostname[1024];
        gethostname(hostname, sizeof(hostname));
        write(1, hostname, str_len(hostname));
        write(1, "\n", 1);

        close(pipe1[1]);
        close(pipe2[0]);

        // write to pipe 
        int lines = 0, words = 0, characters = 0, longest = 0, temp_line = 0;
        while (bytes != 0 && bytes != -1)
        {
            // check for end of buffer and end of file
            for (int i = 0; i < bytes; i++)
            {
                if (buffer[i] == '\n')
                {
                    lines++;
                    if (temp_line > longest)
                        longest = temp_line;
                    temp_line = 0;
                } else if (buffer[i] == ' ') {
                    words++;
                } else if (buffer[i] == -1) {
                    break;
                } else {
                    characters++;
                    temp_line++;
                }
            }

            // read from pipe and print to console 
            write(pipe2[1], &lines, sizeof(int));
            write(pipe2[1], &words, sizeof(int));
            write(pipe2[1], &characters, sizeof(int));
            write(pipe2[1], &longest, sizeof(int));
            bytes = read(pipe1[0], buffer, BUFFER_SIZE);
        }

        close(pipe1[0]);
        close(pipe2[1]);
    }
    else
    {
        // write info about parent process
        // pid of process, pid of parent process, current working directory, hostname of process
        write(1, "Parent info: ", 14);
        write(1, "\n", 1);
        write(1, "Process ID: ", 13);
        print_num(getpid());
        write(1, "\n", 1);


        write(1, "Parent process ID: ", 20);
        print_num(getppid());
        write(1, "\n", 1);


        write(1, "Current working directory: ", 28);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        write(1, cwd, str_len(cwd));
        write(1, "\n", 1);


        write(1, "Hostname: ", 11);
        char hostname[1024];
        gethostname(hostname, sizeof(hostname));
        write(1, hostname, str_len(hostname));
        write(1, "\n", 1);

        close(pipe1[0]);
        close(pipe2[1]);

        // open file and write to pipe
        for (int i = 0; i < FILE_COUNTER; i++)
        {
            int file_descriptor = open(FILES[i], O_RDONLY);

            int bytes = read(file_descriptor, buffer, BUFFER_SIZE);
            while (bytes != 0 && bytes != -1)
            {
                write(pipe1[1], buffer, bytes);
                bytes = read(file_descriptor, buffer, BUFFER_SIZE);
            }
            char end_of_file = -1;
            write(pipe1[1], &end_of_file, 1);

            close(file_descriptor);
        }
        close(pipe1[1]);

        /* //sample code for file copy from lecture
        int f1, f2;
        if (argc != 3){
        printf("Wrong number of command line arguments\n");
        // exit(1);
        return 1;
        }
        // Can we access the source file?
        
        if ((f1 = open(argv[1], O_RDONLY, 0)) == -1){
        printf("Can't open %s \n", argv[1]);
        return 2;
        }
        
        if ((f2 = creat(argv[2], 0644)) == -1){
        printf("Can't create %s \n", argv[2]);
        return 3;
        }
        // Copy source file contents to target file.
        char buf[SIZ];
        int n;
        while ((n = read( f1, buf, SIZ)) > 0)
        if (write(f2, buf, n) != n)
        {
        printf("Can't write file" );
        close(f1);
        close(f2);
        return 4;
        }
        close(f1);
        close(f2);
        printf("Success!" );
        return 0;
        } 
        */
        int count;

        // read from pipe and print to console
        for (int i = 0; i < FILE_COUNTER; i++)
        {
            read(pipe2[0], &count, sizeof(int));
            if (ARGS_l)
            {
                // print newline count
                write(1, FILES[i], str_len(FILES[i]));
                write(1, ":\t newline count is: ", 22);
                print_num(count);
                write(1, "\n", 1);
            }
            read(pipe2[0], &count, sizeof(int));
            if (ARGS_w)
            {
                // print word count
                write(1, FILES[i], str_len(FILES[i]));
                write(1, ":\t word count is: ", 19);
                print_num(count);
                write(1, "\n", 1);
            }
            read(pipe2[0], &count, sizeof(int));
            if (ARGS_m)
            {
                // print character count
                write(1, FILES[i], str_len(FILES[i]));
                write(1, ":\t character count is: ", 24);
                print_num(count);
                write(1, "\n", 1);
            }
            read(pipe2[0], &count, sizeof(int));
            if (ARGS_L)
            {
                // print longest line
                write(1, FILES[i], str_len(FILES[i]));
                write(1, ":\t longest line length is: ", 28);
                print_num(count);
                write(1, "\n", 1);
            }
        }

        close(pipe2[0]); // end of pipe
    }

    return 0;
} 
