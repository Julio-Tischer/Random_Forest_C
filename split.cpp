#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int* iSplit(float ratio, int number_of_indexes, int* output);
int checkRepeat (int* vector, int new_entry, int sizeOf_Vector);
int* invertList(int indexes, int* trim_Vector,int SizeOf_Trim);

int main()
{
    //VARIAVEIS
    FILE* data = NULL;
    FILE* trainData = NULL;
    FILE* testData = NULL;
    char data_string[200][1000]; 
    int i = 0;
    int indices=0;
    int testNumber;

    int* testList = NULL;
    int* trainList = NULL;

    //Inicializar seed random
    srand(time(NULL));

    //ABRIR ARQUIVO DE DATA
    data = fopen("data.csv","r");
    if (data==NULL)
    {
        printf("---ERRO DE ABERTURA DE DATA---");
        return 1;
    }

    //Anota toda a data 
    while(!feof(data))
    {
        fscanf(data, "%s",data_string[i]);
        printf("%s\n",data_string[i]);
        i++;    
    }

    //i-2 é igual a quantidade de indices (pela primeira linha, e contador i++ da ultima)
    indices = i-2;
    i = 0;

    printf("\n\n%d indices anotados",indices);

    //Gera lista de indices de teste
    testList = iSplit(0.2, indices, &testNumber);

    //Gera lista de indices de treino
    trainList = invertList(indices, testList, testNumber);

    //Cria os arquivos test e train e anota seus conteudos
    trainData = fopen("train.csv","w");
    testData = fopen("test.csv","w");

    //Printa o header da tabela, como invertList mantem indice 0 (ja que iSplit não escolhe 0), trainData não precisa
    fprintf(testData, "%s\n", data_string[0]);

    //Escrever no arquivo testData
    for(i=0; i<testNumber;i++)
    {
        fprintf(testData,"%s\n",data_string[testList[i]]);
    }

    //Escrever no arquivo trainData
    for(i=0; i<(indices-testNumber);i++)
    {
        fprintf(trainData,"%s\n",data_string[trainList[i]]);
    }

    //Finalização do codigo
    getchar();
    free(testList);
    free(trainList);
    fclose(data);
    fclose(trainData);
    fclose(testData);
    return 0;
}



int* iSplit(float ratio, int number_of_indexes, int* output)
{
    //dividir indices por ratio para descobrir qtd de entradas (splitNumber)
    int splitNumber = round(number_of_indexes*ratio);
    *output = splitNumber;
    int randomNumber;
    int* arrayBuff;

    arrayBuff = (int*)malloc(splitNumber*sizeof(int)); //Define o tamnho do output

    if (output == NULL)
    {
        printf("\n\nERRO NA ALOCAÇÃO DINAMICA iSPLIT--");
        exit(0);
    }

    //Aleatoriamente selecionar numeros de 1 a indice, qtd de entrada vezes
    for (int i=0; i<splitNumber; i++)
    {
        randomNumber = rand()%number_of_indexes + 1;

        //Verificar se numero é repetido
        if (checkRepeat(arrayBuff, randomNumber, i)==0){
            arrayBuff[i] = randomNumber;
            printf("\nSelecionado o numero %d\n",arrayBuff[i]);
        }
        else {i--;}
    }

    printf("\n---Criado uma lista com %d numeros\n",splitNumber);
    return arrayBuff;
}


int checkRepeat (int* vector, int new_entry, int sizeOf_Vector)
{
    int flag=0;
    //Flag para anotar se numero repete

    for (int i=0; i<sizeOf_Vector; i++)
    {
        if (new_entry == vector[i])
        {
            flag=1;
            printf("\n---NUMERO %d E REPETIDO---\n",new_entry);
        }
        else{}
    }

    //Se houver repetido, retorna 1;
    return flag;
}


int* invertList(int indexes, int* trim_Vector,int SizeOf_Trim)
{
    int j = 0;
    int* output_Vector;

    output_Vector = (int*)malloc((indexes-SizeOf_Trim)*sizeof(int));

    //Criar lista de 1 a indexes sem os valores de trim_Vector
    for (int i=0; i<indexes; i++)
    {
        if (checkRepeat(trim_Vector, i, SizeOf_Trim)==0)
        {
            output_Vector[j] = i;
            j++;
        }
        else
        {
            printf("\n--- iL ignorando %d---",i);
        }
    }
    
    if (output_Vector==NULL)
    {
        printf("\n\n---ERRO ALOCACAO DINAMICA invertList---\n\n");
        exit(0);
    }

    return output_Vector;
}