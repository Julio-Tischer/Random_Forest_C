#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define tamanhoStrings 30
#define respostaPositiva ('P')

int checkRepeat (int* vector, int new_entry, int sizeOf_Vector);
float** sortSTG (char*** matrizDados,int coluna, int numeroLinhas, int numeroFeatures);
//Aqui coloque o valor para considerar como positivo

//MEU DEUS STRUCTS SÃO MUITO TOPPPPPPPPPPPPPPP

//Esse struct armazena a lista de features e seus threshoulds ideais, ordem de impureza crescente
struct ordemFeature 
{
    int* Troncos;
    int* threshould;
};

//Essa função retorna um struct de formato ordemFeature
struct ordemFeature giniImpurity(int numeroLinhas, int numeroFeatures, char*** matrizDados)
{
    //Declara um struct local
    struct ordemFeature localStruct; 

    float threshold=0;

    //Vamos precisa de um buffer de matriz int para ordenar nossas features
    //O buffer tera "numeroLinhas" linhas e 3 colunas (para a feature e sua resposta)
    float** bufferListaFeatures = NULL;

    //Para toda feature (menos a resposta)
    for (int i=0;i<numeroFeatures-2;i++)
    {
        //Ordena de menor para maior
        bufferListaFeatures = sortSTG(matrizDados,i+1,numeroLinhas,numeroFeatures);
        for (int j=0; j<(numeroLinhas-2);j++)
        {
            threshold = ((bufferListaFeatures[j][0]+bufferListaFeatures[j+1][0])/2);
            printf("--Threshold e %f\n",threshold);
        }

    }

    //Encerração da funcão (não esqueça de desalocar o bufferLista pvfr)
    
    return localStruct;
}

//Essa função ordenará a lista de forma crescente, mantendo a relação com sua resposta
//É aqui que dizemos adeus ao cabeçalho
float** sortSTG (char*** matrizDados,int coluna, int numeroLinhas, int numeroFeatures) 
{
    //Vamos criar uma lista de respostas para acompanhar nosso buffer
    int menorNumero;
    int indice;
    int* listaRespostas = NULL;
    int* indicesExcluidos = NULL;
    float* buffMatriz = NULL;
    float** matrizOrdenada = NULL;

    //matrizOrdenada sera o valor de retorno, tera tamnanho "numerolinhas-1" por 2
    matrizOrdenada = (float**)malloc((numeroLinhas-1)*sizeof(float*));
    //para cada linha duas colunas
    for (int i=0; i<(numeroLinhas-1);i++)
    {
        matrizOrdenada[i] = (float*)malloc(2*sizeof(float));
    }

    indicesExcluidos = (int*)malloc((numeroLinhas-1)*sizeof(int));
    if (indicesExcluidos == NULL)
    {
        printf("--ERRO iexcluidos DINAMICA");
        getchar();
        exit(1);
    }
    //Vamos preencher indicesexcluidos com indices impossiveis inicalmente para não ter problemas com checkRepeat
    for (int i=0;i<(numeroLinhas-1);i++)
    {
        indicesExcluidos[i]=-1;
    }

    listaRespostas = (int*)malloc((numeroLinhas-1)*sizeof(int));
    if (listaRespostas == NULL)
    {
        printf("--ERRO LISTA RESPOSTA DINAMICA");
        getchar();
        exit(1);
    }

    //Para todas as linhas -1, o valor da resposta é o valor da matriz de dados na linha i+1 (pulando cabeçalho) na ultima coluna
    //Se o valor da ultima coluna for o respostaPositiva, coloque como 1, se não, como 0
    //TALVEZ DE ERRO!!!
    for (int i =0; i<numeroLinhas-1; i++)
    {
        if (matrizDados[i+1][numeroFeatures-1][0] == respostaPositiva)
        {
            listaRespostas[i] = 1;
        }
        else {listaRespostas[i]=0;}
    }

    //Transformar de matrizDados[i+1][coluna] até matrizDados[numeroLinhas][coluna] em floats
    //Para isso vamos precisar de uma matriz float "numeroLinhas-1" por 2
    buffMatriz = (float*)malloc((numeroLinhas-1)*sizeof(float));
    if (buffMatriz == NULL)
    {
        printf("--Erro buffmatrizint dinamica");
        getchar();
        exit (1);
    }

    //Para todas as linhas - 1, o valor de buffmatriz é o valor da da matriz de dados na linha i+1 (pulando cabeçalho) na coluna selecionada
    for (int i =0; i<numeroLinhas-1; i++)
    {
        buffMatriz[i] = (float)atof(matrizDados[i+1][coluna]);
    }

    //Para todas as linhas de buffMatriz, anotar menor valor e seu indice em matrizOrdenada
    for (int i=0; i < (numeroLinhas-1); i++)
    {
        float menorNumero = 0.0f;
        int indice = -1;
        int found = 0;

        for (int j = 0; j < (numeroLinhas-1); j++)
        {
            if (checkRepeat(indicesExcluidos, j, i)) continue; // already used
            if (!found)
            {
            menorNumero = buffMatriz[j];
                indice = j;
                found = 1;
            } 
            else if (buffMatriz[j] < menorNumero)
            {
                menorNumero = buffMatriz[j];
                indice = j;
            }
        }

    if (!found) {
        // should not happen unless numeroLinhas<=1
        matrizOrdenada[i][0] = 0.0f;
        matrizOrdenada[i][1] = -1;
        indicesExcluidos[i] = -1;
    } else {
        matrizOrdenada[i][0] = menorNumero;
        matrizOrdenada[i][1] = (float)listaRespostas[indice];
        indicesExcluidos[i] = indice;
    }
    }
    for (int i=0;i<(numeroLinhas-1);i++)
    {
        printf("matrizOrdenada[%d][0]=%f\n",i,matrizOrdenada[i][0]);
        printf("matrizOrdenada[%d][1]=%f\n",i,matrizOrdenada[i][1]);
    }

    //Encerrar função e desalocar a bendita memoria
    free(indicesExcluidos);
    free(listaRespostas);
    free(buffMatriz);
    return matrizOrdenada;
}

