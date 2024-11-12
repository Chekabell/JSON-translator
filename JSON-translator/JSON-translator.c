#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

int addNewWord(cJSON* dict, char* word, char* translate) {
    if (dict == NULL) return 0;
    cJSON* arrTranslate = cJSON_CreateArray();
    cJSON_AddItemToArray(arrTranslate, cJSON_CreateString(translate));
    cJSON_AddItemToObject(dict, word, arrTranslate);
    return 1;
}

int addNewTranslateWord(cJSON* dict, char* word, char* translate) {
    if (dict == NULL) return 0;
    cJSON* arrTranslate = cJSON_GetObjectItem(dict, word);
    if (cJSON_IsArray(arrTranslate)) {
        cJSON_AddItemToArray(arrTranslate, cJSON_CreateString(translate));
        return 1;
    }
    return 0;
}

char* toLower(char* word) {
    for (; *word; ++word) *word = tolower(*word);
    return word;
}

int HasTranslateWord(cJSON* dict, char* word, char* translate) {
    if (dict == NULL) return 0;
    cJSON* arrTranslate = cJSON_GetObjectItem(dict, word);
    cJSON* element;
    if (cJSON_IsArray(arrTranslate)) {
        cJSON_ArrayForEach(element, arrTranslate) {
            if(strcmp(element->valuestring,translate) == 0){
                return 1;
            }
        }
    }
    return 0;

}
void printWordWithTranslate(cJSON* dict, char * word) {
    if (dict == NULL) return;
    cJSON* arrTranslate;
    cJSON* element;
    arrTranslate = cJSON_GetObjectItem(dict, word);
    if (cJSON_IsArray(arrTranslate)) {
        printf("%s -", word);
        cJSON_ArrayForEach(element, arrTranslate) {
            if (element->next == NULL) {
                printf(" %s.", element->valuestring);
                break;
            }
            printf(" %s,", element->valuestring);
        }
        printf("\n");
    }
}

void printAllDict(cJSON* dict) {
    if (dict == NULL) return;
    cJSON* elWord;
    cJSON* elTranslate;
    for (elWord = dict->child; elWord != NULL; elWord = elWord->next) {
        printf("%s -", elWord->string);
        for (elTranslate = elWord->child; elTranslate != NULL; elTranslate = elTranslate->next) {
            if (elTranslate->next == NULL) {
                printf(" %s;", elTranslate->valuestring);
                break;
            }
            printf(" %s,", elTranslate->valuestring);
        }
        printf("\n");

    }
}

cJSON* readJSON(char* fileName) {
    strcat_s(fileName, 10, ".json");
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Ошибка! Такого файла не существует.\n");
        return NULL;
    }

    // read the file contents into a string 
    char buffer[2048];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    // parse the JSON data 
    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Ошибка: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return NULL;
    }
    return json;

}

int saveJSON(cJSON* dict, char * fileName) {
    strcat_s(fileName, 10, ".json");

    char* json_str = cJSON_Print(dict);
    // write the JSON string to a file 
    FILE* fp = fopen(fileName, "w");
    if (fp == NULL) {
        printf("Ошибка! Не получилось открыть файл.\n");
        return 0;
    }
    fputs(json_str, fp);
    fclose(fp);
    return 1;
}

//int main() {
//    setlocale(LC_ALL, "Rus");
//
//    char fileName[10] = "test";
//    toLower(fileName);
//    cJSON* dict = readJSON(fileName);
//    char* str = cJSON_Print(dict);
//    printf("%s\n", str);
//    return 0;
//}


