//315314930 Shay Tzirin


#include <stdio.h>
#include <string.h>
int is_little_endian();
void part1(FILE* src, FILE* dest, int swap);
void part2(FILE* src, FILE* dest, char* flag1, char* flag2, int swap);
void part2MacTo( FILE* src, FILE* dest, char* flagTo, int order);
void part2UnixTo( FILE* src, FILE* dest, char* flagTo,int swap);
void part2WinTo( FILE* src, FILE* dest, char* flagTo, int order);
int checkKeepOrSwap(char* flag);
int checkIfFilesName(char* file1, char* file2);
int checkIfFlags(char* flag1, char* flag2);

int main(int argc, char* argv[]) {
    FILE *src, *dest;
    //using known values to check endianess of a file
    unsigned bomFE = 0xFE;
    unsigned bomFF = 0xFF;
    unsigned char bytes[2];
    int endian;
    int swap = 0;

    //impossible valid input
    if ((argc == 4) || (argc > 6) || (argc < 3)) {
        return 1;
    }
    //checks if the names of files arent "unix mac or win"
    if (checkIfFilesName(argv[1],argv[2]) == 1) {
        return 1;
    }
    //checks if the flags are valid mac/win/unix
    if ((argc == 5) || (argc == 6)) {
        if (checkIfFlags(argv[3],argv[4]) == 1) {
            return 1;
        }

    }
    if (argc == 6) {
        //whether "swap" or "keep"
        swap = checkKeepOrSwap(argv[5]);
        //this place suppose to be a flag but its not
        if (swap == -1) {
            fclose(src);
            fclose(dest);
            return 1;
        }
    }

    //opening the source file
    src = fopen(argv[1], "rb");
    if (src == NULL) {
        return 1;
    }
    //creating the destination file
    dest = fopen(argv[2], "wb");
    if (dest == NULL) {
        fclose(src);
        return 1;
    }

    if (argc == 3) {
        part1(src, dest, swap);
    }


    fread(bytes,sizeof(bytes), 1, src);
    unsigned char bytes0BackUp = bytes[0];
    unsigned char bytes1BackUp = bytes[1];
     // if the flag is "swap", change the order of bytes
    if (swap == 1) {
        unsigned char temp = bytes[0];
        bytes[0] = bytes[1];
        bytes[1] = temp;
    }
    fwrite(bytes, sizeof(bytes), 1, dest);

    //keep back ups of the original order
    bytes[0] = bytes0BackUp;
    bytes[1] = bytes1BackUp;


    if ((argc == 5) || (argc == 6)) {
        part2(src, dest, argv[3], argv[4], swap);
    }
    return 0;
}

/**
 * getting source file and copying its content to dest file
 * @param src source file pointer
 * @param dest pointer to destination file
 * @param swap telling whether theres a need tp swap bytes
 * 0 = keep, 1 = swap
 */
void part1(FILE* src, FILE* dest, int swap) {
    unsigned char bytes[2];
    //read bytes from source file
    while (fread(bytes, 1, sizeof(bytes), src) == sizeof(bytes)) {
        //swap bytes if swap flag is 1 and write them
        if (swap == 1) {
            char swappedBytes[2];
            swappedBytes[0] = bytes[1];
            swappedBytes[1] = bytes[0];
            fwrite(swappedBytes, sizeof(swappedBytes), 1, dest);
            //write bytes as written originaly
        } else {
            fwrite(bytes, 1, sizeof(bytes), dest);
        }

    }
    fclose(dest);
    fclose(src);
}

/**
 * seperating to diffrent functions according to the source flag;
 * @param src pointer to source file
 * @param dest pointer to destination file
 * @param flag1 source file flag (unix/mac/win)
 * @param flag2 destination file flag (unix/mac/win)
 * @param endian source file endianess (0 -big, 1-little)
 * @param swap determine whether to swap bytes (0-keep, 1-swap)
 */
void part2(FILE* src, FILE* dest, char* flag1, char* flag2, int swap) {
    //if the flags are the same, there are no changes needed byte wise
    //send it to part 1 function.
    if (strcmp(flag1, flag2) == 0) {
        part1(src, dest, swap);
        //send to special unix function if the source is unix
    } else if (strcmp(flag1, "-unix") == 0) {
        part2UnixTo(src, dest, flag2, swap);
        //send to special windows function if the source is windows
    } else if (strcmp(flag1, "-win") == 0) {
        part2WinTo(src, dest, flag2,swap);
        //send to special mac function if the source is mace
    } else if (strcmp(flag1, "-mac") == 0) {
        part2MacTo(src, dest, flag2,swap);
    }
}

/**
 * special function for unix source file
 * @param src pointer unix file
 * @param dest pointer to dest file
 * @param flagTo flag of destination file
 * @param endian endianess of source file
 * @param swap swap/keep
 */