int main()
{
    //Arquivos
    FILE* FILE_test = NULL;
    FILE* FILE_Train = NULL;

    int numeroFeatures=0;
    int numeroLinhas=0;

    //Matriz de string que armazena conteudo de uma dada linha e coluna
    char*** trainTable = NULL;
    char charBuff;

    //Abrir FILE_Train
    FILE_Train = fopen("train.csv","r");
    if (FILE_Train == NULL)
    {
        printf("Erro Abertura train.csv\n");
        exit (1);
    }

    //Contar quantidade de features 
    while (charBuff != '\n')
    {
        charBuff = fgetc(FILE_Train);
        if (charBuff == ','){numeroFeatures++;}
    }
    numeroFeatures++; //Como ultima feature não virgula, somamos 1
    printf("--%d Features detectadas\n",numeroFeatures);

    //Contar quantidade de linhas
    rewind(FILE_Train);
    charBuff='0';
    while (charBuff != EOF)
    {
        charBuff = fgetc(FILE_Train);
        if (charBuff == '\n'){numeroLinhas++;}
    }
    printf("--%d Linhas detectadas\n",numeroLinhas);
    rewind(FILE_Train);

    //Alocar linhas e colunas
    trainTable = (char***)malloc(numeroLinhas*sizeof(char**));
    if (trainTable==NULL)
    {
        printf("ERRO aDN table");
        exit (1);
    }

    for (int i=0; i<numeroLinhas; i++)
    {
        trainTable[i] = (char**)malloc(numeroFeatures*sizeof(char*));
        if (trainTable[i]==NULL)
        {
            printf("ERRO aDN table[i]");
            exit (1);
        }

        //Alocar uma quantidade de caracteres para cada string
        for(int j=0; j<numeroFeatures; j++)
        {
            trainTable[i][j] = (char*)malloc(tamanhoStrings*sizeof(char));
            if (trainTable[i][j]==NULL)
            {
                printf("ERRO aDN table[i][j]");
                exit (1);
            }

        }
    }
    printf("-- Matriz de strings de tamanho[%d][%d] alocada\n",numeroLinhas,numeroFeatures);

    //Preencher a matriz com os valores
    //Utilizei Copilot para me ajudar com o scanf
    for (int i=0; i<numeroLinhas;i++)
    {
        for(int j=0;j<numeroFeatures;j++)
        {
            //Se for a ultima feature, deve procurar até \n
            if (j < numeroFeatures-1) {
                fscanf(FILE_Train,"%[^,],",trainTable[i][j]);
            } else {
                fscanf(FILE_Train,"%[^\n]\n",trainTable[i][j]);
            }
        }
    }

    printf("\n\n--%c--",trainTable[1][451][0]);
    giniImpurity(numeroLinhas,numeroFeatures,trainTable);

    //Encerramento do programa
    for (int i = 0;i<numeroLinhas;i++)
    {
        for(int j =0; j<numeroFeatures; j++)
        {
            free(trainTable[i][j]);
        }
        free(trainTable[i]);
    }
    free(trainTable);

    getchar(); //para terminal não fechar sozinho
    fclose(FILE_Train);
    return 0;
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