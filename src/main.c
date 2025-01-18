#define SDL_MAIN_HANDLED
#define _XOPEN_SOURCE 700
#include "common/common.h"
#include <dirent.h>
#include <sys/types.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_FILE_SIZE 10000000

#define INPUT_SPLIT_PATH_PREFIX "./inputSplit/"
#define INPUT_COMPLETE_PATH_PREFIX "./inputComplete/"
#define OUTPUT_SPLIT_PATH_PREFIX "./outputSplit/"
#define OUTPUT_COMPLETE_PATH_PREFIX "./outputComplete/"

#define INPUT_MENU_LIST_COUNT 14

uint64_t getBufferLength(const char* buffer){
    uint64_t length = 0;
    while(*buffer != 0x0){
        buffer++;
        length++;
    }
    return length;
}

typedef struct String{
    uint64_t size;
    char* chars;
}String;

String* createString(uint64_t size){
    String* string = malloc(sizeof(String));
    string->size = size;
    string->chars = malloc(size * sizeof(char));
    return string;
}

String* createStringFromBuffer(const char* chars){
    String* string = malloc(sizeof(String));
    string->size = getBufferLength(chars) + 1;
    string->chars = malloc(string->size * sizeof(char));
    memcpy(string->chars, chars, string->size);
    return string;
}

void destroyString(String* string){
    free(string->chars);
    free(string);
}

void writeStringToFile(String* string, FILE* file){
    for(uint64_t i = 0; i < string->size; i++){
        fprintf(file, "%c", string->chars[i]);
    }
}



String* concatenate(String* string1, String* string2){
    uint64_t bruh = (string1->size + string2->size) * sizeof(char);
    String* result = createString(bruh);
    memcpy(result->chars, string1->chars, string1->size);
    memcpy(result->chars + string1->size * sizeof(char) - 1, string2->chars, string2->size);
    return result;
}

void listDir(const char* dir){
    DIR* directory;
    struct dirent* entries;
    directory = opendir("./");
    if(directory != NULL){
        while((entries = readdir(directory)) != NULL){
            printf("%s\n", entries->d_name);
        }
        closedir(directory);
    }
    else{
        printf("Couldn't open the directory");
    }
}

uint64_t getDirEntryCount(const char* path){
    DIR* directory = NULL;
    struct dirent* entries = NULL;
    uint64_t entryCount = 0;

    directory = opendir(path);
        if(directory == NULL){
        return 0;
    }

    (void)readdir(directory);
    (void)readdir(directory);
    //count number of entries
    while((entries = readdir(directory)) != NULL){
        entryCount++;
    }
    closedir(directory);

    return entryCount;
}

String** getDir(const char* path){

    String** directoryEntries = NULL;
    DIR* directory = NULL;
    struct dirent* entries = NULL;
    uint64_t entryCount = 0;

    entryCount = getDirEntryCount(path);

    //allocate a String* array for each entry
    directoryEntries = malloc(entryCount * sizeof(String*));

    directory = opendir(path);

    if(directory == NULL){
        return NULL;
    }

    (void)readdir(directory);
    (void)readdir(directory);

    //i starts at 2 to skip "." and ".." directories
    for(int i = 0; i < entryCount; i++){
        entries = readdir(directory);
        uint64_t length = getBufferLength(entries->d_name);
        directoryEntries[i] = createString(length + 1);
        memcpy(directoryEntries[i]->chars, entries->d_name, directoryEntries[i]->size);
        //printf("%s\n", entries->d_name);
    }

    closedir(directory);

    return directoryEntries;
}

