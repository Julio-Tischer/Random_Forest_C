#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int *iSplit(float ratio, int number_of_indexes, int *output);
int checkRepeat(int *vector, int new_entry, int sizeOf_Vector);
int *invertList(int indexes, int *trim_Vector, int SizeOf_Trim);

#define MAX_LINES 200
#define MAX_LINE_LENGTH 1000

int main()
{
    // VARIAVEIS
    FILE *data = NULL;
    FILE *trainData = NULL;
    FILE *testData = NULL;

    char data_string[MAX_LINES][MAX_LINE_LENGTH];
    char header[MAX_LINE_LENGTH];

    int i = 0;
    int indices = 0;
    int testNumber;
    int data_line_count = 0;

    int *testList = NULL;
    int *trainList = NULL;

    // Inicializar seed random
    srand(time(NULL));

    // ABRIR ARQUIVO DE DATA
    data = fopen("data.csv", "r");
    if (data == NULL)
    {
        printf("---ERRO DE ABERTURA DE DATA---");
        return 1;
    }

    if (fgets(header, MAX_LINE_LENGTH, data) == NULL)
    {
        printf("--- ERRO: Arquivo 'data.csv' está vazio ---\n");
        fclose(data);
        return 1;
    }
    // Remove a quebra de linha
    header[strcspn(header, "\r\n")] = 0;
    printf("Cabeçalho foi lido: %s\n", header);

    while (fgets(data_string[i], MAX_LINE_LENGTH, data) != NULL)
    {
        data_string[i][strcspn(data_string[i], "\r\n")] = 0;
        if (strlen(data_string[i]) > 0)
        {
            i++;
        }
        if (i >= MAX_LINES)
        {
            printf("Aviso: O arquivo excedeu %d linhas. Leitura interrompida.\n", MAX_LINES);
            break;
        }
    }

    data_line_count = i;
    printf("\n%d linhas de dados anotadas, cabeçalho excluido\n", data_line_count);
    // Verifica se tem dados suficientes para separar
    if (data_line_count < 2)
    {
        printf("--- ERRO: Não há dados suficientes para realizar a separação (apenas %d linhas) ---\n", data_line_count);
        fclose(data);
        return 1;
    }
    testList = iSplit(0.2, data_line_count, &testNumber);
    trainList = invertList(data_line_count, testList, testNumber);

    trainData = fopen("train.txt", "w");
    testData = fopen("test.txt", "w");

    if (trainData == NULL || testData == NULL)
    {
        printf("--- ERRO: Não foi possível criar os arquivos de saída ---\n");
        return 1;
    }
    // Escreva o cabeçalho nos arquivos
    fprintf(trainData, "%s\n", header);
    fprintf(testData, "%s\n", header);

    // Escrever no arquivo test.txt
    for (i = 0; i < testNumber; i++)
    {
        fprintf(testData, "%s\n", data_string[testList[i]]);
    }
    printf("Escritas %d linhas em 'test.txt'\n", testNumber);

    // Escrever no arquivo train.txt
    int trainNumber = data_line_count - testNumber;
    for (i = 0; i < trainNumber; i++)
    {
        fprintf(trainData, "%s\n", data_string[trainList[i]]);
    }
    printf("Escritas %d linhas em 'train.txt'\n", trainNumber);
    // Finalização do codigo

    printf("\nArquivos 'train.txt' e 'test.txt' gerados com sucesso!\n");

    system("pause");

    free(testList);
    free(trainList);
    fclose(data);
    fclose(trainData);
    fclose(testData);
    return 0;
}

int *iSplit(float ratio, int number_of_indexes, int *output)
{
    // dividir indices por ratio para descobrir qtd de entradas (splitNumber)
    int splitNumber = round(number_of_indexes * ratio);
    *output = splitNumber;
    int randomNumber;
    int *arrayBuff;

    arrayBuff = (int *)malloc(splitNumber * sizeof(int)); // Define o tamnho do output

    if (output == NULL)
    {
        printf("\n\nERRO NA ALOCAÇÃO DINAMICA iSPLIT--");
        exit(0);
    }

    // Aleatoriamente selecionar numeros de 1 a indice, qtd de entrada vezes
    for (int i = 0; i < splitNumber; i++)
    {
        randomNumber = rand() % number_of_indexes + 1;

        // Verificar se numero é repetido
        if (checkRepeat(arrayBuff, randomNumber, i) == 0)
        {
            arrayBuff[i] = randomNumber;
            printf("\nSelecionado o numero %d\n", arrayBuff[i]);
        }
        else
        {
            i--;
        }
    }

    printf("\n---Criado uma lista com %d numeros\n", splitNumber);
    return arrayBuff;
}

int checkRepeat(int *vector, int new_entry, int sizeOf_Vector)
{
    int flag = 0;
    // Flag para anotar se numero repete

    for (int i = 0; i < sizeOf_Vector; i++)
    {
        if (new_entry == vector[i])
        {
            flag = 1;
            printf("\n---NUMERO %d E REPETIDO---\n", new_entry);
        }
        else
        {
        }
    }

    // Se houver repetido, retorna 1;
    return flag;
}

int *invertList(int indexes, int *trim_Vector, int SizeOf_Trim)
{
    int j = 0;
    int *output_Vector;

    output_Vector = (int *)malloc((indexes - SizeOf_Trim) * sizeof(int));

    // Criar lista de 1 a indexes sem os valores de trim_Vector
    for (int i = 0; i < indexes; i++)
    {
        if (checkRepeat(trim_Vector, i, SizeOf_Trim) == 0)
        {
            output_Vector[j] = i;
            j++;
        }
        else
        {
            printf("\n--- iL ignorando %d---", i);
        }
    }

    if (output_Vector == NULL)
    {
        printf("\n\n---ERRO ALOCACAO DINAMICA invertList---\n\n");
        exit(0);
    }

    return output_Vector;
}