void part2UnixTo( FILE* src, FILE* dest, char* flagTo, int swap) {
    unsigned char readBytes[2];
    unsigned char sourceNewLineBytes[2];
    unsigned char destNewLineBytes[2];
    unsigned char additionForWin[2];

    //read 2 bytes at a time
    while (fread(readBytes, sizeof(readBytes), 1, src) > 0) {
        //if we read unix new line bytes
        if ((0x0a == readBytes[0]) && (0x00 == readBytes[1])) {
            destNewLineBytes[0] = 0x0d;
            destNewLineBytes[1] = 0x00;
            additionForWin[0] = 0x0a;
            additionForWin[1] = 0x00;

            if (swap == 1) {
                destNewLineBytes[0] = 0x00;
                destNewLineBytes[1] = 0x0d;
                additionForWin[0] = 0x00;
                additionForWin[1] = 0x0a;
            }
            //write mac new line bytes
            fwrite(destNewLineBytes, sizeof(destNewLineBytes),1,  dest);
            //if the destination flag in windows, add only windows addition
            if (strcmp(flagTo, "-win") == 0) {
                fwrite(additionForWin, sizeof(additionForWin),1, dest);
            }
            //if it wasnt new line bytes
        } else if ((0x00 == readBytes[0]) &&
                   (0x0a == readBytes[1])) {
            destNewLineBytes[0] = 0x00;
            destNewLineBytes[1] = 0x0d;
            additionForWin[0] = 0x00;
            additionForWin[1] = 0x0d;
            if (swap == 1) {
                destNewLineBytes[0] = 0x0d;
                destNewLineBytes[1] = 0x00;
                additionForWin[0] = 0x0a;
                additionForWin[1] = 0x00;
            }
            //write mac new line bytes
            fwrite(destNewLineBytes, sizeof(destNewLineBytes),1,  dest);
            //if the destination flag in windows, add only windows addition
            if (strcmp(flagTo, "-win") == 0) {
                fwrite(additionForWin, sizeof(additionForWin),1, dest);
            }
        } else {
            //if swap flag was up - swap what we read
            if (swap == 1) {
                unsigned char readByteTemp = readBytes[0];
                readBytes[0] = readBytes[1];
                readBytes[1] = readByteTemp;
            }
            //write it
            fwrite(readBytes, sizeof(readBytes),1, dest);
        }
    }
    fclose(dest);
    fclose(src);
}



/**
 * special function for mac source file
 * @param src pointer mac file
 * @param dest pointer to dest file
 * @param flagTo flag of destination file
 * @param endian endianess of source file
 * @param swap swap/keep
 */
void part2MacTo(FILE *src, FILE *dest, char *flagTo, int order) {
    unsigned char readBytes[2];
    unsigned char uniBytes[2];
    unsigned char flag2Bytes[2];
    unsigned char winHelpArray[2];
    //read 2 bytes at a time
    while (fread(readBytes, sizeof(readBytes), 1, src) > 0) {
        //if we read mac new line bytes
        if ((0x0d == readBytes[0]) &&
                (0x00 == readBytes[1])) {
            flag2Bytes[0] = 0x0a;
            flag2Bytes[1] = 0x00;
            winHelpArray[0] = 0x0d;
            winHelpArray[1] = 0x00;

            if (order == 1) {
                flag2Bytes[0] = 0x00;
                flag2Bytes[1] = 0x0a;
                winHelpArray[0] = 0x00;
                winHelpArray[1] = 0x0d;
            }
            //if the destination flag is windows, just write again
            if (strcmp(flagTo, "-win") == 0) {
                fwrite(winHelpArray, sizeof(winHelpArray),1, dest);
            }
            //and add /n
            fwrite(flag2Bytes, sizeof(flag2Bytes),1,  dest);
        } else if ((0x00 == readBytes[0]) &&
                      (0x0d == readBytes[1])) {
            flag2Bytes[0] = 0x00;
            flag2Bytes[1] = 0x0a;
            winHelpArray[0] = 0x00;
            winHelpArray[1] = 0x0d;
            if (order == 1) {
                flag2Bytes[0] = 0x0a;
                flag2Bytes[1] = 0x00;
                winHelpArray[0] = 0x0d;
                winHelpArray[1] = 0x00;
            }
            //if the destination flag is windows, just write again
            if (strcmp(flagTo, "-win") == 0) {
                    fwrite(winHelpArray, sizeof(winHelpArray),1, dest);
            }
            //and add /n
            fwrite(flag2Bytes, sizeof(flag2Bytes),1,  dest);
        }else {
            //if theres a need to swap, swap the bytes
            if (order == 1) {
                unsigned char readByteTemp = readBytes[0];
                readBytes[0] = readBytes[1];
                readBytes[1] = readByteTemp;
            }
            fwrite(readBytes, sizeof(readBytes),1, dest);
        }
    }
    fclose(dest);
    fclose(src);
}

