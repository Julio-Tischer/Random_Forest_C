#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define tamanhoStrings 30
#define respostaPositiva ('1')//Aqui coloque o valor para considerar como positivo

int checkRepeat (int* vector, int new_entry, int sizeOf_Vector);
float** sortSTG (char*** matrizDados,int coluna, int numeroLinhas, int numeroFeatures);
float* da_FloArr (float* variable, int size);
int* da_IntArr (int* variable, int size);

//MEU DEUS STRUCTS SÃO MUITO TOPPPPPPPPPPPPPPP
struct giniOutput calcGini(int ID, float threshold, float best_Threshold, float** featureTable, int lenghtOf_Table);
struct ordemFeature sortByGini(struct ordemFeature input, int lineNumber, int featurenumber);
//Esse struct armazena uma lista de ID com seus respectivos IDs e positividades (se abaixo de threshold é positivo ou falso)
struct ordemFeature 
{
    int* ID;
    float* threshold;
    int* isPositive;
    int* isLeaf;
    float* gini;
};

//Essa é estrutura (hehe) do output da função GINI
struct giniOutput
{
    int feature_ID;
    float impurity;
    float threshold;
    int isPositive;
    int isLeaf;
};


//Criar a lista com as features e sua respctivas caracterisitcas
struct ordemFeature giniImpurity(int numeroLinhas, int numeroFeatures, char*** matrizDados)
{
    //Declara um struct para output e um local para armazenar temporaraimente as respostas
    struct ordemFeature outputStruct;
    struct giniOutput giniStruct;

    float threshold=0;
    float best_Threshold=-1; //Definir como -1 pois é o padrão de inicio

    //Alocações
    outputStruct.ID = da_IntArr(outputStruct.ID,numeroFeatures-2);
    outputStruct.isLeaf = da_IntArr(outputStruct.isLeaf,numeroFeatures-2);
    outputStruct.isPositive = da_IntArr(outputStruct.isPositive,numeroFeatures-2);
    outputStruct.threshold = da_FloArr(outputStruct.threshold,numeroFeatures-2);
    outputStruct.gini = da_FloArr(outputStruct.gini,numeroFeatures-2);

    //Vamos precisa de um buffer de matriz int para ordenar nossas features
    //O buffer tera "numeroLinhas" linhas e 2 colunas (para a feature e sua resposta)
    float** bufferListaFeatures = NULL;

    //Para toda feature (menos a resposta e cabeçalho)
    for (int i=0;i<numeroFeatures-2;i++)
    {
        //Seta best_Threshold para padrão de inicio
        best_Threshold =-1;

        //Ordena de menor para maior apartir do cabeçalho
        bufferListaFeatures = sortSTG(matrizDados,i+1,numeroLinhas,numeroFeatures);

        //Calcula os thresholds
        for (int j=0; j<(numeroLinhas-2);j++)
        {
            threshold = ((bufferListaFeatures[j][0]+bufferListaFeatures[j+1][0])/2);

            //ignorar casos onde threshold = bufferListaFeatures[j][0], evitando lista pos vazias
            if (bufferListaFeatures[j][0]!=threshold)
            {
                giniStruct = calcGini(i, threshold, best_Threshold, bufferListaFeatures, (numeroLinhas-1));
                best_Threshold = giniStruct.threshold;
            }
        }
        printf("--Feature %d menor impureza %f com threshold %f\n",i,giniStruct.impurity,giniStruct.threshold);

        //Vamos ter que alocar para cada array da outputStruct
        outputStruct.ID[i]=i;
        outputStruct.isLeaf[i] = giniStruct.isLeaf;
        outputStruct.isPositive[i] = giniStruct.isPositive;
        outputStruct.threshold[i] = giniStruct.threshold;
        outputStruct.gini[i] = giniStruct.impurity;
    }

    //Encerração da funcão (não esqueça de desalocar o bufferLista pvfr)
    
    return outputStruct;
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

    //Para todas as linhas de buffMatriz, anotar menor valor e seu indice em matrizOrdenada (Utilizei ajuda do Copilot)
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
        printf("matrizOrdenada[%d][1]=%.0f\n",i,matrizOrdenada[i][1]);
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
    FILE_Train = fopen("pd_speech_features.csv","r");
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

    struct ordemFeature matrix = giniImpurity(numeroLinhas,numeroFeatures,trainTable);
    matrix = sortByGini(matrix, numeroLinhas, numeroFeatures);

    for (int i=0; i<numeroFeatures-2;i++)
    {
        printf("--matrix.gine[%d] %f\t\t--matrix.ID[%d] %d\n",i,matrix.gini[i],i,matrix.ID[i]);
    }

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
            //printf("\n---NUMERO %d E REPETIDO---\n",new_entry);
        }
        else{}
    }

    //Se houver repetido, retorna 1;
    return flag;
}

//Função que calcula melhor gini dada uma lista de feature, use best_Threshold = -1 para condição inicial
struct giniOutput calcGini(int ID, float threshold, float best_Threshold, float** featureTable, int lenghtOf_Table)
{
    struct giniOutput outputStruct;

    //Os contadores
    float counter_PrePos=0;
    float counter_PreFal=0;
    float counter_PosPos=0;
    float counter_PosFal=0;

    float gini_Pre =0.0;
    float gini_Pos =0.0;
    float gini_Final =0.0;
    float gini_Best =0.0;
    int is_Best =0;

