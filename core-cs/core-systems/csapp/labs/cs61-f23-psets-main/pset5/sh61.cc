#include "sh61.hh"
#include <cstring>
#include <cerrno>
#include <vector>
#include <sys/stat.h>
#include <sys/wait.h>

// For the love of God
#undef exit
#define exit __DO_NOT_CALL_EXIT__READ_PROBLEM_SET_DESCRIPTION__


// struct command
//    Data structure describing a command. Add your own stuff.

struct command {
    std::vector<std::string> args;
    pid_t pid = -1;      // process ID running this command, -1 if none
    command* next = nullptr;
    int separator = 2;
    command();
    ~command();

    void run();
};


// command::command()
//    This constructor function initializes a `command` structure. You may
//    add stuff to it as you grow the command structure.

command::command() {
}


// command::~command()
//    This destructor function is called to delete a command.

command::~command() {
}




void reap_zombies() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
    }
}

// COMMAND EXECUTION

// command::run()
//    Creates a single child process running the command in `this`, and
//    sets `this->pid` to the pid of the child process.
//
//    If a child process cannot be created, this function should call
//    `_exit(EXIT_FAILURE)` (that is, `_exit(1)`) to exit the containing
//    shell or subshell. If this function returns to its caller,
//    `this->pid > 0` must always hold.
//
//    Note that this function must return to its caller *only* in the parent
//    process. The code that runs in the child process must `execvp` and/or
//    `_exit`.
//
//    PART 1: Fork a child process and run the command using `execvp`.
//       This will require creating a vector of `char*` arguments using
//       `this->args[N].c_str()`. Note that the last element of the vector
//       must be a `nullptr`.
//    PART 4: Set up a pipeline if appropriate. This may require creating a
//       new pipe (`pipe` system call), and/or replacing the child process's
//       standard input/output with parts of the pipe (`dup2` and `close`).
//       Draw pictures!
//    PART 7: Handle redirections.

void command::run() {
    assert(this->pid == -1);
    assert(this->args.size() > 0);

    pid_t p = fork();
    if(p == 0)
    {
        std::vector<char*>exec_argv;
        for(unsigned long int i = 0;i < this->args.size();++i)
        {
           exec_argv.push_back(const_cast<char*>(this->args[i].c_str()));
        }
        exec_argv.push_back(nullptr);

        if(execvp(exec_argv[0], exec_argv.data()) == -1)
            {
                perror("execvp failed");
                _exit(1);
            }  
    }
    else if(p > 0)
    {
         this->pid = p;  
    }
    else 
    {   
        perror("fork failed");
        _exit(1);
    }
}


// run_list(c)
//    Run the command *list* starting at `c`. Initially this just calls
//    `c->run()` and `waitpid`; you’ll extend it to handle command lists,
//    conditionals, and pipelines.
//
//    It is possible, and not too ugly, to handle lists, conditionals,
//    *and* pipelines entirely within `run_list`, but many students choose
//    to introduce `run_conditional` and `run_pipeline` functions that
//    are called by `run_list`. It’s up to you.
//
//    PART 1: Start the single command `c` with `c->run()`,
//        and wait for it to finish using `waitpid`.
//    The remaining parts may require that you change `struct command`
//    (e.g., to track whether a command is in the background)
//    and write code in `command::run` (or in helper functions).
//    PART 2: Introduce a loop to run a list of commands, waiting for each
//       to finish before going on to the next.
//    PART 3: Change the loop to handle conditional chains.
//    PART 4: 
//            The key insight with step 4 is that with TYPE_PIPE you fork in parallel
//                  Sequential (; && ||):    Fork → wait → fork → wait
//                  Pipeline (|):            Fork → fork → fork → wait (only last)
//                   1. Detect pipeline (count commands with | between them)
//                   2. Create N-1 pipes
//                   3. For each command in pipeline:
//                       fork()
//                       if child: connect stdin/stdout to correct pipe ends, then exec
//                       if parent: store last PID, continue
//                   4. Close all pipes in parent
//                   5. Wait for last PID
//                   6. Move to next command after pipeline
//    PART 5: Change the loop to handle background conditional chains.
//       This may require adding another call to `fork()`!
void run_list(command* c) 
{
    int prev_status = 0;
    int prev_separator = TYPE_SEQUENCE;

    while(c != nullptr)
    {
        int pipe_count = 1;
        command* temp = c;
        while(temp->separator == TYPE_PIPE)
        {
            pipe_count++;
            temp = temp->next;
        }

        if(pipe_count >= 2)
        {
            std::vector<int> pipes(2 * (pipe_count - 1));
            
            for(int i = 0; i < pipe_count - 1; i++)
            {
                int pipefd[2];
                if(pipe(pipefd) == -1)
                {
                    perror("pipe failed");
                    for(int j = 0; j < i; j++)
                    {
                        close(pipes[j]);
                    }
                    _exit(1);
                }
                pipes[2*i] = pipefd[0];
                pipes[2*i+1] = pipefd[1];
            }
            
            pid_t last_pid = 0;
            command* curr = c;
            for(int i = 0; i < pipe_count; ++i)
            {
                pid_t pid = fork();
                if(pid == 0)
                {
                    if(i > 0)
                    {
                        dup2(pipes[2*(i-1)], STDIN_FILENO);
                    }
                    if(i < pipe_count - 1)
                    {
                        dup2(pipes[2*i+1], STDOUT_FILENO);
                    }
                    for(size_t j = 0; j < pipes.size(); j++)
                    {
                        close(pipes[j]);
                    }
                    std::vector<char*> exec_argv;
                    for(unsigned long int k = 0; k < curr->args.size(); k++)
                    {
                        exec_argv.push_back(const_cast<char*>(curr->args[k].c_str()));
                    }
                    exec_argv.push_back(nullptr);
                    execvp(exec_argv[0], exec_argv.data());
                    perror("execvp failed");
                    _exit(1);
                }
                else if(pid > 0)
                {
                    if(i == pipe_count - 1)
                    {
                        last_pid = pid;
                    }
                    curr = curr->next;
                }
                else
                {
                    perror("fork failed");
                    for(size_t j = 0; j < pipes.size(); j++)
                    {
                        close(pipes[j]);
                    }
                    _exit(1);
                }
            }
            
            for(size_t i = 0; i < pipes.size(); i++)
            {
                close(pipes[i]);
            }
            
            int status;
            waitpid(last_pid, &status, 0);
            
            if(WIFEXITED(status))
            {
                prev_status = WEXITSTATUS(status);
            }
            else
            {
                prev_status = 1;
            }
            prev_separator = temp->separator;
            
            c = temp->next;
        }
        else
        {
            if(c->args.size() > 0)
            {
               if(c->separator == TYPE_BACKGROUND)
               {
                c->run();
                prev_separator = TYPE_SEQUENCE;
               }
               else
               {
                int should_run = 0;

                if(prev_separator == TYPE_SEQUENCE)
                {
                    should_run = 1;
                }
                else if(prev_separator == TYPE_AND && prev_status == 0)
                {
                    should_run = 1;
                }
                else if(prev_separator == TYPE_OR && prev_status != 0)
                {
                    should_run = 1;
                }
                
                if(should_run)
                {
                    c->run();
                    int status;
                    waitpid(c->pid, &status, 0);
                    if(WIFEXITED(status))
                    {
                        prev_status = WEXITSTATUS(status);
                    }
                    else
                    {
                        prev_status = 1;
                    }
                }
                prev_separator = c->separator;
               }
            }
            c = c->next;
        }
    }
}