/**
 * special function for windows source file
 * @param src pointer windows file
 * @param dest pointer to dest file
 * @param flagTo flag of destination file
 * @param endian endianess of source file
 * @param swap swap/keep
 */
void part2WinTo(FILE *src, FILE *dest, char *flagTo, int order) {
    unsigned char readBytes[2];
    unsigned char uniBytes[2];
    unsigned char flag2Bytes[2];
    unsigned char moreOfWin[2];


    //read 2 bytes each time
    while (fread(readBytes, sizeof(readBytes), 1, src) > 0) {
        //if we read the first 2 bytes of windows "new line"
        if ((0x0d == readBytes[0]) &&
                (0x00 == readBytes[1])) {
            moreOfWin[0] = 0x0a;
            moreOfWin[1] = 0x00;
            if (strcmp(flagTo,"-mac") == 0) {
                flag2Bytes[0] = 0x0d;
                flag2Bytes[1] = 0x00;
            } else if (strcmp(flagTo, "-unix") == 0 ) {
                flag2Bytes[0] = 0x0a;
                flag2Bytes[1] = 0x00;
            }
            if (order == 1) {
                moreOfWin[0] = 0x00;
                moreOfWin[1] = 0x0a;
                if (strcmp(flagTo,"-mac") == 0) {
                    flag2Bytes[0] = 0x00;
                    flag2Bytes[1] = 0x0d;
                } else if (strcmp(flagTo, "-unix") == 0 ) {
                    flag2Bytes[0] = 0x00;
                    flag2Bytes[1] = 0x0a;
                }
            }
            //read the next 2 bytes (continue of new line bytes)
            fread(readBytes, sizeof(readBytes), 1, src);
            //then write the destionation flag new line
            fwrite(flag2Bytes, sizeof(flag2Bytes),1,  dest);
        } else if ((0x00 == readBytes[0]) &&
                   (0x0d == readBytes[1])) {
            moreOfWin[0] = 0x00;
            moreOfWin[1] = 0x0a;
            if (strcmp(flagTo,"-mac") == 0) {
                flag2Bytes[0] = 0x00;
                flag2Bytes[1] = 0x0d;
            } else if (strcmp(flagTo, "-unix") == 0 ) {
                flag2Bytes[0] = 0x00;
                flag2Bytes[1] = 0x0a;
            }
            if (order == 1) {
                moreOfWin[0] = 0x0a;
                moreOfWin[1] = 0x00;
                if (strcmp(flagTo,"-mac") == 0) {
                    flag2Bytes[0] = 0x0d;
                    flag2Bytes[1] = 0x00;
                } else if (strcmp(flagTo, "-unix") == 0 ) {
                    flag2Bytes[0] = 0x0a;
                    flag2Bytes[1] = 0x00;
                }
            }
            //read the next 2 bytes (continue of new line bytes)
            fread(readBytes, sizeof(readBytes), 1, src);
            //then write the destionation flag new line
            fwrite(flag2Bytes, sizeof(flag2Bytes),1,  dest);
        }
        else {
            if (order == 1) {
                unsigned char readByteTemp = readBytes[0];
                readBytes[0] = readBytes[1];
                readBytes[1] = readByteTemp;
            }
            fwrite(readBytes, sizeof(readBytes),1, dest);
        }
    }
    fclose(dest);
    fclose(src);
}

/**
 *
 * @param flag flag which says to keep bytes order or swap
 * @return 0 is "-keep", 1 if "-swap", -1 for illegal input
 */
int checkKeepOrSwap(char *flag) {
    if (strcmp(flag,"-keep") == 0) {
        return 0;
    } else if (strcmp(flag, "-swap") == 0) {
        return 1;
    }
    //not a possible flag
    return -1;

}

/**
 * checks if the given file names arent flags names
 * @param file1 file name
 * @param file2 file name
 * @return 0 if names are ok, 1 if not
 */
int checkIfFilesName(char *file1, char *file2) {
    if ((strcmp(file1,"-win")== 0) ||
        (strcmp(file1,"-mac") == 0) ||
        (strcmp(file1,"-unix") == 0)) {
        return 1;
    }
    if ((strcmp(file2,"-win")== 0) ||
            (strcmp(file2,"-mac") == 0) ||
            (strcmp(file2,"-unix") == 0)) {
        return 1;
    }
    return 0;
}

/**
 * checks if the flags are flags names
 * @param flag1 flag name
 * @param flag2 flag name
 * @return  0 if flags are flags, 1 if not
 */
int checkIfFlags(char *flag1, char *flag2) {
    if ((strcmp(flag1, "-win") == 0) ||
        (strcmp(flag1, "-mac") == 0) ||
        (strcmp(flag1, "-unix") == 0)) {
        if ((strcmp(flag2, "-win") == 0) ||
            (strcmp(flag2, "-mac") == 0) ||
            (strcmp(flag2, "-unix") == 0)) {
            return 0;
        }
    }
    return 1;
}