int main() {
    system("chcp 1251 > nul");

    cJSON* dict = NULL;
    int key = 0;
    char nameFile[10] = "";
    char bufferWord[20];
    char bufferTranslatedWord[20];

    while (1) {
        if (nameFile[0] != '\0') printf("\nСловарь - %s\n", nameFile);

        printf("1 - Создать словарь;\n2 - Открыть словарь;\n3 - Добавить слово;"
            "\n4 - Найти слово;\n5 - Показать весь словарь;"
            "\n6 - Сохранить и отключить словарь;\n7 - Выйти из приложения\n");
        printf("Выберите вариант: ");
        while (scanf_s("%i", &key) < 1 && key >= 1 && key <= 7) {
            printf("Ошибка! Введите существующий вариант: ");
            while ((getchar()) != '\n');
        }
        switch (key) {
        case 1:
            if (dict != NULL)
                printf("Прежде чем открыть или создать новый словарь, закройте старый.\n");
            else {
                printf("Введите имя файла: ");
                if (scanf_s("%s", nameFile, 5))
                    dict = cJSON_CreateObject();
            }
            break;
        case 2:
            if (dict != NULL)
                printf("Преждем чем открыть или создать новый словарь, закройте старый.\n");
            else {
                printf("Введите имя файла: ");
                if (scanf_s("%s", nameFile, 5)) {
                    strcpy_s(bufferWord, 5, nameFile);
                    dict = readJSON(nameFile);
                    if (dict == NULL) {
                        strcpy_s(nameFile, 1, "");
                    }
                    strcpy_s(nameFile, 5, bufferWord);
                }
                else {
                    printf("Ошибка чтения имени файла! Попробуйте ещё раз.\n");
                    strcpy_s(nameFile, 1, "");
                }
            }
            break;
        case 3:
            if (dict == NULL) {
                printf("Сначала откройте словарь!\n");
            }
            else {
                printf("Введите новое слово: ");
                if (scanf_s("%s", bufferWord, 20)) {
                    if (!cJSON_HasObjectItem(dict, bufferWord)) {
                        toLower(bufferWord);
                        printf("Введите слово перевод: ");
                        if (scanf_s("%s", bufferTranslatedWord, 20)) {
                            toLower(bufferTranslatedWord);
                            if (addNewWord(dict, bufferWord, bufferTranslatedWord)) {
                                printf("Слово и перевод добавлены.\n");
                            }
                        }
                        else
                            printf("Не удалось считать слово.\n");
                    }
                    else {
                        printf("Это слово уже есть в словаре. Какой перевод вы хотите добавить?\n");
                        printf("Введите слово перевод: ");
                        if (scanf_s("%s", bufferTranslatedWord, 20)) {
                            if (!HasTranslateWord(dict, bufferWord, bufferTranslatedWord)) {
                                toLower(bufferTranslatedWord);
                                if (addNewTranslateWord(dict, bufferWord, bufferTranslatedWord)) {
                                    printf("Перевод слова добавлен.\n");
                                }
                            }
                            else
                                printf("Этот перевод слова уже записан.\n");
                        }
                        else
                            printf("Не удалось считать слово.\n");
                    }
                }
                else
                    printf("Не удалось считать слово.\n");
            }
            break;
        case 4:
            if (dict == NULL) {
                printf("Сначала откройте словарь.\n");
            }
            else {
                printf("Введите слово: ");
                if (scanf_s("%s", bufferWord, 20)) {
                    if (cJSON_HasObjectItem(dict, bufferWord)) {
                        printWordWithTranslate(dict, bufferWord);
                    }
                    else {
                        printf("Такого слова нет в словаре.\n");
                    }
                }
                else {
                    printf("Ошибка ввода!\n");
                }
            }
            break;
        case 5:
            if (dict == NULL)
                printf("Словарь ещё не открыт.");
            else
                printAllDict(dict);
            break;
        case 6:
            if (dict == NULL) {
                printf("Сначала откройте словарь\n");
            }
            else {
                saveJSON(dict, nameFile);
                cJSON_Delete(dict);
                strcpy_s(nameFile, 1, "");
                dict = NULL;
                printf("\n");
            }
            break;
        case 7:
            if (dict != NULL) {
                saveJSON(dict, nameFile);
                cJSON_Delete(dict);
            }
            return 0;
        }
    }
}