void completeToSplit(String* fileName){

    //TODO
    //DONE Add path prefix to file name so there's no need to call function with the entire complete input path
    //DONE Rename every subfile to name format: filename.extension.n.file
        //DONE concatenate original file name and extension + "." + subfile number + ".file"

    //Original file path
    String* inputFilePath;
    //Input path prefix
    String* inputPrefix;
    //Original file, so be split
    FILE* inputFile = NULL;
    //File size in number of bytes
    uint64_t fileSize = 0;
    //Byte buffer containing the entire extracted file
    String* content;
    //Original file split in slices of MAX_FILE_SIZE or less for the last one
    String** subContent = NULL;
    //Number of subfiles created to split original file
    int subFileCount = 0;
    //Array of subfiles to be created and written into
    FILE** outputFiles = NULL;

    inputPrefix = createStringFromBuffer(INPUT_COMPLETE_PATH_PREFIX);
    inputFilePath = concatenate(inputPrefix, fileName);

    inputFile = fopen(inputFilePath->chars, "rb");
    fseek(inputFile, 0, SEEK_END);
    fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);
    content = createString(fileSize);
    fread(content->chars, sizeof(char), fileSize, inputFile);

    subFileCount = (fileSize + MAX_FILE_SIZE - 1) / MAX_FILE_SIZE;
    subContent = malloc(subFileCount * sizeof(String*));

    for(int i = 0, remainingFileSize = fileSize; remainingFileSize != 0; i++){
        if(remainingFileSize >= MAX_FILE_SIZE){
            subContent[i] = createString(MAX_FILE_SIZE);
            memcpy(subContent[i]->chars, content->chars, MAX_FILE_SIZE * sizeof(char));
            content->chars += MAX_FILE_SIZE * sizeof(char);
            remainingFileSize -= MAX_FILE_SIZE;
        }
        else{
            subContent[i] = createString(remainingFileSize);
            memcpy(subContent[i]->chars, content->chars, remainingFileSize * sizeof(char));
            content->chars += remainingFileSize;
            remainingFileSize -= remainingFileSize;
        }
    }
    fseek(inputFile, 0, SEEK_SET);
    content->chars -= fileSize;

    outputFiles = malloc(subFileCount * sizeof(FILE*));

    for(int i = 0; i < subFileCount; i++){
        char* fileNameBuffer = malloc(280 * sizeof(char));
        sprintf(fileNameBuffer,"outputSplit/%s.%i.file", fileName->chars, i + 1);
        outputFiles[i] = fopen(fileNameBuffer, "wb");
        free(fileNameBuffer);
        printf("Writing %i bytes to file #%i", subContent[i]->size, i);
        fwrite(subContent[i]->chars, sizeof(char), subContent[i]->size, outputFiles[i]);
        destroyString(subContent[i]);
        fclose(outputFiles[i]);

    }

    fclose(inputFile);
    destroyString(inputFilePath);
    destroyString(inputPrefix);
    free(outputFiles);
    destroyString(content);
    free(subContent);
}

void splitToComplete(){

    FILE** inputFiles = NULL;
    FILE* outputFile = NULL;
    String** subContent = NULL;
    String** directory = NULL;
    String* fileName = NULL;
    String* temp = NULL;
    int subFileCount = 0;

    

    directory = getDir(INPUT_SPLIT_PATH_PREFIX);

    temp = directory[0];

    int dots = 0;
    int fileNameSize = 0;
    for(int i = temp->size - 1; i >= 0; i--){
        if(temp->chars[i] == '.'){
            dots++;
        }
        if(dots == 2){
            fileNameSize = i + 1;
            break;
        }
    }

    fileName = createString(fileNameSize);
    memcpy(fileName->chars, temp->chars, fileNameSize - 1);
    fileName->chars[fileName->size - 1] = 0x00;
    //printf("Opening file: %s\n", fileName->chars);

    subFileCount = getDirEntryCount(INPUT_SPLIT_PATH_PREFIX);

    inputFiles = malloc(subFileCount * sizeof(FILE*));
    subContent = malloc(subFileCount * sizeof(char*));

    for(int i = 0; i < subFileCount; i++){
        String* pathPrefix = createString(sizeof(INPUT_SPLIT_PATH_PREFIX));
        memcpy(pathPrefix->chars, INPUT_SPLIT_PATH_PREFIX, pathPrefix->size);
        String* path = concatenate(pathPrefix, directory[i]);
        printf("Opening file: %s\n", path->chars);
        inputFiles[i] = fopen(path->chars, "rb");
        destroyString(pathPrefix);
        destroyString(path);
        fseek(inputFiles[i], 0, SEEK_END);
        subContent[i] = createString(ftell(inputFiles[i]));
        fseek(inputFiles[i], 0, SEEK_SET);
        fread(subContent[i]->chars, sizeof(char), subContent[i]->size, inputFiles[i]);
    }

    char* outputFilePath = malloc(sizeof(OUTPUT_COMPLETE_PATH_PREFIX) + fileName->size);
    sprintf(outputFilePath, "%s%s", OUTPUT_COMPLETE_PATH_PREFIX, fileName->chars); 
    outputFile = fopen(outputFilePath, "wb");
    free(outputFilePath);

    for(int i = 0; i < subFileCount; i++){
        fwrite(subContent[i]->chars, sizeof(char), subContent[i]->size, outputFile);
    }

    fclose(outputFile);

    for(int i = 0; i < subFileCount; i++){
        fclose(inputFiles[i]);
        free(subContent[i]);
    }
    free(inputFiles);
    free(subContent);

}