// parse_line(s)
//    Parse the command list in `s` and return it. Returns `nullptr` if
//    `s` is empty (only spaces). You’ll extend it to handle more token
//    types.

command* parse_line(const char* s) {
    shell_parser parser(s);
    command* first = nullptr;
    // Your code here!

    // Build the command
    // The handout code treats every token as a normal command word.
    // You'll add code to handle operators.
    command* current = nullptr;
    for (shell_token_iterator it = parser.begin(); it != parser.end(); ++it) {
        if(it.type() == TYPE_NORMAL)
        {
            if (!current) 
            {
            current = new command; 
            first = current;
            }
        current->args.push_back(it.str());
        }
        else if(it.type() == TYPE_BACKGROUND)
        {
            current->separator = TYPE_BACKGROUND;
            current->next = new command;
            current = current->next;
        }
        else if(it.type() == TYPE_PIPE)
        {
            current->separator = TYPE_PIPE;
            current->next = new command;
            current = current->next;
        }
        else if(it.type() == TYPE_AND)
        {
            current->separator = TYPE_AND;
            current->next = new command;
            current = current->next;    
        }
        else if(it.type() == TYPE_OR)
        {
            current->separator = TYPE_OR;
            current->next = new command;
            current = current->next;
        }
        else if(it.type() == TYPE_SEQUENCE)
        {
            current->separator = TYPE_SEQUENCE;
            current->next = new command;
            current = current->next;
        }
    }
    return first;
}


int main(int argc, char* argv[]) {
    FILE* command_file = stdin;
    bool quiet = false;

    // Check for `-q` option: be quiet (print no prompts)
    if (argc > 1 && strcmp(argv[1], "-q") == 0) {
        quiet = true;
        --argc, ++argv;
    }

    // Check for filename option: read commands from file
    if (argc > 1) {
        command_file = fopen(argv[1], "rb");
        if (!command_file) {
            perror(argv[1]);
            return 1;
        }
    }

    // - Put the shell into the foreground
    // - Ignore the SIGTTOU signal, which is sent when the shell is put back
    //   into the foreground
    claim_foreground(0);
    set_signal_handler(SIGTTOU, SIG_IGN);

    char buf[BUFSIZ];
    int bufpos = 0;
    bool needprompt = true;

    while (!feof(command_file)) {
        // Print the prompt at the beginning of the line
        if (needprompt && !quiet) {
            reap_zombies();
            printf("sh61[%d]$ ", getpid());
            fflush(stdout);
            needprompt = false;
        }

        // Read a string, checking for error or EOF
        if (fgets(&buf[bufpos], BUFSIZ - bufpos, command_file) == nullptr) {
            if (ferror(command_file) && errno == EINTR) {
                // ignore EINTR errors
                clearerr(command_file);
                buf[bufpos] = 0;
            } else {
                if (ferror(command_file)) {
                    perror("sh61");
                }
                break;
            }
        }

        // If a complete command line has been provided, run it
        bufpos = strlen(buf);
        if (bufpos == BUFSIZ - 1 || (bufpos > 0 && buf[bufpos - 1] == '\n')) {
            if (command* c = parse_line(buf)) {
                run_list(c);
                reap_zombies();
                delete c;
            }
            bufpos = 0;
            needprompt = 1;
        }

        // Handle zombie processes and/or interrupt requests
        // Your code here!
    }

    return 0;
}
