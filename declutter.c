#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>

#define HELP_OPTION_VERBOSE \
  "      -v, --verbose        explain what is being done\n"
#define HELP_OPTION_DESCRIPTION \
  "      --help        display this help and exit\n"
#define VERSION_OPTION_DESCRIPTION \
  "      --version     output version information and exit\n"

#define PROGRAM_NAME "declutter"
#define VERSION "1.0"
#define AUTHORS "Faber"

int verbose = 0;

void print_help()
{
    printf("Usage: %s [OPTION]\n", PROGRAM_NAME);
    printf("Organize files in the current directory into subfolders based on extension.\n\n");

    printf("Rules:\n");
    printf("      Images/    <- .jpg, .png, .jpeg, .svg\n");
    printf("      Documents/ <- .pdf, .docx, .txt, .md\n");
    printf("      Archives/  <- .zip, .tar, .gz, .rar\n");
    printf("      Code/      <- .cpp, .c, .h, .py\n\n");

    printf("Options:\n");
    printf(" %s", HELP_OPTION_VERBOSE);
    printf("%s", HELP_OPTION_DESCRIPTION);
    printf("%s", VERSION_OPTION_DESCRIPTION);
}

void print_version()
{
    printf("%s %s\n", PROGRAM_NAME, VERSION);
    printf("Written by %s.\n", AUTHORS);
}

int has_extension(const char *filename, const char *ext)
{
    const char* dot = strrchr(filename, '.');
    if(!dot || dot == filename)
    {
        return 0;
    }
    return (strcasecmp(dot + 1, ext) == 0);
}

void move_file(const char *filename, const char *target_folder)
{
    mkdir(target_folder, 0755);

    unsigned long len = strlen(target_folder) + 1 + strlen(filename) + 1;
    char *target_path = malloc(len);
    if(!target_path)
    {
        perror("Memory error");
        return;
    }

    sprintf(target_path, "%s/%s", target_folder, filename);

    if(rename(filename, target_path) == 0)
    {
        if(verbose)
        {
            printf("Moved: %s -> %s\n", filename, target_folder);
        }
    }
    else
    {
        perror("Error moving file");
    }

    free(target_path);
}

int main(int argc, char **argv)
{
    if(argc > 1)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            print_help();
            return 0;
        }
        if(strcmp(argv[1], "--version") == 0)
        {
            print_version();
            return 0;
        }
        if(strcmp(argv[1], "--verbose") || strcmp(argv[1], "-v"))
        {
            verbose = 1;
        }
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(!d)
    {
        perror("Cannot open directory");
        return 1;
    }

    while((dir = readdir(d)) != NULL)
    {
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }

        if(dir->d_type != DT_REG)
        {
            continue;
        }

        char *name = dir->d_name;
        if(has_extension(name, "jpg") || has_extension(name, "png") || has_extension(name, "jpeg") || has_extension(name, "svg"))
        {
            move_file(name, "Images");
        }
        else if(has_extension(name, "pdf") || has_extension(name, "docx") || has_extension(name, "txt") || has_extension(name, "md"))
        {
            move_file(name, "Documents");
        }
        else if(has_extension(name, "zip") || has_extension(name, "tar") || has_extension(name, "gz") || has_extension(name, "rar"))
        {
            move_file(name, "Archives");
        }
        else if(has_extension(name, "cpp") || has_extension(name, "h") || has_extension(name, "c") || has_extension(name, "py"))
        {
            if(strcmp(name, "declutter.c") != 0)
            {
                move_file(name, "Code");
            }
        }
    }

    closedir(d);
    return 0;
}