int main(int argc, char** argv){

    //String** directories = getDir("./");

    //String* fileName = createStringFromBuffer("ryujinx-1.1.343-win_x64.zip");
    //completeToSplit(fileName);

    //splitToComplete();

    //common_shutdown();


    //getchar();
    //getchar();

    //return 0;


    common_startup();
    char windowTitle[32];
    bool runs = true;

    SDL_SetWindowTitle(window, "Club's file decomposition and recomposition tool");

    SDL_Texture* image;

    TTF_Font* font = TTF_OpenFont("ressources/fonts/calibri.ttf", 40);

    bool mainMenuActive = true;
    bool splitFileSelectMenuActive = false;
    //Don't forget to set this back to 0 whenever the menu is changed
    uint64_t menuIndex = 0;

    String** splitFileMenuOptions;
    uint64_t splitFileMenuOptionsCount = 0;

    SDL_Surface* splitFileOptionSurface = TTF_RenderText_Solid(font, "Decompose a file in ./inputComplete into multiple subfiles", (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
    SDL_Surface* mergeFileOptionSurface = TTF_RenderText_Solid(font, "Recompose subfiles in ./inputSplit back into the original file", (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
    SDL_Surface* menuCursorSurface = TTF_RenderText_Solid(font, ">", (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
    SDL_Rect splitFileOptionRec;
    SDL_Rect mergeFileOptionRec;
    SDL_Rect menuCursorRec;
    SDL_Texture* splitFileOptionTexture = SDL_CreateTextureFromSurface(renderer, splitFileOptionSurface);
    SDL_Texture* mergeFileOptionTexture = SDL_CreateTextureFromSurface(renderer, mergeFileOptionSurface);
    SDL_Texture* menuCursorTexture = SDL_CreateTextureFromSurface(renderer, menuCursorSurface);

    splitFileOptionRec.x = 30;
    splitFileOptionRec.y = 20;
    splitFileOptionRec.w = splitFileOptionSurface->w;
    splitFileOptionRec.h = splitFileOptionSurface->h;

    mergeFileOptionRec.x = 30;
    mergeFileOptionRec.y = 60;
    mergeFileOptionRec.w = mergeFileOptionSurface->w;
    mergeFileOptionRec.h = mergeFileOptionSurface->h;

    menuCursorRec.x = 10;
    menuCursorRec.y = 20;
    menuCursorRec.w = 20;
    menuCursorRec.h = 30;

    SDL_Surface* fileOptionsSurface;
    SDL_Rect fileOptionsRect;
    SDL_Texture* fileOptionsTexture;

    fileOptionsRect.x = 30;
    fileOptionsRect.y = 20;
    fileOptionsRect.w = 550;
    fileOptionsRect.h = 30;

    
    SDL_Surface* FpsLabelSurface = TTF_RenderText_Solid(font, "FPS: ", (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
    SDL_Texture* FpsLabelTexture = SDL_CreateTextureFromSurface(renderer, FpsLabelSurface);
    SDL_Rect FpsLabelRect;
    SDL_Rect fumoRect;
    int width = 0, height = 0;

    image = IMG_LoadTexture(renderer, "./fumo.png");
    SDL_QueryTexture(image, NULL, NULL, &width, &height);
    fumoRect.x = WIDTH / 2;
    fumoRect.y = HEIGHT / 2;
    fumoRect.w = width/2;
    fumoRect.h = height/2;
    FpsLabelRect.x = 20;
    FpsLabelRect.y = 20;
    FpsLabelRect.w = FpsLabelSurface->w;
    FpsLabelRect.h = FpsLabelSurface->h;

    while(runs){

        SDL_Delay(4);
        
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case(SDL_QUIT): {
                    runs = false;
                    break;
                }
                case(SDL_KEYDOWN): {
                    switch(e.key.keysym.sym){
                        case(SDLK_UP): {
                            menuIndex--;
                            //fumoRect.y-= 5;;
                            break;
                        }
                        case(SDLK_LEFT): {
                            //fumoRect.x-= 5;
                            break;
                        }
                        case(SDLK_DOWN): {
                            menuIndex++;
                            //fumoRect.y+= 5;
                            break;
                        }
                        case(SDLK_RIGHT): {
                            //fumoRect.x+= 5;
                            break;
                        }
                        case(SDLK_RETURN): {
                            //printf("Enter\n");

                            if(splitFileSelectMenuActive){
                                printf("%s\n", splitFileMenuOptions[menuIndex]->chars);
                                String* fileName = createStringFromBuffer(splitFileMenuOptions[menuIndex]->chars);
                                completeToSplit(fileName);
                                destroyString(fileName);
                            }
                            else if(menuIndex == 1){
                                splitToComplete();
                            }
                            
                            if(menuIndex == 0){
                                splitFileSelectMenuActive = true;
                                mainMenuActive = false;

                                //Free previous menu options if there were any
                                if(splitFileMenuOptionsCount > 0){
                                    for(int i = 0; i < splitFileMenuOptionsCount; i++){
                                        free(splitFileMenuOptions[i]);
                                    }
                                    free(splitFileMenuOptions);
                                }

                                splitFileMenuOptionsCount = getDirEntryCount(INPUT_COMPLETE_PATH_PREFIX);
                                splitFileMenuOptions = getDir(INPUT_COMPLETE_PATH_PREFIX);


                            }
                            break;
                        }
                        case(SDLK_ESCAPE): {
                            if(splitFileSelectMenuActive){
                                splitFileSelectMenuActive = false;
                                mainMenuActive = true;
                                menuIndex = 0;
                            }
                            break;
                        }

                    }
                    break;
                }

            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, image, NULL, &fumoRect);
 
        //SDL_RenderCopy(renderer, FpsLabelTexture, NULL, &FpsLabelRect);

        if(mainMenuActive){
            menuIndex %= 2;
            menuCursorRec.y = 20 + menuIndex * 40; 


            SDL_RenderCopy(renderer, splitFileOptionTexture, NULL, &splitFileOptionRec);
            SDL_RenderCopy(renderer, mergeFileOptionTexture, NULL, &mergeFileOptionRec);
            SDL_RenderCopy(renderer, menuCursorTexture, NULL, &menuCursorRec);
        }

        if(splitFileSelectMenuActive){

            if(menuIndex < 0){
                menuIndex = 0;
            }
            if(menuIndex >= splitFileMenuOptionsCount){
                menuIndex = 0;
            }
            for(int i = menuIndex; i < INPUT_MENU_LIST_COUNT + menuIndex; i++){
                if(i >= splitFileMenuOptionsCount){
                    break;
                }

                fileOptionsSurface = TTF_RenderText_Solid(font, splitFileMenuOptions[i]->chars, (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
                fileOptionsTexture = SDL_CreateTextureFromSurface(renderer, fileOptionsSurface);
                fileOptionsRect.y = 20 + (i - menuIndex) * 40;
                fileOptionsRect.w = fileOptionsSurface->w;
                fileOptionsRect.h = fileOptionsSurface->h;

                SDL_RenderCopy(renderer, fileOptionsTexture, NULL, &fileOptionsRect);

                SDL_FreeSurface(fileOptionsSurface);
                SDL_DestroyTexture(fileOptionsTexture);



            }
            SDL_RenderCopy(renderer, menuCursorTexture, NULL, &menuCursorRec);
        }


        SDL_RenderPresent(renderer);
    }

    common_shutdown();


    return 0;
}