    //Condição de inicio
    if (best_Threshold==-1)
    {
        //Percorra a mesa e incremente os contadores conforema necessario
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (featureTable[i][0]<=threshold)
            {
                if (featureTable[i][1]==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (featureTable[i][1]==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }

        //Calcular GINI pre e pos;
        gini_Pre = 1-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Final= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        is_Best=1;       
    }

    else
    {
        //Calcular impureza de best_Threshold, reutilizarei counter_pre e pos para isso
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (featureTable[i][0]<=best_Threshold)
            {
                if (featureTable[i][1]==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (featureTable[i][1]==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }
        //Calcular GINI pre e pos do best;
        gini_Pre = 1-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Best= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        //isPositive sera definido inicialmente como o isPositive do Best, caso o novo threshold seja melhor, sera substituido
        if (counter_PreFal>counter_PrePos){outputStruct.isPositive=1;}
        else {outputStruct.isPositive=0;}

        if (gini_Pre<gini_Pos){outputStruct.isLeaf = 0;} //Se pre é mais puro que pos, pre é folha
        else {outputStruct.isLeaf = 1;}                 //Se não pos é folha

        //Resetar contadores
        counter_PosFal=0; counter_PosPos=0; counter_PreFal=0; counter_PrePos=0;

        //Calcular impureza do novo threshold
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (featureTable[i][0]<=threshold)
            {
                if (featureTable[i][1]==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (featureTable[i][1]==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }
        gini_Pre = 1.0-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1.0-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Final= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        //Se o novo gini for menor que o melhor, is_Best é 1;
        if (gini_Final<gini_Best)
        {
            is_Best=1;
        }
        else {is_Best=0;}
    }

    //Se for melhor que threshold anterior, substituir valores, se não, manter.
    if (is_Best==1)
    {
        outputStruct.feature_ID = ID;
        outputStruct.threshold = threshold;
        outputStruct.impurity = gini_Final;

        if (gini_Pre<gini_Pos){outputStruct.isLeaf = 0;} //Se pre é mais puro que pos, pre é folha
        else {outputStruct.isLeaf = 1;}                 //Se não pos é folha

        //Rodamos o .isPositive aqui pois os counter contem o valor do THR novo, assim substituindo o valor dado por best_THR
        if (counter_PreFal>counter_PrePos){outputStruct.isPositive=1;}
        else {outputStruct.isPositive=0;}
    }

    else
    {
        outputStruct.feature_ID = ID;
        outputStruct.threshold = best_Threshold;
        outputStruct.impurity = gini_Best;
    }

    return outputStruct;
}

//Vou usar isso para alocar vetores int
int* da_IntArr (int* variable, int size)
{
    variable = (int*)malloc(size*sizeof(int));
    if(variable == NULL)
    {
        printf("--Erro alocação da_IntArr");
        getchar();
        exit(1);
    }

    return variable;
}

//Vou usar isso para alcoar vetores float
float* da_FloArr (float* variable, int size)
{
    variable = (float*)malloc(size*sizeof(float));
    if(variable == NULL)
    {
        printf("--Erro alocação da_IntArr");
        getchar();
        exit(1);
    }

    return variable;
}

//Funcção que ordena o struct ordemFeature de acordo com gini crescente
struct ordemFeature sortByGini(struct ordemFeature input, int lineNumber, int featurenumber)
{
    struct ordemFeature localStruct;
    int* IDs_Excluidos = NULL;
    float menor_Gini = 2; //Gini não pode ser maior q 1
    int ID_Menor=0;
    float threshold_Menor=0;
    int isPositive_Menor=0;
    int isLeaf_Menor=0;

    //Inicialização de vetores
    localStruct.gini = da_FloArr(localStruct.gini,featurenumber-2);
    localStruct.threshold = da_FloArr(localStruct.threshold,featurenumber-2);
    localStruct.ID = da_IntArr(localStruct.ID,featurenumber-2);
    localStruct.isLeaf = da_IntArr(localStruct.isLeaf,featurenumber-2);
    localStruct.isPositive = da_IntArr(localStruct.isPositive,featurenumber-2);

    IDs_Excluidos = da_IntArr(IDs_Excluidos, featurenumber-2);
    //Preencher matriz com ID impossiveis
    for(int i=0;i<featurenumber-2;i++)
    {
        IDs_Excluidos[i]=-1;
    }
    
    //Para toda feature
    for (int i=0; i<featurenumber-2;i++)
    {
        menor_Gini=2;
        //Compara todas as feautures e acha aquela com menor gini e com ID não excluido
        for(int j=0; j<featurenumber-2;j++)
        {
            if(input.gini[j]<menor_Gini&&checkRepeat(IDs_Excluidos,input.ID[j], featurenumber)!=1)
            {
                //Anota os dados do menor gini atual
                ID_Menor = input.ID[j];
                menor_Gini = input.gini[j];
                isLeaf_Menor = input.isLeaf[j];
                isPositive_Menor = input.isPositive[j];
                threshold_Menor = input.threshold[j];
            }
        }
        //Adiciona a feature na primeria entrada junto com suas caracteristicas, e adiciona ID aos excluidos
        IDs_Excluidos[i] = ID_Menor;

        localStruct.gini[i] = menor_Gini;
        localStruct.ID[i] = ID_Menor;
        localStruct.isLeaf[i] = isLeaf_Menor;
        localStruct.isPositive[i] = isPositive_Menor;
        localStruct.threshold[i] = threshold_Menor;
    }

    return localStruct;
}